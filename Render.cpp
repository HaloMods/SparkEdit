// Render.cpp: implementation of the CRender class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SparkEdit.h"
#include "Render.h"
#include "HaloMapFile.h"
#include "ShaderManager.h"
#include "SelectionManager.h"
#include "BspManager.h"
#include "Frustum1.h"
#include "ModelManager.h"
#include "Perspective.h"
#include "ProfileSettings.h"
#include ".\render.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CHaloMapFile HaloMap;
extern CShaderManager gShaderManager;
extern CSelectionManager gSelection;
extern CBspManager gBspManager;
extern CFrustum gFrustum;
extern CModelManager gModelManager;
extern CPerspective *g_pRenderWnd;
extern CProfileSettings gProfile;

extern PFNGLMULTITEXCOORD2FVARBPROC pglMultiTexCoord2fvARB;
extern PFNGLCLIENTACTIVETEXTUREARBPROC pglClientActiveTextureARB;


CRender::CRender()
{
  m_CamAng = 0.0f;
  m_RenderMode = RENDER_TEXTURED_TRIS;
  ResetMeshColors();
  m_GreenPulseCount = 0;

  //put the find coord outside of z range so it doesn't appear unless asked
  m_FindCoord[0] = 10000;
  m_FindCoord[1] = 10000;
  m_FindCoord[2] = 10000;
  m_RenderTime = 0;
  m_DebugPlaneCounter = 200;
  m_bEnableRender = TRUE;
  m_ModelLOD = SUPER_HIGH;
  m_pPcSkybox = NULL;
  m_pXboxSkybox = NULL;
  m_ActiveMarkerIndex = -1;

  ZeroMemory(m_FrameTimes, sizeof(m_FrameTimes));
  m_FrameIndex = 0;
  m_ActiveRenderModel = -1;
}

CRender::~CRender()
{

}

void CRender::Initialize(int argc, TCHAR *argv[])
{
  /* init opengl stuff */ 
	glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);	
}

void CRender::IdleAnimate()
{
	m_CamAng += 1.0f;

	if (m_CamAng > 360.0f)
	{
		m_CamAng -= 360.0f;
	}
}

void CRender::RenderTextureAndLightmaps(UINT mesh_index)
{
  SUBMESH_INFO *pMesh;
  
  pMesh = gBspManager.GetActiveBspSubmesh(mesh_index);
  
  if(pMesh->ShaderIndex == -1)
  {
    glColor3f(0.1f, 0.1f, 0.1f);
    RenderPcSubmeshLines(mesh_index);
  }
  else
  {
    //gShaderManager.ActivateShader(pMesh->BaseTextureIndex);
    gShaderManager.ActivateLightmapBlend(pMesh->BaseTextureIndex, 0, pMesh->LightmapIndex);

    if(pMesh->LightmapIndex != -1)
      glEnable(GL_TEXTURE_2D);

    glColor3f(1,1,1);
    glVertexPointer(3, GL_FLOAT, 56, pMesh->pVert[0].vertex_k);

    pglClientActiveTextureARB(GL_TEXTURE0_ARB); 
    glTexCoordPointer(2, GL_FLOAT, 56, pMesh->pVert[0].uv); 
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    pglClientActiveTextureARB(GL_TEXTURE1_ARB); 
    glTexCoordPointer(2, GL_FLOAT, 20, &(pMesh->pLightmapVert[0].uv)); 
    glEnableClientState(GL_TEXTURE_COORD_ARRAY); 

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glDrawElements(GL_TRIANGLES, pMesh->IndexCount*3, GL_UNSIGNED_SHORT, pMesh->pIndex);

    glDisableClientState(GL_VERTEX_ARRAY);
    pglClientActiveTextureARB(GL_TEXTURE0_ARB); 
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    pglClientActiveTextureARB(GL_TEXTURE1_ARB); 
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);
  }
}

