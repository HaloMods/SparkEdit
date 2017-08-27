#include "stdafx.h"
#include "SparkEdit.h"
#include "Perspective.h"
#include "HaloMapFile.h"
#include "SelectionManager.h"
#include "ToolView.h"
#include "Render.h"
#include "BspManager.h"
#include "Frustum1.h"
#include "ProfileSettings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CPerspective *g_pRenderWnd;
extern CHaloMapFile HaloMap;
extern CSelectionManager gSelection;
extern CToolView *g_pTools;
extern CRender gRender;
extern CBspManager gBspManager;
extern CFrustum gFrustum;
extern CProfileSettings gProfile;

#define FRAME_UPDATE_TIMER  1

IMPLEMENT_DYNCREATE(CPerspective, COpenGLWnd)

CPerspective::CPerspective()
{
	m_xpos = 0.0f;
	m_ypos = 0.0f;
	m_zoom = 100.0f;
	m_xrot = 0.0f;
	m_yrot = 90.0f;
  m_fCameraSpeed = 1.5f;

  g_pRenderWnd = this;

	m_lastMouseX = -1;
	m_lastMouseY = -1;
  m_Camera.PositionCamera(75, 75, 0,   0, 0, 0,   0, 0, 1);

  m_CameraAccCount = 0;
	m_fCameraAcc = 0.3f;
  m_CameraAccMax = 1.0f;
  m_CameraAccTimer.ResetTimer();
  m_bInvertMouse = FALSE;
  m_bLookMode = FALSE;
  m_bEnableRotationMode = FALSE;
  m_LastSelectionPos[0] = 0;
  m_LastSelectionPos[1] = 0;
  m_LastSelectionPos[2] = 0;
}

CPerspective::~CPerspective()
{
}

void CPerspective::OnDraw(CDC* pDC)
{
  gRender.TimerEnd();
  gRender.TimerStart();
  glLoadIdentity();
  CheckKeyboard();
  m_Camera.Update();
  m_Camera.Look();
  gFrustum.CalculateFrustum();

	SetContext();
	RenderScene();
	SwapGLBuffers();
}

BEGIN_MESSAGE_MAP(CPerspective, COpenGLWnd)
	//{{AFX_MSG_MAP(CPerspective)
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_CHAR()
	ON_WM_TIMER()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#ifdef _DEBUG
void CPerspective::AssertValid() const
{
	CView::AssertValid();
}

void CPerspective::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

void CPerspective::OnSize(UINT nType, int cx, int cy) 
{
  float CullDist;
	COpenGLWnd::OnSize(nType, cx, cy);

	if ( 0 >= cx || 0 >= cy || nType == SIZE_MINIMIZED )
		return;

  CullDist = gProfile.GetCullDistance();
  SetRenderWindowOptions(CullDist, m_bInvertMouse);
}

void CPerspective::OnMouseMove(UINT nFlags, CPoint point) 
{
  int DeltaMouseX = point.x - m_lastMouseX;
  int DeltaMouseY = point.y - m_lastMouseY;

  m_lastMouseX = point.x;
  m_lastMouseY = point.y;

  if(nFlags & MK_RBUTTON)
  {
    if(m_bInvertMouse)
      m_Camera.UpdateMouseMove(DeltaMouseX, -DeltaMouseY);
    else
      m_Camera.UpdateMouseMove(DeltaMouseX, DeltaMouseY);
  }

  //Check for mouse translation editing
  if(gSelection.IsObjectSelected())
  {
    EDITOR_MODE mode;
    mode = gSelection.GetCurrentEditorMode();
    
    if(mode == EDITOR_MODE_ROTATE)
    {
      PerformRotationEdit(nFlags, DeltaMouseX, DeltaMouseY);
    }
    else if(mode == EDITOR_MODE_TRANSLATE)
    {
      PerformTranslationEdit(nFlags, point);
    }
  }

  //If we are in look mode, reset mouse cursor position to center of client
  if((m_bLookMode)&&((abs(DeltaMouseX) > 1)||(abs(DeltaMouseY) > 1)))
  {
    RECT rc;
    POINT pt;

    GetWindowRect(&rc);
    pt.x = (rc.right-rc.left)/2;
    pt.y = (rc.bottom-rc.top)/2;

    m_lastMouseX = pt.x;
    m_lastMouseY = pt.y;

    ClientToScreen(&pt);
    SetCursorPos(pt.x, pt.y);

    if(m_bInvertMouse)
      m_Camera.UpdateMouseMove(DeltaMouseX, -DeltaMouseY);
    else
      m_Camera.UpdateMouseMove(DeltaMouseX, DeltaMouseY);
  }

	COpenGLWnd::OnMouseMove(nFlags, point);
}

void CPerspective::SetCameraSpeed(float scale)
{
  m_fCameraSpeed = scale;
}

