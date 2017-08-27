// ImportScenarioTagDlg.cpp : implementation file
//

#include "stdafx.h"
#include "sparkedit.h"
#include "ImportScenarioTagDlg.h"
#include "HaloStructDefs.h"
#include "TagManager.h"
#include "Scenario.h"
#include "ToolView.h"

extern CTagManager gTagManager;
extern CScenario gScenario;
extern CToolView *g_pTools;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImportScenarioTagDlg dialog


CImportScenarioTagDlg::CImportScenarioTagDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CImportScenarioTagDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CImportScenarioTagDlg)
	m_AvailableTagIndex = 0;
	m_PalletteType = 0;
	//}}AFX_DATA_INIT
  //ZeroMemory(m_UnregSceneryPallette, sizeof(m_UnregSceneryPallette));
  //ZeroMemory(m_UnregVehiclePallette, sizeof(m_UnregVehiclePallette));
  m_VehicleItemCount = 0;
  m_SceneryItemCount = 0;
}


void CImportScenarioTagDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImportScenarioTagDlg)
	DDX_CBIndex(pDX, IDC_AVAILABLE_TAGS, m_AvailableTagIndex);
	DDX_CBIndex(pDX, IDC_PALLETTE_TYPE, m_PalletteType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CImportScenarioTagDlg, CDialog)
	//{{AFX_MSG_MAP(CImportScenarioTagDlg)
	ON_CBN_SELCHANGE(IDC_PALLETTE_TYPE, OnSelchangePalletteType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImportScenarioTagDlg message handlers

void CImportScenarioTagDlg::SearchForMatchingTags()
{
  int i;
  int item_coll_count;
  int scenery_count;
  int vehicle_count;
  CString str;
  UINT tag_id, raw_name_offset;

  //call on tag manager to build a list of vehi,scenery, mp_equip tags
  item_coll_count = gTagManager.GetHighLevelTagCount(TAG_ITEM_COLLECTION);
  scenery_count = gTagManager.GetHighLevelTagCount(TAG_SCENERY);
  vehicle_count = gTagManager.GetHighLevelTagCount(TAG_VEHICLE);

  for(i=0; i<vehicle_count; i++)
  {
    gTagManager.GetHighLevelTagInfo(TAG_VEHICLE, i, &tag_id, &raw_name_offset, &str);
    if(!gScenario.CheckPalletteForTagExistence(TAG_VEHICLE, tag_id))
    {
      m_UnregVehiclePallette[m_VehicleItemCount].GuiName = str;
      m_UnregVehiclePallette[m_VehicleItemCount].TagId = tag_id;
      m_UnregVehiclePallette[m_VehicleItemCount].RawName = raw_name_offset;
      m_VehicleItemCount++;
    }
  }

  for(i=0; i<scenery_count; i++)
  {
    gTagManager.GetHighLevelTagInfo(TAG_SCENERY, i, &tag_id, &raw_name_offset, &str);
    if(!gScenario.CheckPalletteForTagExistence(TAG_SCENERY, tag_id))
    {
      m_UnregSceneryPallette[m_SceneryItemCount].GuiName = str;
      m_UnregSceneryPallette[m_SceneryItemCount].TagId = tag_id;
      m_UnregSceneryPallette[m_SceneryItemCount].RawName = raw_name_offset;
      m_SceneryItemCount++;
    }
  }
}

BOOL CImportScenarioTagDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  SearchForMatchingTags();
  OnSelchangePalletteType();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CImportScenarioTagDlg::OnSelchangePalletteType() 
{
  UpdateData(TRUE);
  CComboBox *pCtrl = NULL;
  int i;

  pCtrl = (CComboBox*)GetDlgItem(IDC_AVAILABLE_TAGS);
  pCtrl->ResetContent();

  switch(m_PalletteType)
  {
  case 0:
    if(m_VehicleItemCount > 0)
      pCtrl->EnableWindow(TRUE);
    else
      pCtrl->EnableWindow(FALSE);

    for(i=0; i<m_VehicleItemCount; i++)
    {
      pCtrl->AddString(m_UnregVehiclePallette[i].GuiName);
    }
    pCtrl->SetCurSel(0);
    break;

  case 1:
    if(m_SceneryItemCount > 0)
      pCtrl->EnableWindow(TRUE);
    else
      pCtrl->EnableWindow(FALSE);

    for(i=0; i<m_SceneryItemCount; i++)
    {
      pCtrl->AddString(m_UnregSceneryPallette[i].GuiName);
    }
    pCtrl->SetCurSel(0);
    break;
  }
}

void CImportScenarioTagDlg::OnOK() 
{
  UpdateData(TRUE);

  switch(m_PalletteType)
  {
  case 0:
    if((m_VehicleItemCount > 0)&&(m_AvailableTagIndex < m_VehicleItemCount))
    {
      gScenario.AddVehicleType(m_UnregVehiclePallette[m_AvailableTagIndex].TagId,
                               m_UnregVehiclePallette[m_AvailableTagIndex].RawName);
    }
    break;

  case 1:
    if((m_SceneryItemCount > 0)&&(m_AvailableTagIndex < m_SceneryItemCount))
    {
      gScenario.AddSceneryType(m_UnregSceneryPallette[m_AvailableTagIndex].TagId,
                               m_UnregSceneryPallette[m_AvailableTagIndex].RawName);
    }
    break;
  }

  g_pTools->m_pToolSheet->m_EditTab.ResetTypePallette();

	CDialog::OnOK();
}