void CRender::RenderPcSubmeshLines(UINT mesh_index)
{
  SUBMESH_INFO *pMesh;
  int i;

  pMesh = gBspManager.GetActiveBspSubmesh(mesh_index);
  
  glBegin(GL_LINES);
  for(i = 0; i < pMesh->IndexCount; i++)
  {
    glVertex3fv((float *)(pMesh->pVert[pMesh->pIndex[i].tri_ind[0]].vertex_k));
    glVertex3fv((float *)(pMesh->pVert[pMesh->pIndex[i].tri_ind[1]].vertex_k));
    glVertex3fv((float *)(pMesh->pVert[pMesh->pIndex[i].tri_ind[1]].vertex_k));
    glVertex3fv((float *)(pMesh->pVert[pMesh->pIndex[i].tri_ind[2]].vertex_k));
    glVertex3fv((float *)(pMesh->pVert[pMesh->pIndex[i].tri_ind[2]].vertex_k));
    glVertex3fv((float *)(pMesh->pVert[pMesh->pIndex[i].tri_ind[0]].vertex_k));
  }
  glEnd();
}

void CRender::RenderPcFlatShadedPolygons(UINT mesh_index)
{
  SUBMESH_INFO *pMesh;
  int i;

  pMesh = gBspManager.GetActiveBspSubmesh(mesh_index);
  
  glBegin(GL_TRIANGLES);
  for(i = 0; i < pMesh->IndexCount; i++)
  {
    glVertex3fv((float *)(pMesh->pVert[pMesh->pIndex[i].tri_ind[0]].vertex_k));
    glVertex3fv((float *)(pMesh->pVert[pMesh->pIndex[i].tri_ind[1]].vertex_k));
    glVertex3fv((float *)(pMesh->pVert[pMesh->pIndex[i].tri_ind[2]].vertex_k));
  }
  glEnd();
}

void CRender::SetNextMeshColor()
{
  /* Select the color for this mesh */ 
  if(m_PolyColor.red < 0.2)
    m_PolyColor.red = 1;
  if(m_PolyColor.blue < 0.2)
    m_PolyColor.blue = 1;
  if(m_PolyColor.green < 0.2)
    m_PolyColor.green = 1;
  
  if((m_PolyColor.color_count%3)==0)
    m_PolyColor.red -= 0.1f;
  
  if((m_PolyColor.color_count%3)==1)
    m_PolyColor.blue -= 0.1f;
  
  if((m_PolyColor.color_count%3)==2)
    m_PolyColor.green -= 0.1f;
  
  m_PolyColor.color_count++;
  
  glColor3f(m_PolyColor.red, m_PolyColor.green, m_PolyColor.blue);
}

void CRender::ResetMeshColors()
{
  m_PolyColor.red = 1.0;
  m_PolyColor.green = 1.0;
  m_PolyColor.blue = 1.0;
  m_PolyColor.color_count = 0;
}

