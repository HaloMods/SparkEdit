#include "stdafx.h"
#include "sparkedit.h"
#include "SelectionManager.h"
#include "Scenario.h"
#include "ProfileSettings.h"
#include "BspManager.h"
#include "Render.h"
#include "Frustum1.h"
#include "Encounters.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CProfileSettings gProfile;
extern CScenario gScenario;
extern CBspManager gBspManager;
extern CRender gRender;
extern CFrustum gFrustum;
extern CEncounters gAI;

CSelectionManager::CSelectionManager()
{
  m_bDrawTriggers = FALSE;
  m_pObj = NULL;
  m_AccCount = 0;
  m_Version = 0;

  SetMaxTranslateSpeed(0.5);


  m_GroupColor[VEHICLES][0] = 0.5f;
  m_GroupColor[VEHICLES][1] = 0.3f;
  m_GroupColor[VEHICLES][2] = 0.3f;

  m_GroupColor[SCENERY][0] = 0;
  m_GroupColor[SCENERY][1] = 0.5f;
  m_GroupColor[SCENERY][2] = 0;
  
  m_GroupColor[WEAPONS][0] = 0.5f;
  m_GroupColor[WEAPONS][1] = 0.7f;
  m_GroupColor[WEAPONS][2] = 0.8f;
  
  m_GroupColor[PLAYER_SPAWNS][0] = 0;
  m_GroupColor[PLAYER_SPAWNS][1] = 0;
  m_GroupColor[PLAYER_SPAWNS][2] = 0.5f;

  m_GroupColor[MP_FLAGS][0] = 0.8f;
  m_GroupColor[MP_FLAGS][1] = 0.5f;
  m_GroupColor[MP_FLAGS][2] = 0;

  m_GroupColor[MP_EQUIPMENT][0] = 0.3f;
  m_GroupColor[MP_EQUIPMENT][1] = 0.5f;
  m_GroupColor[MP_EQUIPMENT][2] = 0.6f;

  m_GroupColor[AI_SPAWNS][0] = 0.8f;
  m_GroupColor[AI_SPAWNS][1] = 0.8f;
  m_GroupColor[AI_SPAWNS][2] = 0.4f;

  m_ReferenceSelection = -1;
  m_CurrentEditorMode = EDITOR_MODE_SELECT;
}

CSelectionManager::~CSelectionManager()
{
  Cleanup();
}

/*-------------------------------------------------------------------
 * Name: RenderActiveSelectionBox()
 * Description:
 *   Renders an individual selection object, including offsets.
 *-----------------------------------------------------------------*/
