// Model.cpp: implementation of the CModel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sparkedit.h"
#include "Model.h"
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

CModel::CModel()
{
  m_pMapFile = NULL;
  m_Magic = 0;

  m_pMarkers = NULL;
  m_pShaderDesc = NULL;
  m_pRegions = NULL;
  m_pShaderIndex = NULL;
  m_pRenderTextureIndex = NULL;
  m_BoundingRadius = 0;
  m_TriCount = 0;
  m_Report.IndexOffsetStart = 0;
  m_Report.IndexOffsetEnd = 0;
  m_Report.VertexOffsetStart = 0;
  m_Report.VertexOffsetEnd = 0;
}

CModel::~CModel()
{
  Cleanup();
}

void CModel::Initialize(CFile *pMapFile, UINT magic, CString name)
{
  m_pMapFile = pMapFile;
  m_Magic = magic;
  m_ModelName = name;
}

void CModel::Cleanup()
{
  int i;

  m_pMapFile = NULL;
  m_Magic = 0;

  if(m_pMarkers)
    delete [] m_pMarkers;
  m_pMarkers = NULL;

  if(m_pShaderDesc)
    delete [] m_pShaderDesc;
  m_pShaderDesc = NULL;

  if(m_pRenderTextureIndex)
    delete [] m_pRenderTextureIndex;
  m_pRenderTextureIndex = NULL;

  if(m_pShaderIndex)
    delete [] m_pShaderIndex;
  m_pShaderIndex = NULL;

  if(m_pRegions)
    delete [] m_pRegions;
  m_pRegions = NULL;

  if(m_ppPermutations)
  {
    for(i=0; i<m_Header.Regions.Count; i++)
    {
      delete [] m_ppPermutations[i];
    }

    delete m_ppPermutations;
    m_ppPermutations = NULL;
  }

  if(m_ppMarkerList)
  {
    for(i=0; i<m_Header.Markers.Count; i++)
    {
      delete [] m_ppMarkerList[i];
    }

    delete m_ppMarkerList;
    m_ppMarkerList = NULL;
  }

  m_ShaderNames.RemoveAll();
  m_BoundingRadius = 0;
  m_TriCount = 0;
  //m_ModelName = "";
}

void CModel::LoadModel(UINT model_offset, UINT size)
{
  m_BoundingBox.max[0] = -40000;
  m_BoundingBox.max[1] = -40000;
  m_BoundingBox.max[2] = -40000;

  m_BoundingBox.min[0] = 40000;
  m_BoundingBox.min[1] = 40000;
  m_BoundingBox.min[2] = 40000;

  m_pMapFile->Seek(model_offset, 0);
  m_pMapFile->Read(&m_Header, sizeof(m_Header));

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
  //LoadMeshes();
  UpdateBoundingRadius();
}

void CModel::LoadShaders()
{
  CString temp;

  if(m_Header.Shaders.Count > 0)
  {
    m_pShaderDesc = new SHADER_DESCRIPTION[m_Header.Shaders.Count];
    m_pShaderIndex = new SHADER_INDEX[m_Header.Shaders.Count];

    m_pRenderTextureIndex = new int[m_Header.Shaders.Count];
    ZeroMemory(m_pRenderTextureIndex, 4*m_Header.Shaders.Count);
    ZeroMemory(m_pShaderIndex, sizeof(SHADER_INDEX)*m_Header.Shaders.Count);

    m_pMapFile->Seek(m_Header.Shaders.Offset, 0);
    m_pMapFile->Read(m_pShaderDesc, sizeof(SHADER_DESCRIPTION)*m_Header.Shaders.Count);
  }
}