void CRender::RenderVisibleBsp(BOOL bSelectMode)
{
  UINT mesh_count;
  SUBMESH_INFO *pMesh;
  
  if(m_bEnableRender)
  {
    //SetupFog();
    
    //DrawOriginVector();
    mesh_count = gBspManager.GetActiveBspSubmeshCount();
    
    ResetMeshColors();
    
    for(int i=0; i<mesh_count; i++)
    {
      pMesh = gBspManager.GetActiveBspSubmesh(i);
  
      if((m_RenderMode == RENDER_POINTS)||(m_RenderMode == RENDER_TRIS)||
         (m_RenderMode == RENDER_FLAT_SHADING))
      {
        SetNextMeshColor();
      }

      if(gFrustum.BoxInFrustum(&pMesh->Box))
      {     
        if(bSelectMode)
          gSelection.Selection_BeforeRender(i);
        
        switch(m_RenderMode)
        {
        case RENDER_POINTS:
          RenderPcPoints(i);
          break;
          
        case RENDER_TRIS:
          glLineWidth(1.0f);
          RenderPcSubmeshLines(i);
          break;
          
        case RENDER_FLAT_SHADING:
          RenderPcFlatShadedPolygons(i);
          break;
          
        case RENDER_TEXTURED:
          RenderTexture(i);
		  //RenderTextureAndLightmaps(i);
          break;
          
        case RENDER_TEXTURED_TRIS:
          glLineWidth(2.0f);
          RenderTexture(i);
          glColor3f(0.5,0.5,0.5);
          RenderPcSubmeshLines(i);
          break;

        case RENDER_LIGHTMAP:
          RenderLightmap(i);
          break;

        case RENDER_LIGHTMAP_BLEND:
          RenderTextureAndLightmaps(i);
          break;

        case RENDER_DETAIL_MAPPING:
          RenderDetailMapping(i);
          break;
        }
        
        if(bSelectMode)
          gSelection.Selection_AfterRender();
      }
    }
        
    if(m_RenderMode == RENDER_FLAT_SHADING)
    {
      for(int i=0; i<mesh_count; i++)
      {
        glLineWidth(3.0f);
        glColor3f(0.5,0.5,0.5);
        RenderPcSubmeshLines(i);
      }
    }
    
    //Render Cluster Portals
    //if(gBspManager.m_pBsp)
    //  gBspManager.m_pBsp[gBspManager.m_ActiveBsp].DrawClusterPortals();

    gSelection.RenderSelectionObjects();
    RenderLocation();
  }
}

void CRender::SetRenderMode(RENDER_MODE mode)
{
  m_RenderMode = mode;
}

void CRender::RenderPcPoints(UINT mesh_index)
{
  SUBMESH_INFO *pMesh;
  int i;

  pMesh = gBspManager.GetActiveBspSubmesh(mesh_index);
  
  glBegin(GL_POINTS);
  for(i = 0; i < pMesh->IndexCount; i++)
  {
    glVertex3fv((float *)(pMesh->pVert[pMesh->pIndex[i].tri_ind[0]].vertex_k));
    glVertex3fv((float *)(pMesh->pVert[pMesh->pIndex[i].tri_ind[1]].vertex_k));
    glVertex3fv((float *)(pMesh->pVert[pMesh->pIndex[i].tri_ind[2]].vertex_k));
  }
  glEnd();
}

void CRender::DrawOriginVector()
{
  glBegin(GL_LINES);
  glColor3f(1,1,1);
  glVertex3f(0,0,0);
  glVertex3f(0,0,100);

  glColor3f(1,0,0);
  glVertex3f(0,0,0);
  glVertex3f(0,100,0);
  
  glColor3f(0,1,0);
  glVertex3f(0,0,0);
  glVertex3f(100,0,0);
  glEnd();
}

void CRender::RenderLocation()
{
  glPointSize(5);

  if(m_GreenPulseCount < 10)
    glColor3f(0, 0.1*m_GreenPulseCount, 0);
  else if(m_GreenPulseCount < 20)
    glColor3f(0, 1 - 0.1*(m_GreenPulseCount-10), 0);

  if(m_GreenPulseCount >= 20)
    m_GreenPulseCount = 0;

  m_GreenPulseCount++;
  
  glBegin(GL_POINTS);
  
  glVertex3fv(m_FindCoord);

  glEnd();

 	glPointSize( 1.0f );//reset the original point size
}

void CRender::SetFindCoord(float x, float y, float z)
{
  m_FindCoord[0] = x;
  m_FindCoord[1] = y;
  m_FindCoord[2] = z;
}