void CSelectionManager::RenderActiveSelectionBox(int index, float *pCoord, BOOL bSelected, SELECTION_MODE sel_mode)
{
  BOUNDING_BOX box;
  float sbx = SEL_BOX_SIZE;

  float c[3];
  float dist;
  float color_red[3] = {1,1,0};
  float color_yellow[3] = {1,0,0};
  float *pColor;

  c[0] = pCoord[0];
  c[1] = pCoord[1];
  c[2] = pCoord[2];
  
  if(m_pObj[index].bSelected)
    pColor = color_red;
  else
    pColor = color_yellow;
  
  
  glPushMatrix();
  glTranslatef(pCoord[0],
               pCoord[1],
               pCoord[2]);

  //draw movement cursor before rotation1
  if(m_pObj[index].bSelected)
    DrawMovementCursor(index, TRUE, pCoord);

  glRotatef(pCoord[5]*RAD_TO_DEG,
            1,
            0,
            0);

  glRotatef(-pCoord[4]*RAD_TO_DEG,
            0,
            1,
            0);

  glRotatef(pCoord[3]*RAD_TO_DEG,
            0,
            0,
            1);



  CPcModel *pPcModel;
  CXboxModel *pXboxModel;

  gScenario.GetTagModel(sel_mode, index, &pPcModel, &pXboxModel);

  if(pPcModel)
  {
    pPcModel->GetBoundingBox(&box);
    box.min[0] += pCoord[0];
    box.min[1] += pCoord[1];
    box.min[2] += pCoord[2];
    
    box.max[0] += pCoord[0];
    box.max[1] += pCoord[1];
    box.max[2] += pCoord[2];
    
    if(gFrustum.BoxInFrustum(&box))
    {
      dist = CalculateCameraDist(pCoord);

      if(IsHighDetailEnabled(sel_mode, dist))
      {
        pPcModel->DrawBoundingBox(pColor);
        pPcModel->DrawModel(pCoord, SUPER_HIGH);
      }
      else
      {
        pPcModel->DrawSolidBoundingBox(pColor);
      }
    }
  }
  else if(pXboxModel)
  {
    pXboxModel->GetBoundingBox(&box);
    box.min[0] += pCoord[0];
    box.min[1] += pCoord[1];
    box.min[2] += pCoord[2];

    box.max[0] += pCoord[0];
    box.max[1] += pCoord[1];
    box.max[2] += pCoord[2];

    if(gFrustum.BoxInFrustum(&box))
    {
      dist = CalculateCameraDist(pCoord);

      if(IsHighDetailEnabled(sel_mode, dist))
      {
        pXboxModel->DrawBoundingBox(pColor);
        pXboxModel->DrawModel(pCoord, SUPER_HIGH);
      }
      else
      {
        pXboxModel->DrawSolidBoundingBox(pColor);
      }
    }
  }
  else
  {
    glBegin(GL_QUADS);
    
    if(m_pObj[index].bSelected)
      glColor3f(1,1,0);
    else
      glColor3f(1,0,0);
    //face 1
    glVertex3f(-sbx, -sbx, -sbx);
    glVertex3f(-sbx, +sbx, -sbx);
    glVertex3f(-sbx, +sbx, +sbx);
    glVertex3f(-sbx, -sbx, +sbx);
    
    //face 2
    glVertex3f(+sbx, -sbx, -sbx);
    glVertex3f(+sbx, +sbx, -sbx);
    glVertex3f(+sbx, +sbx, +sbx);
    glVertex3f(+sbx, -sbx, +sbx);
    
    if(m_pObj[index].bSelected)glColor3f(0.8f,0.8f,0);
    else glColor3f(0.8f,0,0);
    //face 3
    glVertex3f(-sbx, -sbx, -sbx);
    glVertex3f(+sbx, -sbx, -sbx);
    glVertex3f(+sbx, -sbx, +sbx);
    glVertex3f(-sbx, -sbx, +sbx);
    
    //face 4
    glVertex3f(-sbx, +sbx, -sbx);
    glVertex3f(+sbx, +sbx, -sbx);
    glVertex3f(+sbx, +sbx, +sbx);
    glVertex3f(-sbx, +sbx, +sbx);
    
    if(m_pObj[index].bSelected)glColor3f(0.6f,0.6f,0);
    else glColor3f(0.6f,0,0);
    //face 5
    glVertex3f(-sbx, -sbx, -sbx);
    glVertex3f(+sbx, -sbx, -sbx);
    glVertex3f(+sbx, +sbx, -sbx);
    glVertex3f(-sbx, +sbx, -sbx);
    
    //face 6
    glVertex3f(-sbx, -sbx, +sbx);
    glVertex3f(+sbx, -sbx, +sbx);
    glVertex3f(+sbx, +sbx, +sbx);
    glVertex3f(-sbx, +sbx, +sbx);
  }

  glEnd();

  if((sel_mode == PLAYER_SPAWNS)||(sel_mode == MP_FLAGS))
  {
    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex3f(0, 0, sbx);
    glVertex3f(sbx*2, 0, sbx);

    glVertex3f(sbx*2, 0, sbx);
    glVertex3f(sbx*1.5, sbx/4, sbx);

    glVertex3f(sbx*2, 0, sbx);
    glVertex3f(sbx*1.5, -sbx/4, sbx);

    glEnd();
  }

  glPopMatrix();
}

