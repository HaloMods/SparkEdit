// ToolTabEdit.cpp : implementation file
//

#include "stdafx.h"
#include "sparkedit.h"
#include "ToolTabEdit.h"
#include "SelectionManager.h"
#include "ObjectInfoDlg.h"
#include "Scenario.h"

extern CSelectionManager gSelection;
extern CObjectInfoDlg gInfoDlg;
extern CScenario gScenario;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CToolTabEdit, CPropertyPage)

//-------------------------------------------------------------------
//  Name: CToolTabEdit()
//  Author(s): Grenadiac
//
//  Description:
//    
//-------------------------------------------------------------------
CToolTabEdit::CToolTabEdit() : CPropertyPage(CToolTabEdit::IDD)
{
	//{{AFX_DATA_INIT(CToolTabEdit)
	m_SelectionMode = -1;
	m_bViewTriggers = FALSE;
	//}}AFX_DATA_INIT
  m_ActiveSelMode = NONE;
  m_ActiveObjectIndex = -1;
  m_LastReference = -1;
  m_SnapYaw = 0.0f;
  m_SnapPitch = 0.0f;
  m_SnapRoll = 0.0f;
}

//-------------------------------------------------------------------
//  Name: ~CToolTabEdit()
//  Author(s): Grenadiac
//
//  Description:
//    
//-------------------------------------------------------------------
CToolTabEdit::~CToolTabEdit()
{
}

//-------------------------------------------------------------------
//  Name: DoDataExchange()
//  Author(s): Grenadiac
//
//  Description:
//    
//-------------------------------------------------------------------
void CToolTabEdit::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CToolTabEdit)
	DDX_Control(pDX, IDC_OBJECT_TYPE, m_ObjectType);
	DDX_CBIndex(pDX, IDC_SELECTION_MODE, m_SelectionMode);
	DDX_Check(pDX, IDC_VIEW_TRIGGERS, m_bViewTriggers);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CToolTabEdit, CPropertyPage)
	//{{AFX_MSG_MAP(CToolTabEdit)
	ON_CBN_SELCHANGE(IDC_SELECTION_MODE, OnSelchangeSelectionMode)
	ON_BN_CLICKED(IDC_VIEW_OBJ_INFO, OnViewObjInfo)
	ON_BN_CLICKED(IDC_VIEW_TRIGGERS, OnViewTriggers)
	ON_CBN_SELCHANGE(IDC_OBJECT_TYPE, OnSelchangeObjectType)
	ON_BN_CLICKED(IDC_RESET_ROTATION, OnResetRotation)
	ON_BN_CLICKED(IDC_SNAP_PITCH, OnSnapPitch)
	ON_BN_CLICKED(IDC_SNAP_ROLL, OnSnapRoll)
	ON_BN_CLICKED(IDC_SNAP_YAW, OnSnapYaw)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//-------------------------------------------------------------------