/*
void CRender::RenderXboxMap(BOOL bSelectMode)
{
  UINT mesh_count;
  SUBMESH_INFO *pMesh;
  
  if(m_bEnableRender)
  {
    //RenderTestObject();
    
    gSelection.RenderSelectionObjects();
    
    //DrawOriginVector();
    mesh_count = gBspManager.GetActiveBspSubmeshCount();
    
    ResetMeshColors();
    
    for(int i=0; i<mesh_count; i++)
    {
      pMesh = gBspManager.GetActiveBspSubmesh(i);

      if((m_RenderMode == RENDER_POINTS)||(m_RenderMode == RENDER_TRIS)||
         (m_RenderMode == RENDER_FLAT_SHADING))
      {
        SetNextMeshColor();
      }

      if(gFrustum.BoxInFrustum(&pMesh->Box))
      {     

        if(bSelectMode)
          gSelection.Selection_BeforeRender(i);

        switch(m_RenderMode)
        {
        case RENDER_POINTS:
          RenderPcPoints(i);
          break;
          
        case RENDER_TRIS:
          glLineWidth(1.0f);
          RenderPcSubmeshLines(i);
          break;
          
        case RENDER_FLAT_SHADING:
          RenderPcFlatShadedPolygons(i);
          break;
          
        case RENDER_TEXTURED:
          RenderTexture(i);
          break;
          
        case RENDER_TEXTURED_TRIS:
          glLineWidth(2.0f);
          RenderTextureAndLightmaps(i);
          glColor3f(0.5,0.5,0.5);
          RenderPcSubmeshLines(i);
          break;
        }
        if(bSelectMode)
          gSelection.Selection_AfterRender();
      }
    }
    
    if(m_RenderMode == RENDER_FLAT_SHADING)
    {
      for(int i=0; i<mesh_count; i++)
      {
        glLineWidth(3.0f);
        glColor3f(0.5,0.5,0.5);
        RenderPcSubmeshLines(i);
      }
    }
    
    RenderLocation();
  }
}
*/

void CRender::RenderXboxPoints(UINT mesh_index)
{
  SUBMESH_INFO *pMesh;
  int i;

  pMesh = gBspManager.GetActiveBspSubmesh(mesh_index);
  
  glBegin(GL_POINTS);
  for(i = 0; i < pMesh->IndexCount; i++)
  {
    glVertex3fv((float *)(pMesh->pCompVert[pMesh->pIndex[i].tri_ind[0]].vertex_k));
    glVertex3fv((float *)(pMesh->pCompVert[pMesh->pIndex[i].tri_ind[1]].vertex_k));
    glVertex3fv((float *)(pMesh->pCompVert[pMesh->pIndex[i].tri_ind[2]].vertex_k));
  }
  glEnd();
}

void CRender::RenderXboxSubmeshLines(UINT mesh_index)
{
  SUBMESH_INFO *pMesh;
  int i;

  pMesh = gBspManager.GetActiveBspSubmesh(mesh_index);
  
  glBegin(GL_LINES);
  for(i = 0; i < pMesh->IndexCount; i++)
  {
    glVertex3fv((float *)(pMesh->pVert[pMesh->pIndex[i].tri_ind[0]].vertex_k));
    glVertex3fv((float *)(pMesh->pVert[pMesh->pIndex[i].tri_ind[1]].vertex_k));
    glVertex3fv((float *)(pMesh->pVert[pMesh->pIndex[i].tri_ind[1]].vertex_k));
    glVertex3fv((float *)(pMesh->pVert[pMesh->pIndex[i].tri_ind[2]].vertex_k));
    glVertex3fv((float *)(pMesh->pVert[pMesh->pIndex[i].tri_ind[2]].vertex_k));
    glVertex3fv((float *)(pMesh->pVert[pMesh->pIndex[i].tri_ind[0]].vertex_k));
  }
  glEnd();
}

void CRender::RenderXboxFlatShadedPolygons(UINT mesh_index)
{
  SUBMESH_INFO *pMesh;
  int i;

  pMesh = gBspManager.GetActiveBspSubmesh(mesh_index);

  glBegin(GL_TRIANGLES);
  for(i = 0; i < pMesh->IndexCount; i++)
  {
    glVertex3fv((float *)(pMesh->pCompVert[pMesh->pIndex[i].tri_ind[0]].vertex_k));
    glVertex3fv((float *)(pMesh->pCompVert[pMesh->pIndex[i].tri_ind[1]].vertex_k));
    glVertex3fv((float *)(pMesh->pCompVert[pMesh->pIndex[i].tri_ind[2]].vertex_k));
  }
  glEnd();
}