void CSelectionManager::RenderInactiveEditBox(int index, float *pCoord, SELECTION_MODE sel_mode)
{
  float sbx = SEL_BOX_SIZE;
  float *pColor = m_GroupColor[sel_mode];
  float dist;
  BOUNDING_BOX box;

  glPushMatrix();

  glTranslatef(pCoord[0],
               pCoord[1],
               pCoord[2]);

  glRotatef(pCoord[5]*RAD_TO_DEG,
            1,
            0,
            0);

  glRotatef(-pCoord[4]*RAD_TO_DEG,
            0,
            1,
            0);

  glRotatef(pCoord[3]*RAD_TO_DEG,
            0,
            0,
            1);

  CPcModel *pPcModel;
  CXboxModel *pXboxModel;

  gScenario.GetTagModel(sel_mode, index, &pPcModel, &pXboxModel);

  if(pPcModel)
  {
    pPcModel->GetBoundingBox(&box);
    box.min[0] += pCoord[0];
    box.min[1] += pCoord[1];
    box.min[2] += pCoord[2];

    box.max[0] += pCoord[0];
    box.max[1] += pCoord[1];
    box.max[2] += pCoord[2];

    if(gFrustum.BoxInFrustum(&box))
    {
      dist = CalculateCameraDist(pCoord);
      
      if(IsHighDetailEnabled(sel_mode, dist))
      {
        //pPcModel->DrawBoundingBox(pColor);
        pPcModel->DrawModel(pCoord, SUPER_HIGH);
      }
      else
      {
        pPcModel->DrawSolidBoundingBox(pColor);
      }
    }
  }
  else if(pXboxModel)
  {
    pXboxModel->GetBoundingBox(&box);
    box.min[0] += pCoord[0];
    box.min[1] += pCoord[1];
    box.min[2] += pCoord[2];

    box.max[0] += pCoord[0];
    box.max[1] += pCoord[1];
    box.max[2] += pCoord[2];

    if(gFrustum.BoxInFrustum(&box))
    {  
      dist = CalculateCameraDist(pCoord);
      
      if(IsHighDetailEnabled(sel_mode, dist))
      {
        pXboxModel->DrawBoundingBox(pColor);
        pXboxModel->DrawModel(pCoord, SUPER_HIGH);
      }
      else
      {
        pXboxModel->DrawSolidBoundingBox(pColor);
      }
    }
  }
  else
  {
    glBegin(GL_QUADS);
    
    glColor3f(0.8f*pColor[0], 0.8f*pColor[1], 0.8f*pColor[2]);
    //face 1
    glVertex3f(-sbx, -sbx, -sbx);
    glVertex3f(-sbx, +sbx, -sbx);
    glVertex3f(-sbx, +sbx, +sbx);
    glVertex3f(-sbx, -sbx, +sbx);
    
    //face 2
    glVertex3f(+sbx, -sbx, -sbx);
    glVertex3f(+sbx, +sbx, -sbx);
    glVertex3f(+sbx, +sbx, +sbx);
    glVertex3f(+sbx, -sbx, +sbx);
    
    glColor3f(0.8f*pColor[0], 0.8f*pColor[1], 0.8f*pColor[2]);
    //face 3
    glVertex3f(-sbx, -sbx, -sbx);
    glVertex3f(+sbx, -sbx, -sbx);
    glVertex3f(+sbx, -sbx, +sbx);
    glVertex3f(-sbx, -sbx, +sbx);
    
    //face 4
    glVertex3f(-sbx, +sbx, -sbx);
    glVertex3f(+sbx, +sbx, -sbx);
    glVertex3f(+sbx, +sbx, +sbx);
    glVertex3f(-sbx, +sbx, +sbx);
    
    glColor3f(0.6f*pColor[0], 0.6f*pColor[1], 0.6f*pColor[2]);
    //face 5
    glVertex3f(-sbx, -sbx, -sbx);
    glVertex3f(+sbx, -sbx, -sbx);
    glVertex3f(+sbx, +sbx, -sbx);
    glVertex3f(-sbx, +sbx, -sbx);
    
    //face 6
    glVertex3f(-sbx, -sbx, +sbx);
    glVertex3f(+sbx, -sbx, +sbx);
    glVertex3f(+sbx, +sbx, +sbx);
    glVertex3f(-sbx, +sbx, +sbx);
    
    glEnd();

    if((sel_mode == PLAYER_SPAWNS)||(sel_mode == MP_FLAGS))
    {
      glLineWidth(3);
      glBegin(GL_LINES);
      glVertex3f(0, 0, sbx);
      glVertex3f(sbx*2, 0, sbx);
      
      glVertex3f(sbx*2, 0, sbx);
      glVertex3f(sbx*1.5, sbx/4, sbx);
      
      glVertex3f(sbx*2, 0, sbx);
      glVertex3f(sbx*1.5, -sbx/4, sbx);
      
      glEnd();
    }

  }

  glPopMatrix();
}


