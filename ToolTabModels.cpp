// ToolTabModels.cpp : implementation file
//

#include "stdafx.h"
#include "sparkedit.h"
#include "ToolTabModels.h"
#include "ModelManager.h"
#include "Render.h"
#include "Perspective.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CModelManager gModelManager;
extern CRender gRender;
extern CPerspective *g_pRenderWnd;

/////////////////////////////////////////////////////////////////////////////
// CToolTabModels property page

IMPLEMENT_DYNCREATE(CToolTabModels, CPropertyPage)

CToolTabModels::CToolTabModels() : CPropertyPage(CToolTabModels::IDD)
{
	//{{AFX_DATA_INIT(CToolTabModels)
	m_ModelLOD = 4;
	//}}AFX_DATA_INIT
}

CToolTabModels::~CToolTabModels()
{
}

void CToolTabModels::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CToolTabModels)
	DDX_Control(pDX, IDC_MODEL_MARKERS, m_MarkerList);
	DDX_Control(pDX, IDC_MODEL_LIST, m_ModelList);
	DDX_CBIndex(pDX, IDC_MODEL_LOD, m_ModelLOD);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CToolTabModels, CPropertyPage)
	//{{AFX_MSG_MAP(CToolTabModels)
	ON_LBN_SELCHANGE(IDC_MODEL_LIST, OnSelchangeModelList)
	ON_CBN_SELCHANGE(IDC_MODEL_LOD, OnSelchangeModelLod)
	ON_CBN_SELCHANGE(IDC_MODEL_MARKERS, OnSelchangeModelMarkers)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToolTabModels message handlers

void CToolTabModels::InitializeModelList()
{
  UINT model_count;
  int i;
  CString name;
  
  if(m_hWnd)
  {
    model_count = gModelManager.GetModelCount();
    
    m_ModelList.ResetContent();
    for(i=0; i<model_count; i++)
    {
      name = gModelManager.GetModelName(i);
      m_ModelList.AddString(name);
    }
  }
}

BOOL CToolTabModels::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	InitializeModelList();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CToolTabModels::OnSelchangeModelList() 
{
  CPcModel *pPcModel = NULL;
  CXboxModel *pXboxModel = NULL;
  int i, index, marker_count;
  CString marker_name;

  index = m_ModelList.GetCurSel();
  gRender.SetActiveRenderModel(index);
  gRender.SetActiveMarker(-1);

  gModelManager.GetModel(index, &pPcModel, &pXboxModel);

  if(pPcModel)
  {
    marker_count = pPcModel->GetMarkerCount();

    m_MarkerList.ResetContent();
    for(i=0; i<marker_count; i++)
    {
      marker_name = pPcModel->GetMarkerName(i);
      m_MarkerList.AddString(marker_name);
    }
  }
  else if(pXboxModel)
  {
  }
}

BOOL CToolTabModels::OnSetActive() 
{	
  int index;
  index = m_ModelList.GetCurSel();
  g_pRenderWnd->m_Camera.SaveCurrentPosition();
  m_fOriginalCamSpeed = g_pRenderWnd->GetCameraSpeed();

  g_pRenderWnd->SetCameraSpeed(0.2f);
  gRender.SetActiveRenderModel(index);

  return CPropertyPage::OnSetActive();
}

BOOL CToolTabModels::OnKillActive() 
{
  gRender.SetActiveRenderModel(-1);
  g_pRenderWnd->SetCameraSpeed(m_fOriginalCamSpeed);
	
	return CPropertyPage::OnKillActive();
}

void CToolTabModels::OnSelchangeModelLod() 
{
  UpdateData(TRUE);
  gRender.SetModelLOD((MODEL_LOD)m_ModelLOD);
}

void CToolTabModels::OnSelchangeModelMarkers() 
{
  int index = m_MarkerList.GetCurSel();
  gRender.SetActiveMarker(index);
}