void CModel::DrawBoundingBox(float *pColor)
{
  float *Min, *Max;

  Min = m_BoundingBox.min;
  Max = m_BoundingBox.max;

  glLineWidth(1);
  glBegin(GL_LINE_STRIP);

  //Sides
  glColor3f(pColor[0], pColor[1], pColor[2]);
  glVertex3f(Min[0], Min[1], Min[2]);
  glVertex3f(Max[0], Min[1], Min[2]);
  glVertex3f(Max[0], Max[1], Min[2]);
  glVertex3f(Min[0], Max[1], Min[2]);
  glVertex3f(Min[0], Min[1], Min[2]);
  
  glVertex3f(Min[0], Min[1], Max[2]);
  glVertex3f(Max[0], Min[1], Max[2]);
  glVertex3f(Max[0], Max[1], Max[2]);
  glVertex3f(Min[0], Max[1], Max[2]);
  glVertex3f(Min[0], Min[1], Max[2]);
  glVertex3f(Min[0], Max[1], Max[2]);
  
  glVertex3f(Min[0], Max[1], Min[2]);
  glVertex3f(Max[0], Max[1], Min[2]);
  glVertex3f(Max[0], Max[1], Max[2]);
  glVertex3f(Max[0], Min[1], Max[2]);
  glVertex3f(Max[0], Min[1], Min[2]);
  glEnd();
}

void CModel::DrawSolidBoundingBox(float *pColor)
{
  float *Min, *Max;

  Min = m_BoundingBox.min;
  Max = m_BoundingBox.max;

  glBegin(GL_QUADS);

  //Sides
  glColor3f(pColor[0]*0.8f,
            pColor[1]*0.8f,
            pColor[2]*0.8f);
  glVertex3f(Min[0], Min[1], Min[2]);
  glVertex3f(Min[0], Max[1], Min[2]);
  glVertex3f(Min[0], Max[1], Max[2]);
  glVertex3f(Min[0], Min[1], Max[2]);

  glVertex3f(Max[0], Max[1], Max[2]);
  glVertex3f(Max[0], Max[1], Min[2]);
  glVertex3f(Max[0], Min[1], Min[2]);
  glVertex3f(Max[0], Min[1], Max[2]);

  glColor3f(pColor[0]*0.6f,
            pColor[1]*0.6f,
            pColor[2]*0.6f);
  glVertex3f(Min[0], Min[1], Min[2]);
  glVertex3f(Max[0], Min[1], Min[2]);
  glVertex3f(Max[0], Min[1], Max[2]);
  glVertex3f(Min[0], Min[1], Max[2]);

  glVertex3f(Max[0], Max[1], Max[2]);
  glVertex3f(Max[0], Max[1], Min[2]);
  glVertex3f(Min[0], Max[1], Min[2]);
  glVertex3f(Min[0], Max[1], Max[2]);

  glColor3f(pColor[0]*0.4f,
            pColor[1]*0.4f,
            pColor[2]*0.4f);
  glVertex3f(Min[0], Min[1], Min[2]);
  glVertex3f(Max[0], Min[1], Min[2]);
  glVertex3f(Max[0], Max[1], Min[2]);
  glVertex3f(Min[0], Max[1], Min[2]);

  glVertex3f(Max[0], Max[1], Max[2]);
  glVertex3f(Max[0], Min[1], Max[2]);
  glVertex3f(Min[0], Min[1], Max[2]);
  glVertex3f(Min[0], Max[1], Max[2]);

  glEnd();
}

void CModel::LoadMarkers()
{
  if(m_Header.Markers.Count > 0)
  {
    m_pMarkers = new MODEL_MARKER[m_Header.Markers.Count];
    m_pMapFile->Seek(m_Header.Markers.Offset, 0);
    m_pMapFile->Read(m_pMarkers, sizeof(MODEL_MARKER)*m_Header.Markers.Count);

    m_ppMarkerList = new MODEL_MARKER_INST*[m_Header.Markers.Count];

    for(int i=0; i<m_Header.Markers.Count; i++)
    {
      m_pMarkers[i].chunk1.Offset -= m_Magic;

      if(m_pMarkers[i].chunk1.Count > 0)
      {
        m_ppMarkerList[i] = new MODEL_MARKER_INST[m_pMarkers[i].chunk1.Count];
        m_pMapFile->Seek(m_pMarkers[i].chunk1.Offset, 0);
        m_pMapFile->Read(m_ppMarkerList[i], sizeof(MODEL_MARKER_INST)*m_pMarkers[i].chunk1.Count);
      }

      //TRACE("Marker(%3d/%3d): %X\n", i, m_pMarkers[i].chunk1.Count, m_pMarkers[i].chunk1.Offset);
    }
  }
}

void CModel::LoadBones()
{

}