void CSelectionManager::DrawMovementCursor(int index, BOOL bEnable, float *pCoord)
{
  float sbx = SEL_BOX_SIZE;

  if(index == m_ReferenceSelection)
  {
    glLineWidth(1);
    glBegin(GL_LINES);
    
    // Draw X-Axis
    if(m_EditMode[0] > 0.5f)
    {
      glColor3f(0,0,1);
      glVertex3f(0, 0, 0);
      glVertex3f(sbx*400, 0, 0);
      
      glColor3f(1,1,1);
      glVertex3f(0, 0, 0);
      glVertex3f(-sbx*400, 0, 0);
    }
    else if(m_EditMode[0] < -0.5f)
    {
      glColor3f(0,0,1);
      glVertex3f(0, 0, 0);
      glVertex3f(-sbx*400, 0, 0);
      
      glColor3f(1,1,1);
      glVertex3f(0, 0, 0);
      glVertex3f(sbx*400, 0, 0);
    }
    else
    {
      glColor3f(1,1,1);
      glVertex3f(sbx*400, 0, 0);
      glVertex3f(-sbx*400, 0, 0);
    }
    
    // Draw Y-Axis
    if(m_EditMode[1] > 0.5f)
    {
      glColor3f(0,1,0);
      glVertex3f(0, 0, 0);
      glVertex3f(0,sbx*400,  0);
      
      glColor3f(1,1,1);
      glVertex3f(0, 0, 0);
      glVertex3f(0, -sbx*400, 0);
    }
    else if(m_EditMode[1] < -0.5f)
    {
      glColor3f(0,1,0);
      glVertex3f(0, 0, 0);
      glVertex3f(0, -sbx*400, 0);
      
      glColor3f(1,1,1);
      glVertex3f(0, 0, 0);
      glVertex3f(0, sbx*400, 0);
    }
    else
    {
      glColor3f(1,1,1);
      glVertex3f(0, sbx*400, 0);
      glVertex3f(0, -sbx*400, 0);
    }
    
    // Draw Z-Axis
    if(m_EditMode[2] > 0.5f)
    {
      glColor3f(1,0,0);
      glVertex3f(0, 0, 0);
      glVertex3f(0, 0, sbx*400);
      
      glColor3f(1,1,1);
      glVertex3f(0, 0, 0);
      glVertex3f(0, 0, -sbx*400);
    }
    else if(m_EditMode[2] < -0.5f)
    {
      glColor3f(1,0,0);
      glVertex3f(0, 0, 0);
      glVertex3f(0, 0, -sbx*400);
      
      glColor3f(1,1,1);
      glVertex3f(0, 0, 0);
      glVertex3f(0, 0, sbx*400);
    }
    else
    {
      glColor3f(1,1,1);
      glVertex3f(0, 0, sbx*400);
      glVertex3f(0, 0,-sbx*400);
    }
    
    glEnd();
  }
}