void CRender::RenderXboxTexturedSubmesh(UINT mesh_index)
{
  SUBMESH_INFO *pMesh;
  
  pMesh = gBspManager.GetActiveBspSubmesh(mesh_index);
  
  if(pMesh->ShaderIndex == -1)
  {
    glColor3f(0.1f, 0.1f, 0.1f);
    RenderXboxSubmeshLines(mesh_index);
  }
  else
  {
    gShaderManager.ActivateShader(pMesh->BaseTextureIndex);
    //gShaderManager.ActivateLightmapBlend(pMesh->BaseTextureIndex, 0, pMesh->LightmapIndex);

    if(pMesh->LightmapIndex != -1)
      glEnable(GL_TEXTURE_2D);

    glColor3f(1,1,1);
    glVertexPointer(3, GL_FLOAT, 56, pMesh->pVert[0].vertex_k);

    pglClientActiveTextureARB(GL_TEXTURE0_ARB); 
    glTexCoordPointer(2, GL_FLOAT, 56, pMesh->pVert[0].uv); 
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    //pglClientActiveTextureARB(GL_TEXTURE1_ARB); 
    //glTexCoordPointer(2, GL_FLOAT, 20, &(pMesh->pLightmapVert[0].uv)); 
    //glEnableClientState(GL_TEXTURE_COORD_ARRAY); 

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glDrawElements(GL_TRIANGLES, pMesh->IndexCount*3, GL_UNSIGNED_SHORT, pMesh->pIndex);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);
  }
}

float CRender::GetFrameTime()
{
  return(m_RenderTime);
}

BOOL CRender::IsInFrustum(BOUNDING_BOX *pBox)
{
  return(FALSE);
}

void CRender::EnableRender(BOOL bEnable)
{
  m_bEnableRender = bEnable;
}

void CRender::TimerStart()
{
  m_RenderTimer.ResetTimer();
}

void CRender::TimerEnd()
{
  if((m_FrameIndex % FRAME_RATE_BUFFER_SIZE)==0)
    m_FrameIndex = 0;

  m_FrameTimes[m_FrameIndex] = m_RenderTimer.GetDeltaTime();
  m_FrameIndex++;
}

float CRender::GetFrameRate()
{
  float frame_sum, frame_rate;

  frame_sum = 0;
  for(int i=0; i<FRAME_RATE_BUFFER_SIZE; i++)
  {
    frame_sum += m_FrameTimes[i];
  }

  if(frame_sum != 0)
    frame_rate = (float)FRAME_RATE_BUFFER_SIZE/frame_sum;
  else
    frame_rate = 0;

  return(frame_rate);
}

