// Model.cpp: implementation of the CXboxModel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sparkedit.h"
#include "XboxModel.h"
#include "ShaderManager.h"
#include "TagManager.h"
#include "Math.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CShaderManager gShaderManager;
extern CTagManager gTagManager;
extern PFNGLMULTITEXCOORD2FARBPROC pglMultiTexCoord2fARB;

CXboxModel::CXboxModel()
{
  m_pRegions = NULL;
  m_pMesh = NULL;
  m_pShaderIndex = NULL;
  m_NamePtr = 0;
  m_TriCount = 0;
}

CXboxModel::~CXboxModel()
{
  Cleanup();
}

void CXboxModel::Cleanup()
{
  int i, j;

  m_pMapFile = NULL;
  m_Magic = 0;

  CModel::Cleanup();

  if(m_pShaderIndex)
    delete [] m_pShaderIndex;
  m_pShaderIndex = NULL;

  if(m_pRegions)
    delete [] m_pRegions;
  m_pRegions = NULL;

  if(m_pMesh)
  {
    for(i=0; i<m_Header.Geometries.Count; i++)
    {
      for(j=0; j<m_pMesh[i].header.SubmeshHeaders.Count; j++)
      {
        delete [] m_pMesh[i].ppCmpVert[j];
        delete [] m_pMesh[i].ppTexVert[j];
        delete [] m_pMesh[i].ppIndex[j];
      }

      delete [] m_pMesh[i].ppCmpVert;
      delete [] m_pMesh[i].ppTexVert;
      delete [] m_pMesh[i].ppIndex;

      delete [] m_pMesh[i].pSubmeshes;
    }

    delete [] m_pMesh;
  }
  m_pMesh = NULL;

  m_TriCount = 0;
}

void CXboxModel::LoadModel(UINT model_offset, UINT size)
{
  m_BoundingBox.max[0] = -40000;
  m_BoundingBox.max[1] = -40000;
  m_BoundingBox.max[2] = -40000;

  m_BoundingBox.min[0] = 40000;
  m_BoundingBox.min[1] = 40000;
  m_BoundingBox.min[2] = 40000;

  m_pMapFile->Seek(model_offset, 0);
  m_pMapFile->Read(&m_Header, sizeof(m_Header));

  m_Header.Markers.Offset -= m_Magic;
  m_Header.Nodes.Offset -= m_Magic;
  m_Header.Regions.Offset -= m_Magic;
  m_Header.Geometries.Offset -= m_Magic;
  m_Header.Shaders.Offset -= m_Magic;

  if(m_ModelName == "vehicles\\warthog\\warthog")
    TRACE("warthog");

  LoadMarkers();
  LoadBones();
  LoadShaders();
  LoadRegions();
  LoadMeshes();
  UpdateBoundingRadius();
}

void CXboxModel::LoadMeshes()
{
  int i;

  if(m_Header.Geometries.Count > 0)
  {
    m_pMesh = new XBOX_MODEL_SUBMESH_INFO[m_Header.Geometries.Count];
    ZeroMemory(m_pMesh, 
sizeof(XBOX_MODEL_SUBMESH_INFO)*m_Header.Geometries.Count);

    m_TriCount = 0;
    for(i=0; i<m_Header.Geometries.Count; i++)
    {
      m_pMapFile->Seek(m_Header.Geometries.Offset + 
sizeof(MODEL_GEOMETRY_HEADER)*i, 0);
      m_pMapFile->Read(&m_pMesh[i].header, sizeof(MODEL_GEOMETRY_HEADER));

      if(m_pMesh[i].header.SubmeshHeaders.Count > 0)
      {
        //Allocate compressed vertex pointers
        m_pMesh[i].ppCmpVert = new 
MODEL_COMPRESSED_VERT*[m_pMesh[i].header.SubmeshHeaders.Count];
        m_pMesh[i].ppTexVert = new 
MODEL_UV*[m_pMesh[i].header.SubmeshHeaders.Count];
        m_pMesh[i].ppIndex = new 
USHORT*[m_pMesh[i].header.SubmeshHeaders.Count];

        m_pMesh[i].header.SubmeshHeaders.Offset -= m_Magic;

        m_pMesh[i].pSubmeshes = new 
MODEL_SUBMODEL_HEADER[m_pMesh[i].header.SubmeshHeaders.Count];
        m_pMapFile->Seek(m_pMesh[i].header.SubmeshHeaders.Offset, 0);
        m_pMapFile->Read(m_pMesh[i].pSubmeshes, 
sizeof(MODEL_SUBMODEL_HEADER)*m_pMesh[i].header.SubmeshHeaders.Count);

        LoadSubmodels(&m_pMesh[i]);
      }
    }
  }
}