void CSelectionManager::RenderSelectionObjects()
{
  float coord[6];
  float tred[4] = {0.8f,0,0,0.5f};
  float tyellow[4] = {0.8f,0.8f,0,0.5f};
  float tblue[4] = {0,0,0.8f,0.5f};
  int sel_mode, count;
  int i;
  BOUNDING_BOX box;

  if(m_pObj)
  {
    for(sel_mode=0; sel_mode<SELECTION_MODE_MAX; sel_mode++)
    {
      if(sel_mode ==  m_ActiveSelectionMode)
      {
        if(sel_mode != BSP_MESH_INFO)
        {
          for(i=0; i<m_ActiveCount; i++)
          {
            if(((SELECTION_MODE)sel_mode == TRIGGERS)&&m_bDrawTriggers)
            {
              gScenario.GetTriggerVolume(i, &box);
              Selection_BeforeRender(i);
              if(m_pObj[i].bSelected)
                RenderBox(&box, tyellow);
              else
                RenderBox(&box, tred);
              Selection_AfterRender();
            }
            else
            {
              Selection_BeforeRender(i);
              gScenario.GetTagCoord(m_ActiveSelectionMode, i, coord);
              RenderActiveSelectionBox(i, coord, FALSE, (SELECTION_MODE)sel_mode);
              Selection_AfterRender();
            }
          }
        }
      }
      else
      {
        if((SELECTION_MODE)sel_mode == AI_SPAWNS)
        {
          int encounter_cnt = gAI.GetEncounterCount();
          int squad_cnt;
          int spawn_cnt;
          int e;

          for(e=0; e<encounter_cnt; e++)
          {
            squad_cnt = gAI.GetSquadCount(e);
            for(int s=0; s<squad_cnt; s++)
            {
              spawn_cnt = gAI.GetSquadSpawnCount(e, s);
              gAI.ActivateSquad(e, s);
              for(int sp=0; sp<spawn_cnt; sp++)
              {
                gAI.GetSquadSpawnCoord(sp, coord);
                RenderInactiveEditBox(0, coord, (SELECTION_MODE)sel_mode);
              }
            }
          }    
        }
        else
        {
          count = gScenario.GetTagCount((SELECTION_MODE)sel_mode);
          
          for(i=0; i<count; i++)
          {
            //Render special cases
            if(((SELECTION_MODE)sel_mode == TRIGGERS)&&m_bDrawTriggers)
            {
              gScenario.GetTriggerVolume(i, &box);
              RenderBox(&box, tblue);
            }
            else //Render standard models, spawns, etc
            {
              gScenario.GetTagCoord((SELECTION_MODE)sel_mode, i, coord);
              
              RenderInactiveEditBox(i, coord, (SELECTION_MODE)sel_mode);
            }
          }
        }
      }
    }
  }
}

void CSelectionManager::PerformSelectionTest(double x, double y)
{
	Selection_InitSelectionTest(x, y);

  if(m_ActiveSelectionMode == BSP_MESH_INFO)
  {
    gRender.RenderVisibleBsp(TRUE);
  }
  else
  {
    RenderSelectionObjects();
  }

  Selection_ProcessPick();
}

UINT CSelectionManager::GetObjectIndex(GLint hits, GLuint *pSelBuf)
{
  unsigned int i;
  UINT closest;
  GLuint names, *ptr, minZ,*ptrNames, numberOfNames;
  
  ptr = (GLuint *) pSelBuf;
  minZ = 0xffffffff;
  for (i = 0; i < hits; i++)
  {	
    names = *ptr;
	   ptr++;
     if(*ptr < minZ)
     {
       numberOfNames = names;
       minZ = *ptr;
       ptrNames = ptr+2;
     }
     
     ptr += names+2;
  }
  
  //TRACE("The closest hit names are ");
  //ptr = ptrNames;
  //for (j = 0; j < numberOfNames; j++,ptr++)
  //{
  //  TRACE("%d ", *ptr);
  //}
  //TRACE("\n");

  if((hits > 0)&&(numberOfNames >= 2))
    closest = ptrNames[1];
  else
    closest = 0xffffffff;
  
  return(closest);
}

void CSelectionManager::SetSelectionMode(SELECTION_MODE sel_mode)
{
  int edit_count = gScenario.GetEditCount(sel_mode);
  m_ActiveSelectionMode = sel_mode;
  m_ReferenceSelection = -1;

  if(m_pObj)
  {
    delete m_pObj;
    m_pObj = NULL;
  }

  if(sel_mode == BSP_MESH_INFO)
  {
    m_ActiveCount = gBspManager.GetActiveBspSubmeshCount();
    edit_count = m_ActiveCount;
  }
  else
  {
    m_ActiveCount = gScenario.GetTagCount(sel_mode);
  }

  m_pObj = new SELECTION_OBJECT[edit_count];
  TRACE("Allocated %08X objects (selmode = %d)\n", edit_count,sel_mode);

  if(m_pObj)
  {
    ZeroMemory(m_pObj, sizeof(SELECTION_OBJECT)*edit_count);
  }
}

void CSelectionManager::SetEditCursorMode(float *pMoveVector)
{
  m_EditMode[0] = pMoveVector[0];
  m_EditMode[1] = pMoveVector[1];
  m_EditMode[2] = pMoveVector[2];
}