void CRender::RenderTestObject()
{
  int i = 0;

  gShaderManager.ActivateLightmap(0,m_ActiveRenderModel);
  //gShaderManager.ActivateShader(0x1f);

    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    
    glColor3f(1,1,1);
    //face 1
    glTexCoord2f(0,0);
    glVertex3f(0,0,0);

    glTexCoord2f(1,0);
    glVertex3f(10,0,0);
    
    glTexCoord2f(1,1);
    glVertex3f(10,10,0);

    glTexCoord2f(0,1);
    glVertex3f(0,10,0);

    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void CRender::SetActiveRenderModel(int index)
{
  m_ActiveRenderModel = index;

  if(index != -1)
  {
    BOUNDING_BOX box;
    CPcModel *pPcModel = NULL;
    CXboxModel *pXboxModel = NULL;
    float dim, temp1, temp2;
    
    gModelManager.GetModel(m_ActiveRenderModel,
                           &pPcModel,
                           &pXboxModel);

    if(pPcModel)
      pPcModel->GetBoundingBox(&box);
    else if(pXboxModel)
      pXboxModel->GetBoundingBox(&box);
    
    if(pPcModel||pXboxModel)
    {
      dim = box.max[0] - box.min[0];
      temp1 = box.max[1] - box.min[1];
      temp2 = box.max[2] - box.min[2];

      if(temp1 > dim)
        dim = temp1;

      if(temp2 > dim)
        dim = temp2;

      //g_pRenderWnd->SetCamForModel(TRUE, 10);
      g_pRenderWnd->SetCamForModel(TRUE, dim);
    }
    
  }
  else
  {
    g_pRenderWnd->SetCamForModel(FALSE, 0);
  }
}

BOOL CRender::RenderModel()
{
  float origin[6] = {0,0,0,0,0,0};
  CPcModel *pPcModel = NULL;
  CXboxModel *pXboxModel = NULL;
  BOOL bModelRendered = FALSE;

  //RenderTestObject();

  if(m_ActiveRenderModel != -1)
  {
    bModelRendered = TRUE;
    gModelManager.GetModel(m_ActiveRenderModel,
                           &pPcModel,
                           &pXboxModel);


    //RenderTestObject();
    
    if(pPcModel)
    {
      if(GetAsyncKeyState('N') & 0x8000)
        pPcModel->DrawMarkers(m_ActiveMarkerIndex);
      else
      {
        pPcModel->DrawMarkers(m_ActiveMarkerIndex);
        pPcModel->DrawModel(origin, m_ModelLOD);
      }
    }
    else if(pXboxModel)
    {
      pXboxModel->DrawModel(origin, m_ModelLOD);
    }
  }

  return(bModelRendered);
}

void CRender::SetModelLOD(MODEL_LOD lod)
{
  m_ModelLOD = lod;
}

void CRender::SetSkyboxModel(CPcModel *pPcSkybox, CXboxModel *pXboxSkybox)
{
  m_pPcSkybox = pPcSkybox;
  m_pXboxSkybox = pXboxSkybox;
}

void CRender::SetActiveMarker(int index)
{
  m_ActiveMarkerIndex = index;
}

void CRender::RenderTexture(UINT mesh_index)
{
  SUBMESH_INFO *pMesh;
  
  pMesh = gBspManager.GetActiveBspSubmesh(mesh_index);
  
  if(pMesh->ShaderIndex == -1)
  {
    glColor3f(0.1f, 0.1f, 0.1f);
    RenderXboxSubmeshLines(mesh_index);
  }
  else
  {
    gShaderManager.ActivateSingleTexture(pMesh->BaseTextureIndex);

    glColor3f(1,1,1);
    glVertexPointer(3, GL_FLOAT, 56, pMesh->pVert[0].vertex_k);

    pglClientActiveTextureARB(GL_TEXTURE0_ARB); 
    glTexCoordPointer(2, GL_FLOAT, 56, pMesh->pVert[0].uv); 
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glDrawElements(GL_TRIANGLES, pMesh->IndexCount*3, GL_UNSIGNED_SHORT, pMesh->pIndex);

    glDisableClientState(GL_VERTEX_ARRAY);
    pglClientActiveTextureARB(GL_TEXTURE0_ARB); 
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);
  }
}

void CRender::SetupFog()
{
  bool   gp;						// G Pressed? ( New )
  GLuint filter;						// Which Filter To Use
  GLuint fogMode[]= { GL_EXP, GL_EXP2, GL_LINEAR };	// Storage For Three Types Of Fog
  GLuint fogfilter= 0;					// Which Fog To Use
  GLfloat fogColor[4]= {0.5f, 0.5f, 0.8f, 1.0f};		// Fog Color

  //glClearColor(0.5f,0.5f,0.5f,1.0f);			// We'll Clear To The Color Of The Fog ( Modified )

  glFogi(GL_FOG_MODE, GL_EXP);		// Fog Mode
  glFogfv(GL_FOG_COLOR, fogColor);			// Set Fog Color
  glFogf(GL_FOG_DENSITY, 0.005f);				// How Dense Will The Fog Be
  glHint(GL_FOG_HINT, GL_DONT_CARE);			// Fog Hint Value
  glFogf(GL_FOG_START, 1.0f);				// Fog Start Depth
  glFogf(GL_FOG_END, gProfile.m_BinProfile.CullDistance);				// Fog End Depth
  glEnable(GL_FOG);					// Enables GL_FOG
}