//  Name: OnInitDialog()
//  Author(s): Grenadiac
//
//  Description:
//    
//-------------------------------------------------------------------
BOOL CToolTabEdit::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

  SetTimer(202, 100, 0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//-------------------------------------------------------------------
//  Name: Reset()
//  Author(s): Grenadiac
//
//  Description:
//    
//-------------------------------------------------------------------
void CToolTabEdit::Reset()
{
  m_SelectionMode = 0;
  
  if(m_hWnd)
    UpdateData(FALSE);

  ResetTypePallette();
}

//-------------------------------------------------------------------
//  Name: OnSelchangeSelectionMode()
//  Author(s): Grenadiac
//
//  Description:
//    
//-------------------------------------------------------------------
void CToolTabEdit::OnSelchangeSelectionMode() 
{
  int pallette_count;
  CString str;

  UpdateData(TRUE);
  gSelection.SetSelectionMode((SELECTION_MODE)m_SelectionMode);

  m_ObjectType.ResetContent();

  pallette_count = gScenario.GetPalletteCount((SELECTION_MODE)m_SelectionMode);

  for(int i=0; i<m_TypePallette[m_SelectionMode].GetSize(); i++)
  {
    m_ObjectType.AddString(m_TypePallette[m_SelectionMode][i]);
  }
}

//-------------------------------------------------------------------
//  Name: OnViewObjInfo()
//  Author(s): Grenadiac
//
//  Description:
//    
//-------------------------------------------------------------------
void CToolTabEdit::OnViewObjInfo() 
{
  //set this flag so that only user resizes change the profile, not automatic
  gInfoDlg.m_bUserTouched = TRUE;
  gInfoDlg.ShowWindow(SW_SHOW);
  gInfoDlg.m_bShowWindow = TRUE;
}

//-------------------------------------------------------------------
//  Name: OnViewTriggers()
//  Author(s): Grenadiac
//
//  Description:
//    
//-------------------------------------------------------------------
void CToolTabEdit::OnViewTriggers() 
{
  UpdateData(TRUE);

  gSelection.EnableTriggerDraw(m_bViewTriggers);
}

//-------------------------------------------------------------------
//  Name: ResetTypePallette()
//  Author(s): Grenadiac
//
//  Description:
//    
//-------------------------------------------------------------------
void CToolTabEdit::ResetTypePallette()
{
  int pallette_count;
  int p;
  CString str;

  for(int i=0; i<SELECTION_MODE_MAX; i++)
  {
    m_TypePallette[i].RemoveAll();
    pallette_count = gScenario.GetPalletteCount((SELECTION_MODE)i);

    for(p=0; p<pallette_count; p++)
    {
      m_TypePallette[i].Add(gScenario.GetPalletteString((SELECTION_MODE)i, p));
    }
  }
}

//-------------------------------------------------------------------
//  Name: ActivateSelectionType()
//  Author(s): Grenadiac
//
//  Description:
//    
//-------------------------------------------------------------------
void CToolTabEdit::ActivateSelectionType(SELECTION_MODE sel_mode, int object_index, int type)
{
  m_ActiveSelMode = sel_mode;
  m_ActiveObjectIndex = object_index;
  m_ObjectType.SetCurSel(type);
}

//-------------------------------------------------------------------
//  Name: OnSelchangeObjectType()
//  Author(s): Grenadiac
//
//  Description:
//    
//-------------------------------------------------------------------
void CToolTabEdit::OnSelchangeObjectType() 
{
  int new_type = m_ObjectType.GetCurSel();
  gScenario.SetObjectType(m_ActiveSelMode, m_ActiveObjectIndex, new_type);
}

void CToolTabEdit::OnResetRotation() 
{
  gSelection.SetActiveSelectionRotation(0, 0, 0, 0);
  m_SnapYaw = 0.0f;
  m_SnapPitch = 0.0f;
  m_SnapRoll = 0.0f;
}

void CToolTabEdit::OnSnapPitch() 
{
  if(gSelection.m_ReferenceSelection != m_LastReference)
  {
    m_SnapPitch += RAD15;
    if(m_SnapPitch > (RAD360 - 0.01f))
    {
      m_SnapPitch = 0;
    }
  }
  else
  {
    m_SnapPitch = 0;
  }

  gSelection.SetActiveSelectionRotation(0, m_SnapPitch, 0, 2);
}

void CToolTabEdit::OnSnapRoll() 
{
  if(gSelection.m_ReferenceSelection != m_LastReference)
  {
    m_SnapRoll += RAD15;
    if(m_SnapRoll > (RAD360 - 0.01f))
    {
      m_SnapRoll = 0;
    }
  }
  else
  {
    m_SnapRoll = 0;
  }

  gSelection.SetActiveSelectionRotation(0, 0, m_SnapRoll, 3);
}

void CToolTabEdit::OnSnapYaw() 
{
  if(gSelection.m_ReferenceSelection != m_LastReference)
  {
    m_SnapYaw += RAD15;
    if(m_SnapYaw > (RAD360 - 0.01f))
    {
      m_SnapYaw = 0;
    }
  }
  else
  {
    m_SnapYaw = 0;
  }

  gSelection.SetActiveSelectionRotation(m_SnapYaw, 0, 0, 1);
}

void CToolTabEdit::OnTimer(UINT nIDEvent) 
{
  CString tmp;
	if(nIDEvent == 202)
  {
    float coord[6];
    ZeroMemory(coord, sizeof(coord));

    //update rotations
    if(gSelection.m_ReferenceSelection != -1)
      gSelection.GetReferenceCoordinate(coord);

    tmp.Format("%.1f", coord[3]*RAD_TO_DEG);
    ((CWnd*)GetDlgItem(IDC_CURRENT_YAW))->SetWindowText(tmp);
    tmp.Format("%.1f", coord[4]*RAD_TO_DEG);
    ((CWnd*)GetDlgItem(IDC_CURRENT_PITCH))->SetWindowText(tmp);
    tmp.Format("%.1f", coord[5]*RAD_TO_DEG);
    ((CWnd*)GetDlgItem(IDC_CURRENT_ROLL))->SetWindowText(tmp);
  }
	
	CPropertyPage::OnTimer(nIDEvent);
}