void CSelectionManager::MoveSelection(float x, float y, float z, float yaw, float pitch, float roll, BOOL bEnableAcc)
{
  float edit_spawn[6];

  // if user has held key down, accelerate the cursor (long distance movement)
  if(m_MoveAccTimer.GetDeltaTime() < 0.1)
  {
    m_AccCount++;
  }
  else
  {
    m_AccCount = 0;
    m_AccFactor = 0.01f;
  }

  if(m_AccCount > 5)
  {
    m_AccFactor += 0.05f;
  }

  if(m_AccFactor > m_AccMax)
    m_AccFactor = m_AccMax;

  m_MoveAccTimer.ResetTimer();

  if(bEnableAcc)
  {
    edit_spawn[0] = m_AccFactor*x;
    edit_spawn[1] = m_AccFactor*y;
    edit_spawn[2] = m_AccFactor*z;
    edit_spawn[3] = m_AccFactor*10*yaw;
    edit_spawn[4] = m_AccFactor*10*pitch;
    edit_spawn[5] = m_AccFactor*10*roll;
  }
  else
  {
    edit_spawn[0] = x;
    edit_spawn[1] = y;
    edit_spawn[2] = z;
    edit_spawn[3] = 10*yaw;
    edit_spawn[4] = 10*pitch;
    edit_spawn[5] = 10*roll;
  }

  for(int i=0; i<m_ActiveCount; i++)
  {
    if(i == m_ReferenceSelection)
      m_ActiveZPlane += edit_spawn[2];

    if(m_pObj[i].bSelected)
      gScenario.EditSelectedTag(m_ActiveSelectionMode, i, edit_spawn);
  }
}

void CSelectionManager::SetMaxTranslateSpeed(float scale)
{
  m_AccMax = scale;
  m_AccFactor = m_AccMax*0.1f;
}

void CSelectionManager::Selection_BeforeRender(int i)
{
  glPushName(i);
}

void CSelectionManager::Selection_AfterRender()
{
  glPopName();
}

void CSelectionManager::Selection_InitSelectionTest(double x, double y)
{
	GLint	viewport[4];

	glGetIntegerv(GL_VIEWPORT, viewport);
	glSelectBuffer(512, m_SelBuffer);

	glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(0);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	gluPickMatrix(x, (viewport[3]-y), 1.0f, 1.0f, viewport);

	// Apply The Perspective Matrix
	gluPerspective(45.0f, (GLfloat) (viewport[2]-viewport[0])/(GLfloat) (viewport[3]-viewport[1]), 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);									// Select The Modelview Matrix
}

void CSelectionManager::Selection_ProcessPick()
{
	GLint	hits;

  glMatrixMode(GL_PROJECTION);								// Select The Projection Matrix
	glPopMatrix();								              // Pop The Projection Matrix
	glMatrixMode(GL_MODELVIEW);									// Select The Modelview Matrix
	hits=glRenderMode(GL_RENDER);								// Switch To Render Mode, Find Out How Many

  UINT sel_obj = GetObjectIndex(hits, m_SelBuffer);

  if(m_pObj&&(m_CurrentEditorMode == EDITOR_MODE_SELECT))
  {
    for(int i=0; i<m_ActiveCount; i++)
    {
      if(m_ActiveSelectionMode != BSP_MESH_INFO)
      {
        if(sel_obj == i)
        {
          float data[6];
          gScenario.GetTagCoord(m_ActiveSelectionMode, i, data);
          m_ActiveZPlane = data[2];
          m_ReferenceSelection = i;
        }
      }

      //If we are in multi-select mode, process selections differently
      if(GetAsyncKeyState(VK_CONTROL) & 0x8000)
      {
        if(sel_obj == i)
        {
          if(m_pObj[i].bSelected == FALSE)
          {
            m_pObj[i].bSelected = TRUE;
            gScenario.UpdateTools(m_ActiveSelectionMode, i);
          }
          else
          {
            m_pObj[i].bSelected = FALSE;
          }
        }
      }
      else
      {
        if(sel_obj == i)
        {
          m_pObj[i].bSelected = TRUE;
          gScenario.UpdateTools(m_ActiveSelectionMode, i);
        }
        else
        {
          m_pObj[i].bSelected = FALSE;
        }
      }
    }
  }
}

void CSelectionManager::Initialize(UINT version)
{
  m_Version = version;
}