void CRender::RenderDetailMapping(UINT mesh_index)
{
  SUBMESH_INFO *pMesh;
  
  pMesh = gBspManager.GetActiveBspSubmesh(mesh_index);
  
  if(pMesh->ShaderIndex == -1)
  {
    glColor3f(0.1f, 0.1f, 0.1f);
    RenderPcSubmeshLines(mesh_index);
  }
  else
  {
    //gShaderManager.ActivateShader(pMesh->BaseTextureIndex);
    gShaderManager.ActivateDetailBlend(pMesh->BaseTextureIndex,
                                       pMesh->DetailTexture1,
                                       pMesh->DetailTexture2,
                                       gBspManager.m_ActiveBsp);

    glColor3f(1,1,1);
    glVertexPointer(3, GL_FLOAT, 56, pMesh->pVert[0].vertex_k);

    pglClientActiveTextureARB(GL_TEXTURE0_ARB); 
    glTexCoordPointer(2, GL_FLOAT, 56, pMesh->pVert[0].uv); 
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    pglClientActiveTextureARB(GL_TEXTURE1_ARB); 
    glTexCoordPointer(2, GL_FLOAT, 20, &(pMesh->pLightmapVert[0].uv)); 
    glEnableClientState(GL_TEXTURE_COORD_ARRAY); 

    pglClientActiveTextureARB(GL_TEXTURE2_ARB); 
    glTexCoordPointer(2, GL_FLOAT, 20, &(pMesh->pLightmapVert[0].uv)); 
    glEnableClientState(GL_TEXTURE_COORD_ARRAY); 

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glDrawElements(GL_TRIANGLES, pMesh->IndexCount*3, GL_UNSIGNED_SHORT, pMesh->pIndex);

    glDisableClientState(GL_VERTEX_ARRAY);
    pglClientActiveTextureARB(GL_TEXTURE0_ARB); 
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    pglClientActiveTextureARB(GL_TEXTURE1_ARB); 
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    pglClientActiveTextureARB(GL_TEXTURE2_ARB); 
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);
  }
}

void CRender::RenderLightmap(UINT mesh_index)
{
  SUBMESH_INFO *pMesh;
  
  pMesh = gBspManager.GetActiveBspSubmesh(mesh_index);
  
  if(pMesh->ShaderIndex == -1)
  {
    glColor3f(0.1f, 0.1f, 0.1f);
    RenderXboxSubmeshLines(mesh_index);
  }
  else
  {
    TRACE("mesh_index=%4d  lightmap_index=%4d\n", mesh_index, pMesh->LightmapIndex);
    gShaderManager.ActivateLightmap(gBspManager.m_ActiveBsp, pMesh->LightmapIndex);

    glColor3f(1,1,1);
    glVertexPointer(3, GL_FLOAT, 56, pMesh->pVert[0].vertex_k);

    pglClientActiveTextureARB(GL_TEXTURE0_ARB); 
    glTexCoordPointer(2, GL_FLOAT, 20, &(pMesh->pLightmapVert[0].uv)); 
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glDrawElements(GL_TRIANGLES, pMesh->IndexCount*3, GL_UNSIGNED_SHORT, pMesh->pIndex);

    glDisableClientState(GL_VERTEX_ARRAY);
    pglClientActiveTextureARB(GL_TEXTURE0_ARB); 
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);
  }
}
