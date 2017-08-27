// OptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "sparkedit.h"
#include "OptionsDlg.h"
#include "ProfileSettings.h"
#include "Perspective.h"

extern CProfileSettings gProfile;
extern CPerspective *g_pRenderWnd;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionsDlg dialog


COptionsDlg::COptionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COptionsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptionsDlg)
	m_bInvertMouseLook = FALSE;
	m_BitmapPath = _T("");
	m_CEBitmapPath = _T("");
	m_DrawDistBipeds = 0.0f;
	m_DrawDistDevices = 0.0f;
	m_DrawDistScenery = 0.0f;
	m_DrawDistVehicles = 0.0f;
	m_DrawDistWeapons = 0.0f;
	m_CullDistance = 0.0f;
	m_DrawDistEquipment = 0.0f;
	//}}AFX_DATA_INIT
}


void COptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsDlg)
	DDX_Check(pDX, IDC_INVERT_MOUSE_LOOK, m_bInvertMouseLook);
	DDX_Text(pDX, IDC_BITMAPS_PATH, m_BitmapPath);
	DDX_Text(pDX, IDC_CEBITMAPS_PATH, m_CEBitmapPath);
	DDX_Text(pDX, IDC_DRAW_DIST_BIPEDS, m_DrawDistBipeds);
	DDX_Text(pDX, IDC_DRAW_DIST_DEVICES, m_DrawDistDevices);
	DDX_Text(pDX, IDC_DRAW_DIST_SCENERY, m_DrawDistScenery);
	DDX_Text(pDX, IDC_DRAW_DIST_VEHICLES, m_DrawDistVehicles);
	DDX_Text(pDX, IDC_DRAW_DIST_WEAPONS, m_DrawDistWeapons);
	DDX_Text(pDX, IDC_CULL_DISTANCE, m_CullDistance);
	DDV_MinMaxFloat(pDX, m_CullDistance, 1.f, 10000.f);
	DDX_Text(pDX, IDC_DRAW_DIST_EQUIPMENT, m_DrawDistEquipment);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsDlg, CDialog)
	//{{AFX_MSG_MAP(COptionsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL COptionsDlg::OnInitDialog() 
{
  PROFILE_SETTINGS temp;
	CDialog::OnInitDialog();
	
  //get profile settings from gProfile
  gProfile.GetProfile(&temp);

	m_BitmapPath = temp.BitmapPath;
	m_CEBitmapPath = temp.CEBitmapPath;
	m_DrawDistBipeds = temp.BipedsSel.HighDetailDrawDist;
	m_DrawDistDevices = temp.MachinesSel.HighDetailDrawDist;
	m_DrawDistScenery = temp.ScenerySel.HighDetailDrawDist;
	m_DrawDistVehicles = temp.VehicleSel.HighDetailDrawDist;
	m_DrawDistWeapons = temp.WeaponSel.HighDetailDrawDist;
	m_DrawDistEquipment = temp.EquipSel.HighDetailDrawDist;
  m_bInvertMouseLook = temp.bInvertMouseLook;
  m_CullDistance = temp.CullDistance;
	
  UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsDlg::OnOK() 
{
  PROFILE_SETTINGS temp;
  char *pPath;
  char *pCEPath;
  UpdateData(TRUE);

  pPath = m_BitmapPath.GetBuffer(255);
  pCEPath = m_CEBitmapPath.GetBuffer(255);
  strncpy(temp.BitmapPath, pPath, 255);
  strncpy(temp.CEBitmapPath, pCEPath, 255);
  m_BitmapPath.ReleaseBuffer();
  m_CEBitmapPath.ReleaseBuffer();

	temp.BipedsSel.HighDetailDrawDist = m_DrawDistBipeds;
	temp.MachinesSel.HighDetailDrawDist = m_DrawDistDevices;
	temp.ScenerySel.HighDetailDrawDist = m_DrawDistScenery;
	temp.VehicleSel.HighDetailDrawDist = m_DrawDistVehicles;
	temp.WeaponSel.HighDetailDrawDist = m_DrawDistWeapons;
  temp.EquipSel.HighDetailDrawDist = m_DrawDistEquipment;
  temp.bInvertMouseLook = m_bInvertMouseLook;
  temp.CullDistance = m_CullDistance;
	
  gProfile.SetProfile(&temp);
	
  // tell the render window to reset the cull distance
  g_pRenderWnd->SetRenderWindowOptions(m_CullDistance, m_bInvertMouseLook);

	CDialog::OnOK();
}