void CPerspective::RefreshWindow()
{
  float x, y, z;

  gBspManager.GetActiveBspCentroid(&x, &y, &z);
  TRACE("map center = %f %f %f\n", x, y, z);
  m_Camera.PositionCamera(x+20, y+20, z+20, 
                          x,y,z, 
                          0, 0, 1);
  OnDraw(NULL);
}

void CPerspective::CheckKeyboard()
{
  BOOL bUpdateCam = FALSE;
  BOOL bMovedCam = FALSE;

  float move[3] = {0,0,0};
  float pitch = 0;
  float roll = 0;
  float yaw = 0;

  CWnd *pCurrentWnd = GetFocus();

  // if user has held key down, accelerate the camera (long distance movement)
  float rendertime = gRender.GetFrameTime();
//  TRACE("render_time = %f\n", rendertime);
  if((m_CameraAccTimer.GetDeltaTime() - rendertime) < 0.1)
  {
    m_CameraAccCount++;
  }
  else
  {
    m_CameraAccCount = 0;
    m_fCameraAcc = 0.3f;
  }

  if(m_CameraAccCount > 5)
  {
    m_fCameraAcc += 0.1f*m_fCameraSpeed;
  }

  if(m_fCameraAcc > m_CameraAccMax)
    m_fCameraAcc = m_CameraAccMax;


  //check keyboard
  if(pCurrentWnd == this)
  {
    if(GetAsyncKeyState(VK_ESCAPE) & 0x8000)
      m_bLookMode = FALSE;

    if(GetAsyncKeyState('W') & 0x8000)
    {
      m_Camera.MoveCamera(m_fCameraSpeed*m_fCameraAcc);				
      bMovedCam = TRUE;
    }
    
    if(GetAsyncKeyState('S') & 0x8000)
    {
      m_Camera.MoveCamera(-m_fCameraSpeed*m_fCameraAcc);				
      bMovedCam = TRUE;
    }
    
    if(GetAsyncKeyState('D') & 0x8000)
    {
      m_Camera.StrafeCamera(m_fCameraSpeed*m_fCameraAcc);
      bMovedCam = TRUE;
    }
    
    if(GetAsyncKeyState('A') & 0x8000)
    {
      m_Camera.StrafeCamera(-m_fCameraSpeed*m_fCameraAcc);
      bMovedCam = TRUE;
    }
    
    if(GetAsyncKeyState(' ') & 0x8000)
    {
      m_Camera.LevitateCamera(m_fCameraSpeed*m_fCameraAcc);
      bMovedCam = TRUE;
    }
    
    if(GetAsyncKeyState(VK_SHIFT) & 0x8000)
    {
      m_Camera.LevitateCamera(-m_fCameraSpeed*m_fCameraAcc);
      bMovedCam = TRUE;
    }

    if(GetAsyncKeyState(VK_LEFT) & 0x8000)
      move[0] = 1.0f;

    if(GetAsyncKeyState(VK_RIGHT) & 0x8000)
      move[0] = -1.0;

    if(GetAsyncKeyState(VK_UP) & 0x8000)
      move[1] = 1.0f;

    if(GetAsyncKeyState(VK_DOWN) & 0x8000)
      move[1] = -1.0f;

    if(GetAsyncKeyState(VK_PRIOR) & 0x8000)
      move[2] = 1.0f;

    if(GetAsyncKeyState(VK_NEXT) & 0x8000)
      move[2] = -1.0f;

    if((move[0]!=0)||(move[1]!=0)||(move[2]!=0)||(pitch!=0)||(roll!=0)||(yaw!=0))
      gSelection.MoveSelection(move[0], move[1], move[2], 0, 0, 0, TRUE);

    gSelection.SetEditCursorMode(move);

    if(bMovedCam)
      m_CameraAccTimer.ResetTimer();

  }
}

void CPerspective::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
  //call this when a key is pressed to update the camera
	//OnDraw(NULL);
  if(nChar == 9)
  {
    gSelection.ToggleEditorMode();
  }
	
	COpenGLWnd::OnChar(nChar, nRepCnt, nFlags);
}

void CPerspective::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == FRAME_UPDATE_TIMER)
  {
    OnDraw(NULL);
  }

	COpenGLWnd::OnTimer(nIDEvent);
}

void CPerspective::OnInitialUpdate()
{
  SetTimer(FRAME_UPDATE_TIMER, 50, 0);
}


void CPerspective::OnLButtonUp(UINT nFlags, CPoint point) 
{
  gSelection.PerformSelectionTest(point.x, point.y);

	COpenGLWnd::OnLButtonUp(nFlags, point);
}

void CPerspective::GetCameraPosition(float *pPosition)
{
  m_Camera.GetCameraPosition(pPosition);
}

void CPerspective::SetRenderWindowOptions(float cull_dist, BOOL bInvertMouse)
{
  RECT rc;

  m_bInvertMouse = bInvertMouse;

  GetClientRect(&rc);

  float x = rc.right - rc.left;
  float y = rc.bottom - rc.top;

  SetContext();
	glViewport( 0, 0, x, y);
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

  gluPerspective(45.0f, x/y, 0.1f, cull_dist);
	glMatrixMode( GL_MODELVIEW );
}

