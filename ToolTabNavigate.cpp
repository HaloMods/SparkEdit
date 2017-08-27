// ToolTabNavigate.cpp : implementation file
//

#include "stdafx.h"
#include "sparkedit.h"
#include "ToolTabNavigate.h"
#include "Perspective.h"
#include "Render.h"
#include "BspManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CPerspective *g_pRenderWnd;
extern CRender gRender;
extern CBspManager gBspManager;


IMPLEMENT_DYNCREATE(CToolTabNavigate, CPropertyPage)

CToolTabNavigate::CToolTabNavigate() : CPropertyPage(CToolTabNavigate::IDD)
{
	//{{AFX_DATA_INIT(CToolTabNavigate)
	m_CameraSpeed = 0;
	m_FindCoordX = 0.0f;
	m_FindCoordY = 0.0f;
	m_FindCoordZ = 0.0f;
	m_SelectedBsp = -1;
	//}}AFX_DATA_INIT
}

CToolTabNavigate::~CToolTabNavigate()
{
}

void CToolTabNavigate::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CToolTabNavigate)
	DDX_Control(pDX, IDC_RENDER_MODE, m_RenderModeCombo);
	DDX_Slider(pDX, IDC_TRAVEL_SPEED, m_CameraSpeed);
	DDX_Text(pDX, IDC_FIND_COORD_X, m_FindCoordX);
	DDX_Text(pDX, IDC_FIND_COORD_Y, m_FindCoordY);
	DDX_Text(pDX, IDC_FIND_COORD_Z, m_FindCoordZ);
	DDX_CBIndex(pDX, IDC_ACTIVE_BSP, m_SelectedBsp);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CToolTabNavigate, CPropertyPage)
	//{{AFX_MSG_MAP(CToolTabNavigate)
	ON_BN_CLICKED(IDC_FIND_COORD, OnFindCoord)
	ON_BN_CLICKED(IDC_RESET_CAMERA, OnResetCamera)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_TRAVEL_SPEED, OnReleasedcaptureTravelSpeed)
	ON_CBN_SELCHANGE(IDC_RENDER_MODE, OnSelchangeRenderMode)
	ON_CBN_SELCHANGE(IDC_ACTIVE_BSP, OnSelchangeActiveBsp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToolTabNavigate message handlers

void CToolTabNavigate::OnFindCoord() 
{
  UpdateData(TRUE); 
  gRender.SetFindCoord(m_FindCoordX, m_FindCoordY, m_FindCoordZ);
}

void CToolTabNavigate::OnResetCamera() 
{
  g_pRenderWnd->RefreshWindow();
}

void CToolTabNavigate::OnReleasedcaptureTravelSpeed(NMHDR* pNMHDR, LRESULT* pResult) 
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

BOOL CToolTabNavigate::OnInitDialog() 
{
  CSliderCtrl *pSlider;
  
  CPropertyPage::OnInitDialog();

  m_RenderModeCombo.SetCurSel(4);
	
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

  Reset();
  UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CToolTabNavigate::OnSelchangeRenderMode() 
{
  int mode;

  mode = m_RenderModeCombo.GetCurSel();

  gRender.SetRenderMode((RENDER_MODE)mode);
  g_pRenderWnd->SetFocus();
  g_pRenderWnd->RedrawWindow();
}


void CToolTabNavigate::OnSelchangeActiveBsp() 
{
  UpdateData(TRUE);
  gBspManager.ActivateBsp(m_SelectedBsp);
}

void CToolTabNavigate::Reset()
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

  UpdateData(FALSE);
}
