// Model.cpp: implementation of the CPcModel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sparkedit.h"
#include "PcModel.h"
#include "ShaderManager.h"
#include "TagManager.h"
#include "math.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CShaderManager gShaderManager;
extern CTagManager gTagManager;

CPcModel::CPcModel()
{
  m_pModels = NULL;
  m_pMesh = NULL;
  m_pBones = NULL;
  m_pMarkers = NULL;
  m_pShaderIndex = NULL;


  m_IndexOffset = 0;
  m_VertOffset = 0;
  m_TagId = 0;
  ZeroMemory(&m_BoundingBox, sizeof(m_BoundingBox));
  m_TriCount = 0;
}

CPcModel::~CPcModel()
{
  Cleanup();
}

void CPcModel::Initialize(CFile *pMapFile, UINT magic, UINT VertOffset, UINT IndexOffset, CString name)
{
  m_pMapFile = pMapFile;
  m_Magic = magic;
  m_IndexOffset = IndexOffset;
  m_VertOffset = VertOffset;
  m_ModelName = name;
}

void CPcModel::Cleanup()
{
  int i, j;

  m_pMapFile = NULL;
  m_Magic = 0;

  CModel::Cleanup();

  if(m_pModels)
    delete [] m_pModels;
  m_pModels = NULL;

  if(m_pBones)
    delete [] m_pBones;
  m_pBones = NULL;

  if(m_pMarkers)
    delete [] m_pMarkers;
  m_pMarkers = NULL;

  if(m_pShaderIndex)
    delete [] m_pShaderIndex;
  m_pShaderIndex = NULL;

  if(m_pMesh)
  {
    for(i=0; i<m_Header.Geometries.Count; i++)
    {
      for(j=0; j<m_pMesh[i].header.SubmeshHeaders.Count; j++)
      {
        delete [] m_pMesh[i].ppVert[j];
        delete [] m_pMesh[i].ppIndex[j];
      }
      delete [] m_pMesh[i].ppIndex;
      delete [] m_pMesh[i].ppVert;
      delete [] m_pMesh[i].pSubmeshes;
    }

    delete [] m_pMesh;
  }
  m_pMesh = NULL;

  //m_ModelName = "";
  m_BoundingRadius = 0;
}

void CPcModel::LoadModel(UINT model_offset, UINT size)
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

  LoadMarkers();
  LoadBones();
  LoadShaders();
  LoadRegions();
  LoadModels();
  LoadMeshes();
  UpdateBoundingRadius();
}

void CPcModel::LoadMeshes()
{
  int i;

  if(m_Header.Geometries.Count > 0)
  {
    m_pMesh = new PCMODEL_SUBMESH_INFO[m_Header.Geometries.Count];
    ZeroMemory(m_pMesh, sizeof(PCMODEL_SUBMESH_INFO)*m_Header.Geometries.Count);

    m_TriCount = 0;
    for(i=0; i<m_Header.Geometries.Count; i++)
    {
      m_pMapFile->Seek(m_Header.Geometries.Offset + sizeof(MODEL_GEOMETRY_HEADER)*i, 0);
      m_pMapFile->Read(&m_pMesh[i].header, sizeof(MODEL_GEOMETRY_HEADER));

      if(m_pMesh[i].header.SubmeshHeaders.Count > 0)
      {
        m_pMesh[i].ppVert = new PCMODEL_VERT*[m_pMesh[i].header.SubmeshHeaders.Count];
        m_pMesh[i].ppIndex = new USHORT*[m_pMesh[i].header.SubmeshHeaders.Count];

        m_pMesh[i].header.SubmeshHeaders.Offset -= m_Magic;

        m_pMesh[i].pSubmeshes = new PCMODEL_SUBMESH_HEADER[m_pMesh[i].header.SubmeshHeaders.Count];
        m_pMapFile->Seek(m_pMesh[i].header.SubmeshHeaders.Offset, 0);
        m_pMapFile->Read(m_pMesh[i].pSubmeshes, sizeof(PCMODEL_SUBMESH_HEADER)*m_pMesh[i].header.SubmeshHeaders.Count);

        LoadSubmodels(&m_pMesh[i]);
      }
    }
  }
}