void CPerspective::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
  m_bLookMode ^= 1;
  
	COpenGLWnd::OnRButtonDblClk(nFlags, point);
}

void CPerspective::SetCamForModel(BOOL bModelMode, float dim)
{
  if(bModelMode)
  {
    m_Camera.PositionCamera(1.5f*dim, 1.5f*dim, 1.5f*dim,   0, 0, 0,   0, 0, 1);
  }
  else  //use stored position
  {
    m_Camera.RestoreSavedPosition();
  }
}

float CPerspective::GetCameraSpeed(void)
{
  float temp;
  temp = m_fCameraSpeed;

  return(temp);
}

void CPerspective::EnableRotationEdit(BOOL bEnable)
{
  m_bEnableRotationMode = bEnable;
}

void CPerspective::PerformTranslationEdit(UINT nFlags, CPoint point)
{
  //calculate mouse-movement of selected object
  float move_x = 0;
  float move_y = 0;
  float move_z = 0;
  float ox,oy,oz;
  float cam_pos[3];
  float sel_pos[3];
  float obj_pos[6];

  ox = 0;
  oy = 0;
  oz = 0;
  ZeroMemory(cam_pos, 12);
  ZeroMemory(sel_pos, 12);
  ZeroMemory(obj_pos, 12);

  //use last selected object's reference planes to figure out where to move the object
  gSelection.GetReferenceCoordinate(obj_pos);

  m_Camera.DoUnProject(point.x, point.y, &ox, &oy, &oz, 0.5, FALSE);
  m_Camera.GetCameraPosition(cam_pos);

  if(GetAsyncKeyState(VK_CONTROL) & 0x8000) //Up-Down Object Movement
  {
    //do calculation to determine selection plane intersect point for XZ plane
    if((cam_pos[1] - oy) != 0)
    {
      float u = (cam_pos[1] - obj_pos[1])/(cam_pos[1] - oy);
      
      sel_pos[0] = cam_pos[0] + u*(ox - cam_pos[0]);
      sel_pos[1] = cam_pos[1] + u*(oy - cam_pos[1]);
      sel_pos[2] = cam_pos[2] + u*(oz - cam_pos[2]);

      move_x = 0;
      move_y = 0;
      move_z = sel_pos[2] - m_LastSelectionPos[2];
    }
  }
  else //X-Y Object Movement
  {
    //do calculation to determine selection plane intersect point for XY plane
    if((cam_pos[2] - oz) != 0)
    {
      float u = (cam_pos[2] - obj_pos[2])/(cam_pos[2] - oz);
      
      sel_pos[0] = cam_pos[0] + u*(ox - cam_pos[0]);
      sel_pos[1] = cam_pos[1] + u*(oy - cam_pos[1]);
      sel_pos[2] = cam_pos[2] + u*(oz - cam_pos[2]);

      move_x = sel_pos[0] - m_LastSelectionPos[0];
      move_y = sel_pos[1] - m_LastSelectionPos[1];
      move_z = 0;
    }
  }

  m_LastSelectionPos[0] = sel_pos[0];
  m_LastSelectionPos[1] = sel_pos[1];
  m_LastSelectionPos[2] = sel_pos[2];
   
  //TRACE("z-plane = %f\n", zplane);
  //TRACE("%.3f  %.3f %0.3f\n",move_x,move_y, move_z);
  //TRACE("%.3f  %.3f %0.3f   %.3f  %.3f %0.3f\n",sel_pos[0], sel_pos[1], sel_pos[2],move_x,move_y, move_z);
  if(nFlags & MK_LBUTTON)
  {
    gSelection.MoveSelection(move_x, move_y, move_z,0,0,0, FALSE);
  }
}

void CPerspective::PerformRotationEdit(UINT nFlags, int dx, int dy)
{
  float roll = 0;
  float pitch = 0;
  float yaw = 0;
  float adx = abs(dx);
  float ady = abs(dy);

  if(GetAsyncKeyState(VK_CONTROL) & 0x8000)
  {
    if(dx > 5)dx = 5;
    if(dx < -5)dx = -5;
    pitch = dx*DEG_TO_RAD*0.05;

    if(dy > 5)dy = 5;
    if(dy < -5)dy = -5;
    roll = dy*DEG_TO_RAD*0.05;
  }
  else
  {
    if(dx > 5)dx = 5;
    if(dx < -5)dx = -5;

    yaw = dx*DEG_TO_RAD*0.05;
  }

  if(nFlags & MK_LBUTTON)
  {
    gSelection.MoveSelection(0, 0, 0, yaw, pitch, roll, FALSE);
  }
}

BOOL CPerspective::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
  gSelection.ToggleEditorMode();
	
	return COpenGLWnd::OnMouseWheel(nFlags, zDelta, pt);
}