void CModel::LoadRegions()
{
  if(m_Header.Regions.Count > 0)
  {
    m_pRegions = new MODEL_REGION[m_Header.Regions.Count];
    m_ppPermutations = new MODEL_REGION_PERMUTATION*[m_Header.Regions.Count];

    m_pMapFile->Seek(m_Header.Regions.Offset, 0);
    m_pMapFile->Read(m_pRegions, sizeof(MODEL_REGION)*m_Header.Regions.Count);

    for(int i=0; i<m_Header.Regions.Count; i++)
    {
      if(m_pRegions[i].Permutations.Offset)
      {
        if(m_pRegions[i].Permutations.Count > 0)
        {
          m_ppPermutations[i] = new MODEL_REGION_PERMUTATION[m_pRegions[i].Permutations.Count];

          m_pRegions[i].Permutations.Offset -= m_Magic;
          m_pMapFile->Seek(m_pRegions[i].Permutations.Offset, 0);
          m_pMapFile->Read(m_ppPermutations[i], sizeof(MODEL_REGION_PERMUTATION)*m_pRegions[i].Permutations.Count);
        }
      }
    }
  }
}

void CModel::UpdateBoundingBox(float *pCoord)
{
  if(pCoord[0] > m_BoundingBox.max[0])m_BoundingBox.max[0] = pCoord[0];
  if(pCoord[1] > m_BoundingBox.max[1])m_BoundingBox.max[1] = pCoord[1];
  if(pCoord[2] > m_BoundingBox.max[2])m_BoundingBox.max[2] = pCoord[2];

  if(pCoord[0] < m_BoundingBox.min[0])m_BoundingBox.min[0] = pCoord[0];
  if(pCoord[1] < m_BoundingBox.min[1])m_BoundingBox.min[1] = pCoord[1];
  if(pCoord[2] < m_BoundingBox.min[2])m_BoundingBox.min[2] = pCoord[2];
}

void CModel::GetBoundingBox(BOUNDING_BOX *pBox)
{
  pBox->min[0] = m_BoundingBox.min[0];
  pBox->min[1] = m_BoundingBox.min[1];
  pBox->min[2] = m_BoundingBox.min[2];

  pBox->max[0] = m_BoundingBox.max[0];
  pBox->max[1] = m_BoundingBox.max[1];
  pBox->max[2] = m_BoundingBox.max[2];
}

void CModel::CompileShader()
{
  CString temp;

  for(int i=0; i<m_Header.Shaders.Count; i++)
  {
    temp = CheckForReference(m_pMapFile, m_pShaderDesc[i].NamePtr, m_Magic);
    m_ShaderNames.Add(temp);
    
    m_pRenderTextureIndex[i] = gTagManager.GetBaseTextureIndex(m_pShaderDesc[i].Shader_TagId);
  }
}

void CModel::UpdateBoundingRadius()
{
  float radius1, radius2;

  radius1 = m_BoundingBox.min[0]*m_BoundingBox.min[0];
  radius1 += m_BoundingBox.min[1]*m_BoundingBox.min[1];
  radius1 += m_BoundingBox.min[2]*m_BoundingBox.min[2];

  radius2 = m_BoundingBox.max[0]*m_BoundingBox.max[0];
  radius2 += m_BoundingBox.max[1]*m_BoundingBox.max[1];
  radius2 += m_BoundingBox.max[2]*m_BoundingBox.max[2];

  if(radius1 > radius2)
    m_BoundingRadius = sqrt(radius1);
  else
    m_BoundingRadius = sqrt(radius2);
}

CString CModel::GetDescription()
{
  CString temp, str;
  float dx, dy, dz;

  temp.Format("Model Name: %s\r\n", m_ModelName);
  str += temp;

  dx = m_BoundingBox.max[0] - m_BoundingBox.min[0];
  dy = m_BoundingBox.max[1] - m_BoundingBox.min[1];
  dz = m_BoundingBox.max[2] - m_BoundingBox.min[2];

  temp.Format("Model Size: %.3f x %.3f x %.3f\r\n", dx, dy, dz);
  str += temp;

  temp.Format("Approx. Model Triangle Count: %d\r\n", m_TriCount);
  str += temp;

  return(str);
}