void CPcModel::LoadSubmodels(PCMODEL_SUBMESH_INFO *pMesh)
{
  int j;

  m_Report.IndexOffsetStart = 0;
  m_Report.IndexOffsetEnd = 0;
  m_Report.VertexOffsetStart = 0;
  m_Report.VertexOffsetEnd = 0;

  for(j=0; j<pMesh->header.SubmeshHeaders.Count; j++)
  {
    pMesh->pSubmeshes[j].IndexOffset += m_IndexOffset;
    //pMesh->pSubmeshes[j].IndexOffset2 -= m_Magic;
    pMesh->pSubmeshes[j].VertexRefOffset += m_VertOffset;

    if(j == 0)
    {
      m_Report.IndexOffsetStart = pMesh->pSubmeshes[j].IndexOffset;
      m_Report.VertexOffsetStart = pMesh->pSubmeshes[j].VertexRefOffset;
    }
    
    if(j == (pMesh->header.SubmeshHeaders.Count-1))
    {
      m_Report.IndexOffsetEnd = pMesh->pSubmeshes[j].IndexOffset + sizeof(USHORT)*pMesh->pSubmeshes[j].IndexCount;
      m_Report.VertexOffsetEnd = pMesh->pSubmeshes[j].VertexRefOffset + sizeof(PCMODEL_VERT)*pMesh->pSubmeshes[j].VertexCount;
    }

    m_TriCount += (pMesh->pSubmeshes[j].IndexCount - 1);
//    printf("Ind1: %8X  Ind2: %8X   Vert: %8X  (indcount = %5d  cnt2=%5d,  vertcount = %5d  unk2[0]=%d)\n",
//      pMesh->pSubmeshes[j].IndexOffset,
//      pMesh->pSubmeshes[j].IndexOffset2,
//      pMesh->pSubmeshes[j].VertexRefOffset,
//      pMesh->pSubmeshes[j].IndexCount,
//      pMesh->pSubmeshes[j].count2,
//      pMesh->pSubmeshes[j].VertexCount,
//      pMesh->pSubmeshes[j].unk2[0]);

    if(pMesh->pSubmeshes[j].VertexCount > 0)
    {
      pMesh->ppVert[j] = new PCMODEL_VERT[pMesh->pSubmeshes[j].VertexCount];
      pMesh->ppIndex[j] = new USHORT[pMesh->pSubmeshes[j].IndexCount];

      m_pMapFile->Seek(pMesh->pSubmeshes[j].IndexOffset, 0);
      m_pMapFile->Read(pMesh->ppIndex[j], sizeof(USHORT)*pMesh->pSubmeshes[j].IndexCount);

      m_pMapFile->Seek(pMesh->pSubmeshes[j].VertexRefOffset, 0);
      m_pMapFile->Read(pMesh->ppVert[j], sizeof(PCMODEL_VERT)*pMesh->pSubmeshes[j].VertexCount);
 
      for(int x=0; x<pMesh->pSubmeshes[j].VertexCount; x++)
        UpdateBoundingBox(pMesh->ppVert[j][x].coord);
    }
  }
}

void CPcModel::DrawModel(float *pCoord, MODEL_LOD lod)
{
  int permutation = 0;

  for(int region=0; region<m_Header.Regions.Count; region++)
  {
    DrawTexturedSubmesh(&m_pMesh[m_ppPermutations[region][permutation].LOD_MeshIndex[lod]]);
  }
}

void CPcModel::DrawTexturedSubmesh(PCMODEL_SUBMESH_INFO *pMesh)
{
  int s, i;
  float u, v;


  for(s=0; s<pMesh->header.SubmeshHeaders.Count; s++)
  {
    if(m_pRenderTextureIndex[pMesh->pSubmeshes[s].ShaderIndex] != -1)
    {
      gShaderManager.ActivateShader(m_pRenderTextureIndex[pMesh->pSubmeshes[s].ShaderIndex]);
      glColor4f(1,1,1,0);
      glEnable(GL_BLEND);      
//      glBlendFunc(GL_SRC_ALPHA,GL_ONE);
      glEnable(GL_TEXTURE_2D);
    }
    else
    {
      glColor4f(0.5f,0.5f,0.5f, 0.5f);
      glEnable(GL_BLEND);
      int err = glGetError();
    }

    glBegin(GL_TRIANGLE_STRIP);
    glColor3f(1,1,1);

    for(i=0; i<pMesh->pSubmeshes[s].IndexCount; i++)
    {
      u = (pMesh->ppVert[s][pMesh->ppIndex[s][i]].u)*m_Header.u_scale;
      v = (pMesh->ppVert[s][pMesh->ppIndex[s][i]].v)*m_Header.v_scale;
      glTexCoord2f(u,v);
      glVertex3fv((float *)(pMesh->ppVert[s][pMesh->ppIndex[s][i]].coord));
    }      

//    u = (pMesh->ppVert[s][pMesh->ppIndex[s][0]].u)*m_Header.u_scale;
//    v = (pMesh->ppVert[s][pMesh->ppIndex[s][0]].v)*m_Header.v_scale;
//    glTexCoord2f(u,v);
//    glVertex3fv((float *)(pMesh->ppVert[s][pMesh->ppIndex[s][0]].coord));
    
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
  }
}

void CPcModel::DrawSubmeshLines(PCMODEL_SUBMESH_INFO *pMesh)
{
  int s, i;

  for(s=0; s<pMesh->header.SubmeshHeaders.Count; s++)
  {
    glBegin(GL_LINES);
    glColor3f(1,1,1);

    for(i=0; i<pMesh->pSubmeshes[s].IndexCount; i++)
    {
      glVertex3fv((float *)(pMesh->ppVert[s][pMesh->ppIndex[s][i]].coord));
    }      
    glEnd();
  }
}