BOOL CSelectionManager::IsHighDetailEnabled(SELECTION_MODE sel_mode, float cam_dist)
{
  BOOL bUseHighDetail = FALSE;

  switch(sel_mode)
  {
  case VEHICLES:
    if(cam_dist < gProfile.m_BinProfile.VehicleSel.HighDetailDrawDist)
      bUseHighDetail = TRUE;
    break;

  case SCENERY:
    if(cam_dist < gProfile.m_BinProfile.ScenerySel.HighDetailDrawDist)
      bUseHighDetail = TRUE;
    break;

  case WEAPONS:
    if(cam_dist < gProfile.m_BinProfile.WeaponSel.HighDetailDrawDist)
      bUseHighDetail = TRUE;
    break;

  case MP_EQUIPMENT:
  case EQUIPMENT:
    if(cam_dist < gProfile.m_BinProfile.EquipSel.HighDetailDrawDist)
      bUseHighDetail = TRUE;
    break;

  case MACHINES:
    if(cam_dist < gProfile.m_BinProfile.MachinesSel.HighDetailDrawDist)
      bUseHighDetail = TRUE;
    break;

  case BIPEDS:
    if(cam_dist < gProfile.m_BinProfile.BipedsSel.HighDetailDrawDist)
      bUseHighDetail = TRUE;
    break;
  }

  return(bUseHighDetail);
}

void CSelectionManager::Cleanup()
{
  if(m_pObj)
    delete [] m_pObj;
  m_pObj = NULL;

  m_ActiveCount = 0;
  m_ReferenceSelection = -1;
}

void CSelectionManager::RenderBox(BOUNDING_BOX *pBox, float *pClr)
{
  float x1,y1,z1,x2,y2,z2;
  float clr[4] = {0,0,0.8f, 0.5f};

  glColor3f(0,1,0);
	glPointSize(5);
  glEnable(GL_BLEND);
  glBegin(GL_QUADS);

  x1 = pBox->min[0];
  y1 = pBox->min[1];
  z1 = pBox->min[2];
  x2 = pBox->min[0] + pBox->max[0];
  y2 = pBox->min[1] + pBox->max[1];
  z2 = pBox->min[2] + pBox->max[2];


  glColor4f(pClr[0],pClr[1],pClr[2],pClr[3]);
  //face 1
  glVertex3f(x1, y1, z1);
  glVertex3f(x1, y2, z1);
  glVertex3f(x1, y2, z2);
  glVertex3f(x1, y1, z2);
  
  //face 2
  glVertex3f(x2, y1, z1);
  glVertex3f(x2, y2, z1);
  glVertex3f(x2, y2, z2);
  glVertex3f(x2, y1, z2);
  
  glColor4f(pClr[0]*0.8f,pClr[1]*0.8f,pClr[2]*0.8f,pClr[3]);
  //face 3
  glVertex3f(x1, y1, z1);
  glVertex3f(x2, y1, z1);
  glVertex3f(x2, y1, z2);
  glVertex3f(x1, y1, z2);
  
  //face 4
  glVertex3f(x1, y2, z1);
  glVertex3f(x2, y2, z1);
  glVertex3f(x2, y2, z2);
  glVertex3f(x1, y2, z2);
  
  glColor4f(pClr[0]*0.6f,pClr[1]*0.6f,pClr[2]*0.6f,pClr[3]);
  //face 5
  glVertex3f(x1, y1, z1);
  glVertex3f(x2, y1, z1);
  glVertex3f(x2, y2, z1);
  glVertex3f(x1, y2, z1);
  
  //face 6
  glVertex3f(x1, y1, z2);
  glVertex3f(x2, y1, z2);
  glVertex3f(x2, y2, z2);
  glVertex3f(x1, y2, z2);
  
  glEnd();
  glDisable(GL_BLEND);
}

void CSelectionManager::EnableTriggerDraw(BOOL bEnable)
{
  m_bDrawTriggers = bEnable;
}

void CSelectionManager::GetReferenceCoordinate(float *pCoord)
{
  gScenario.GetTagCoord(m_ActiveSelectionMode, m_ReferenceSelection, pCoord);
}

EDITOR_MODE CSelectionManager::GetCurrentEditorMode()
{
  return(m_CurrentEditorMode);
}

void CSelectionManager::SetCurrentEditorMode(EDITOR_MODE mode)
{
  m_CurrentEditorMode = mode;
}

