// Tools1.cpp : implementation file
//

#include "stdafx.h"
#include "SparkEdit.h"
#include "Tools1.h"
#include "Perspective.h"
#include "Render.h"
#include "SelectionManager.h"
#include "HaloMapFile.h"
#include "BspManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CPerspective *g_pRenderWnd;
extern CRender gRender;
extern CSelectionManager gSelection;
extern CTools1 *g_pTools;
extern CHaloMapFile HaloMap;
extern CBspManager gBspManager;

/////////////////////////////////////////////////////////////////////////////
// CTools1

IMPLEMENT_DYNCREATE(CTools1, CFormView)

CTools1::CTools1()
	: CFormView(CTools1::IDD)
{
	//{{AFX_DATA_INIT(CTools1)
	m_CameraSpeed = 0;
	m_FindCoordX = 0.0f;
	m_FindCoordY = 0.0f;
	m_FindCoordZ = 0.0f;
	m_CameraLocation = _T("CameraLoc:");
	m_Info = _T("");
	m_TranslateSpeed = 0;
	m_SelectedBsp = -1;
	//}}AFX_DATA_INIT
}

CTools1::~CTools1()
{
}

void CTools1::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTools1)
	DDX_Control(pDX, IDC_TOOL_TAB, m_ToolsTab);
	DDX_Control(pDX, IDC_SEL_MP_EQUIP, m_SelMpEquipBtn);
	DDX_Control(pDX, IDC_SEL_MP_FLAGS, m_SelMpFlagsBtn);
	DDX_Control(pDX, IDC_SEL_WEAPONS, m_SelWeaponsBtn);
	DDX_Control(pDX, IDC_SEL_VEHICLES, m_SelVehiclesBtn);
	DDX_Control(pDX, IDC_SEL_SCENERY, m_SelSceneryBtn);
	DDX_Control(pDX, IDC_SEL_PLAYER_SPAWNS, m_SelPlayerSpawnsBtn);
	DDX_Control(pDX, IDC_RENDER_MODE, m_RenderModeCombo);
	DDX_Slider(pDX, IDC_TRAVEL_SPEED, m_CameraSpeed);
	DDX_Text(pDX, IDC_FIND_COORD_X, m_FindCoordX);
	DDX_Text(pDX, IDC_FIND_COORD_Y, m_FindCoordY);
	DDX_Text(pDX, IDC_FIND_COORD_Z, m_FindCoordZ);
	DDX_Text(pDX, IDC_CAMERA_LOCATION, m_CameraLocation);
	DDX_Text(pDX, IDC_INFO, m_Info);
	DDX_Slider(pDX, IDC_TRANSLATE_SPEED, m_TranslateSpeed);
	DDX_CBIndex(pDX, IDC_ACTIVE_BSP, m_SelectedBsp);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTools1, CFormView)
	//{{AFX_MSG_MAP(CTools1)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_TRAVEL_SPEED, OnReleasedcaptureTravelSpeed)
	ON_CBN_SELCHANGE(IDC_RENDER_MODE, OnSelchangeRenderMode)
	ON_BN_CLICKED(IDC_FIND_COORD, OnFindCoord)
	ON_BN_CLICKED(IDC_SEL_VEHICLES, OnSelVehicles)
	ON_BN_CLICKED(IDC_SEL_WEAPONS, OnSelWeapons)
	ON_BN_CLICKED(IDC_SEL_SCENERY, OnSelScenery)
	ON_BN_CLICKED(IDC_SEL_PLAYER_SPAWNS, OnSelPlayerSpawns)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_SEL_MP_FLAGS, OnSelMpFlags)
	ON_BN_CLICKED(IDC_SEL_MP_EQUIP, OnSelMpEquip)
	ON_BN_CLICKED(IDC_RESET_CAMERA, OnResetCamera)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_TRANSLATE_SPEED, OnReleasedcaptureTranslateSpeed)
	ON_CBN_SELCHANGE(IDC_ACTIVE_BSP, OnSelchangeActiveBsp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTools1 diagnostics

#ifdef _DEBUG
void CTools1::AssertValid() const
{
	CFormView::AssertValid();
}

void CTools1::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTools1 message handlers

void CTools1::OnInitialUpdate() 
{
  CSliderCtrl* pSlider;

	CFormView::OnInitialUpdate();
	
  g_pTools = this;

  m_RenderModeCombo.SetCurSel(4);

  m_ToolsTab.ShowWindow(FALSE);
  m_ToolsTab.InsertItem(0, "General");
  m_ToolsTab.InsertItem(1, "Multiplayer");

  //setup camera speed slider
  pSlider = (CSliderCtrl*)GetDlgItem(IDC_TRAVEL_SPEED);
  pSlider->SetRangeMin(0);
  pSlider->SetRangeMax(5);
  pSlider->SetTic(1);
  pSlider->SetTic(2);
  pSlider->SetTic(3);
  pSlider->SetTic(4);
//  pSlider->SetPos(4);
  pSlider->SetTic(5);
  m_CameraSpeed = 4;

  //setup camera speed slider
  m_TranslateSpeed = 2;

  Reset();

  UpdateData(FALSE);
  SetTimer(2, 50, 0);
}

void CTools1::OnReleasedcaptureTravelSpeed(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
  float scale = 1.0;

  UpdateData(TRUE);	

  switch(m_CameraSpeed)
  {
  case 0:
    scale = 0.1f;
    break;
  case 1:
    scale = 0.2f;
    break;
  case 2:
    scale = 0.4f;
    break;
  case 3:
    scale = 0.8f;
    break;
  case 4:
    scale = 1.5f;
    break;
  case 5:
    scale = 2.5f;
    break;
  default:
    scale = 1.5f;
    break;
  }

  TRACE("scale = %f\n", scale);
  g_pRenderWnd->SetCameraSpeed(scale);
}

void CTools1::OnSelchangeRenderMode() 
{
  int mode;

  mode = m_RenderModeCombo.GetCurSel();

  gRender.SetRenderMode((RENDER_MODE)mode);
  g_pRenderWnd->SetFocus();
  g_pRenderWnd->RedrawWindow();
}

void CTools1::OnFindCoord() 
{
  UpdateData(TRUE); 
  gRender.SetFindCoord(m_FindCoordX, m_FindCoordY, m_FindCoordZ);
}

void CTools1::OnSelVehicles() 
{
	m_SelWeaponsBtn.SetState(0);
	m_SelVehiclesBtn.SetState(1);
	m_SelSceneryBtn.SetState(0);
	m_SelPlayerSpawnsBtn.SetState(0);
	m_SelMpFlagsBtn.SetState(0);
  m_SelMpEquipBtn.SetState(0);
  gSelection.SetSelectionMode(VEHICLES);
}

void CTools1::OnSelWeapons() 
{
	m_SelWeaponsBtn.SetState(1);
	m_SelVehiclesBtn.SetState(0);
	m_SelSceneryBtn.SetState(0);
	m_SelPlayerSpawnsBtn.SetState(0);
	m_SelMpFlagsBtn.SetState(0);
  m_SelMpEquipBtn.SetState(0);
  gSelection.SetSelectionMode(WEAPONS);
}

void CTools1::OnSelScenery() 
{
	m_SelWeaponsBtn.SetState(0);
	m_SelVehiclesBtn.SetState(0);
	m_SelSceneryBtn.SetState(1);
	m_SelPlayerSpawnsBtn.SetState(0);
	m_SelMpFlagsBtn.SetState(0);
  m_SelMpEquipBtn.SetState(0);
  gSelection.SetSelectionMode(SCENERY);
}

void CTools1::OnSelPlayerSpawns() 
{
	m_SelWeaponsBtn.SetState(0);
	m_SelVehiclesBtn.SetState(0);
	m_SelSceneryBtn.SetState(0);
	m_SelPlayerSpawnsBtn.SetState(1);
	m_SelMpFlagsBtn.SetState(0);
  m_SelMpEquipBtn.SetState(0);
  gSelection.SetSelectionMode(PLAYER_SPAWNS);
}

void CTools1::OnSelMpFlags() 
{
	m_SelWeaponsBtn.SetState(0);
	m_SelVehiclesBtn.SetState(0);
	m_SelSceneryBtn.SetState(0);
	m_SelPlayerSpawnsBtn.SetState(0);
	m_SelMpFlagsBtn.SetState(1);
  m_SelMpEquipBtn.SetState(0);
  gSelection.SetSelectionMode(MP_FLAGS);
}

void CTools1::SetInfoText(CString str)
{
  m_Info = str;
  UpdateData(FALSE);
}

void CTools1::SetCameraPosInfo(float x, float y, float z)
{

}

void CTools1::OnTimer(UINT nIDEvent) 
{
  float Pos[3];
  CString str;

  if((nIDEvent == 2)&&(g_pRenderWnd))
  {
    g_pRenderWnd->GetCameraPosition(Pos);
    str.Format("CameraLoc: %.2f, %.2f, %.2f", Pos[0], Pos[1], Pos[2]);
    ((CWnd*)GetDlgItem(IDC_CAMERA_LOCATION))->SetWindowText(str);
  }

	CFormView::OnTimer(nIDEvent);
}

void CTools1::Reset()
{
  int i, bsp_count;
  CComboBox *pBspList;

  bsp_count = gBspManager.m_BspNames.GetSize();
  pBspList = (CComboBox*)GetDlgItem(IDC_ACTIVE_BSP);
  pBspList->ResetContent();

  for(i=0; i<bsp_count; i++)
  {
    pBspList->AddString(gBspManager.m_BspNames[i]);
  }
  m_SelectedBsp = 0;
	m_SelWeaponsBtn.SetState(0);
	m_SelVehiclesBtn.SetState(0);
	m_SelSceneryBtn.SetState(0);
	m_SelPlayerSpawnsBtn.SetState(0);
  m_SelMpFlagsBtn.SetState(0);
  m_SelMpEquipBtn.SetState(0);

  UpdateData(FALSE);
}

void CTools1::OnSelMpEquip() 
{
	m_SelWeaponsBtn.SetState(0);
	m_SelVehiclesBtn.SetState(0);
	m_SelSceneryBtn.SetState(0);
	m_SelPlayerSpawnsBtn.SetState(0);
  m_SelMpFlagsBtn.SetState(0);
  m_SelMpEquipBtn.SetState(1);
  gSelection.SetSelectionMode(MP_EQUIPMENT);
}

void CTools1::OnResetCamera() 
{
  g_pRenderWnd->RefreshWindow();
}

void CTools1::OnReleasedcaptureTranslateSpeed(NMHDR* pNMHDR, LRESULT* pResult) 
{
  float scale = 1.0;

  UpdateData(TRUE);	

  switch(m_TranslateSpeed)
  {
  case 0:
    scale = 0.05f;
    break;
  case 1:
    scale = 0.1f;
    break;
  case 2:
    scale = 0.5f;
    break;
  case 3:
    scale = 1.0f;
    break;
  default:
    scale = 0.5f;
    break;
  }

  gSelection.SetMaxTranslateSpeed(scale);
	
	*pResult = 0;
}

void CTools1::OnSelchangeActiveBsp() 
{
  UpdateData(TRUE);
  gBspManager.ActivateBsp(m_SelectedBsp);
}