void CPcModel::LoadBones()
{
  if(m_Header.Nodes.Count > 0)
  {
    m_pBones = new PCMODEL_BONE[m_Header.Nodes.Count];
    m_pMapFile->Seek(m_Header.Nodes.Offset, 0);
    m_pMapFile->Read(m_pBones, sizeof(PCMODEL_BONE)*m_Header.Nodes.Count);
  }
}

void CPcModel::LoadModels()
{
  if(m_Header.Regions.Count > 0)
  {
    m_pModels = new PCMODEL_MODEL[m_Header.Regions.Count];
    m_pMapFile->Seek(m_Header.Regions.Offset, 0);
    m_pMapFile->Read(m_pModels, sizeof(PCMODEL_MODEL)*m_Header.Regions.Count);

    for(int i=0; i<m_Header.Regions.Count; i++)
    {
      m_pModels[i].chunk1.Offset -= m_Magic;
      //m_pModels[i].chunk2.Offset -= m_Magic;

      CString temp = m_pModels[i].name1;
      //printf("1   %8X  %32s\n", m_pChunk2[i].chunk1.Offset, temp);
      temp = m_pModels[i].name2;
      //printf("2   %8X  %32s\n", m_pChunk2[i].chunk2.Offset, temp);
    }
  }
}

void CPcModel::DrawNodes()
{
  //Draw Node boxes
  GLUquadricObj* pTemp = gluNewQuadric();
  PCMODEL_BONE *pCurrentNode;
  float dd[3];
  float np[3];

  glColor3f(1,0,1);
  for(int i=0; i<m_Header.Nodes.Count; i++)
  {
    //glPushMatrix();

    np[0] = m_pBones[i].Translation[0];
    np[1] = m_pBones[i].Translation[1];
    np[2] = m_pBones[i].Translation[2];

    pCurrentNode = &m_pBones[i];
    
    while(pCurrentNode->ParentNode != -1)
    {
      
      np[0] += pCurrentNode[pCurrentNode->ParentNode].Translation[0];
      np[1] += pCurrentNode[pCurrentNode->ParentNode].Translation[1];
      np[2] += pCurrentNode[pCurrentNode->ParentNode].Translation[2];

      pCurrentNode = &m_pBones[pCurrentNode->ParentNode];
    }

    glPointSize(10);
    glBegin(GL_POINTS);
    glVertex3f(np[0], np[2], np[1]);
    glEnd();
    //glTranslatef(dd[0], dd[1], dd[2]);

    //gluSphere(pTemp, 0.01, 10, 10);
    //glPopMatrix();

    if(0)//m_pBones[i].ParentNode != -1)
    {
      glLineWidth(1);
      glBegin(GL_LINES);
      glVertex3f(np[0],
                 np[1],
                 np[2]);

      glVertex3f(np[0] - m_pBones[m_pBones[i].ParentNode].Translation[0],
                 np[1] - m_pBones[m_pBones[i].ParentNode].Translation[1],
                 np[2] - m_pBones[m_pBones[i].ParentNode].Translation[2]);
      glEnd();
    }
  }



  gluDeleteQuadric(pTemp);
}

void CPcModel::DrawMarkers(int active_index)
{
  GLUquadricObj* pTemp = gluNewQuadric();
  float dd[3];

  for(int i=0; i<m_Header.Markers.Count; i++)
  {
    if(active_index == i)
      glColor3f(1,0,0);
    else
      glColor3f(1,1,1);

    for(int j=0; j<m_pMarkers[i].chunk1.Count; j++)
    {
      glPushMatrix();

      dd[0] = m_pBones[m_ppMarkerList[i][j].node_index].Translation[0];
      dd[0] += m_ppMarkerList[i][j].translation[0];

      dd[1] = m_pBones[m_ppMarkerList[i][j].node_index].Translation[1];
      dd[1] += m_ppMarkerList[i][j].translation[1];

      dd[2] = m_pBones[m_ppMarkerList[i][j].node_index].Translation[2];
      dd[2] += m_ppMarkerList[i][j].translation[2];

      glTranslatef(dd[0], 
                   dd[1], 
                   dd[2]);

      gluSphere(pTemp, m_BoundingRadius/50.0, 10, 10);
      glPopMatrix();
    }    
  }

  gluDeleteQuadric(pTemp);
}

CString CPcModel::GetMarkerName(int index)
{
  CString str, temp;

  if(index < m_Header.Markers.Count)
  {
    str = m_pMarkers[index].name1;
    temp.Format(" (%s)", m_pBones[m_ppMarkerList[index][0].node_index].name);
    str += temp;
  }

  return(str);
}

int CPcModel::GetMarkerCount()
{
  return(m_Header.Markers.Count);
}