void CSelectionManager::SetActiveSelectionRotation(float yaw, float pitch, float roll, int mode)
{
  float temp[6];
  ZeroMemory(temp, 24);
  for(int i=0; i<m_ActiveCount; i++)
  {
    if(m_pObj[i].bSelected)
    {
      gScenario.GetTagCoord(m_ActiveSelectionMode, i, temp);
      temp[0] = 0;
      temp[1] = 0;
      temp[2] = 0;

      switch(mode)
      {
      case 0:
        temp[3] *= -1;
        temp[4] *= -1;
        temp[5] *= -1;
        break;

      case 1:
        temp[3] *= -1;
        temp[3] += yaw;
        temp[4] = 0;
        temp[5] = 0;
        break;
      
      case 2:
        temp[3] = 0;
        temp[4] *= -1;
        temp[4] += pitch;
        temp[5] = 0;
        break;
      
      case 3:
        temp[3] = 0;
        temp[4] = 0;
        temp[5] *= -1;
        temp[5] += roll;
        break;
      }

      gScenario.EditSelectedTag(m_ActiveSelectionMode, i, temp);
    }
  }
}

BOOL CSelectionManager::DuplicateActiveSelection()
{
  BOOL bStatus = FALSE;

  if(m_ReferenceSelection != -1)
  {
    switch(m_ActiveSelectionMode)
    {
    case SCENERY:
      gScenario.DuplicateScenery(m_ReferenceSelection);
      bStatus = TRUE;
      break;

    case VEHICLES:
      gScenario.DuplicateVehicles(m_ReferenceSelection);
      bStatus = TRUE;
      break;
 
    case MP_EQUIPMENT:
      gScenario.DuplicateMpEquip(m_ReferenceSelection);
      bStatus = TRUE;
      break;

    case MACHINES:
      gScenario.DuplicateMachine(m_ReferenceSelection);
      bStatus = TRUE;
      break;
    }
  }
  m_ActiveCount = gScenario.GetTagCount(m_ActiveSelectionMode);

  //clear the selection list
  if(bStatus == TRUE)
  {
    for(int i=0; i<m_ActiveCount; i++)
    {
      m_pObj[i].bSelected = FALSE;
    }
    
    //activate the last item
    m_pObj[m_ActiveCount - 1].bSelected = TRUE;
    m_ReferenceSelection = m_ActiveCount - 1;
  }

  return(bStatus);
}

SELECTION_MODE CSelectionManager::GetActiveSelectionMode()
{
  return(m_ActiveSelectionMode);
}

BOOL CSelectionManager::IsObjectSelected()
{
  BOOL bSelected = TRUE;
  
  if(m_ReferenceSelection == -1)
    bSelected = FALSE;

  return(bSelected);
}

BOOL CSelectionManager::DeleteActiveSelection()
{
  BOOL bStatus = FALSE;

  if(m_ReferenceSelection != -1)
  {
    switch(m_ActiveSelectionMode)
    {
    case SCENERY:
      gScenario.DeleteScenery(m_ReferenceSelection);
      bStatus = TRUE;
      break;

    case VEHICLES:
      gScenario.DeleteVehicle(m_ReferenceSelection);
      bStatus = TRUE;
      break;
 
    case MP_EQUIPMENT:
      gScenario.DeleteMpEquip(m_ReferenceSelection);
      bStatus = TRUE;
      break;
    }
  }
  m_ActiveCount = gScenario.GetTagCount(m_ActiveSelectionMode);

  //clear the selection list
  if(bStatus == TRUE)
  {
    for(int i=0; i<m_ActiveCount; i++)
    {
      m_pObj[i].bSelected = FALSE;
    }
    
    m_ReferenceSelection = -1;
  }

  return(bStatus);
}

void CSelectionManager::ToggleEditorMode()
{
  switch(m_CurrentEditorMode)
  {
  case EDITOR_MODE_SELECT:
    m_CurrentEditorMode = EDITOR_MODE_TRANSLATE;
    break;

  case EDITOR_MODE_TRANSLATE:
    m_CurrentEditorMode = EDITOR_MODE_ROTATE;
    break;

  default:
    m_CurrentEditorMode = EDITOR_MODE_SELECT;
    break;
  }
}