void CXboxModel::LoadSubmodels(XBOX_MODEL_SUBMESH_INFO *pMesh)
{
  MODEL_VERT_REFERENCE vert_hdr;
  int j;

  for(j=0; j<pMesh->header.SubmeshHeaders.Count; j++)
  {
    pMesh->pSubmeshes[j].IndexOffset -= m_Magic;
    pMesh->pSubmeshes[j].IndexOffset2 -= (m_Magic);
    pMesh->pSubmeshes[j].VertexRefOffset -= (m_Magic - 4);

    m_pMapFile->Seek(pMesh->pSubmeshes[j].VertexRefOffset, 0);
    m_pMapFile->Read(&vert_hdr, 12);
    vert_hdr.unk[0] -= m_Magic;

    if(j == 0)
    {
      m_Report.IndexOffsetStart = pMesh->pSubmeshes[j].IndexOffset;
      m_Report.VertexOffsetStart = vert_hdr.unk[0];
    }
    
    if(j == (pMesh->header.SubmeshHeaders.Count-1))
    {
      m_Report.IndexOffsetEnd = pMesh->pSubmeshes[j].IndexOffset + sizeof(USHORT)*pMesh->pSubmeshes[j].IndexCount;
      m_Report.VertexOffsetEnd = pMesh->pSubmeshes[j].VertexRefOffset + sizeof(MODEL_COMPRESSED_VERT)*pMesh->pSubmeshes[j].VertexCount;
    }

    m_TriCount += (pMesh->pSubmeshes[j].IndexCount - 2);

    //Read in compressed vertices
    if(pMesh->pSubmeshes[j].VertexCount > 0)
    {
      pMesh->ppCmpVert[j] = new MODEL_COMPRESSED_VERT[pMesh->pSubmeshes[j].VertexCount];
      pMesh->ppTexVert[j] = new MODEL_UV[pMesh->pSubmeshes[j].VertexCount];
      pMesh->ppIndex[j] = new USHORT[pMesh->pSubmeshes[j].IndexCount];

      m_pMapFile->Seek(pMesh->pSubmeshes[j].IndexOffset, 0);
      m_pMapFile->Read(pMesh->ppIndex[j], sizeof(USHORT)*pMesh->pSubmeshes[j].IndexCount);

      m_pMapFile->Seek(vert_hdr.unk[0], 0);
      m_pMapFile->Read(pMesh->ppCmpVert[j],
        sizeof(MODEL_COMPRESSED_VERT)*pMesh->pSubmeshes[j].VertexCount);

      //Decompress verts
      for(int v=0; v<pMesh->pSubmeshes[j].VertexCount; v++)
      {
        UpdateBoundingBox(pMesh->ppCmpVert[j][v].coord);
        pMesh->ppTexVert[j][v].u = pMesh->ppCmpVert[j][v].u/32767.0;
        pMesh->ppTexVert[j][v].u = pMesh->ppCmpVert[j][v].v/32767.0;
      }
    }
  }
}


void CXboxModel::DrawModel(float *pCoord, MODEL_LOD lod)
{
  int permutation = 0;

  for(int region=0; region<m_Header.Regions.Count; region++)
  {
    DrawTexturedSubmesh(&m_pMesh[m_ppPermutations[region][permutation].LOD_MeshIndex[lod]]);
  }
}

void CXboxModel::DrawTexturedSubmesh(XBOX_MODEL_SUBMESH_INFO *pMesh)
{
  int s, i;
  float u, v;

  for(s=0; s<pMesh->header.SubmeshHeaders.Count; s++)
  {
    
    gShaderManager.ActivateSingleTexture(m_pRenderTextureIndex[pMesh->pSubmeshes[s].ShaderIndex]);

    glBegin(GL_TRIANGLE_STRIP);
    glColor3f(1,1,1);

    for(i=0; i<pMesh->pSubmeshes[s].IndexCount; i++)
    {
      glVertex3fv((float*)(pMesh->ppCmpVert[s][pMesh->ppIndex[s][i]].coord));

      if(i<(pMesh->pSubmeshes[s].IndexCount-1))
      {
        u = (pMesh->ppCmpVert[s][pMesh->ppIndex[s][i+1]].u/32767.0f)*m_Header.u_scale;
        v = (pMesh->ppCmpVert[s][pMesh->ppIndex[s][i+1]].v/32767.0f)*m_Header.v_scale;
      }
      pglMultiTexCoord2fARB(GL_TEXTURE0_ARB, u,v);
    }
    glEnd();
    gShaderManager.DeactivateSingleTexture();
  }
}

