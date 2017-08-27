// Tools1.cpp : implementation file
//

#include "stdafx.h"
#include "SparkEdit.h"
#include "ToolView.h"
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
extern CToolView *g_pTools;
extern CHaloMapFile HaloMap;
extern CBspManager gBspManager;

/////////////////////////////////////////////////////////////////////////////
// CToolView

IMPLEMENT_DYNCREATE(CToolView, CFormView)

CToolView::CToolView()
	: CFormView(CToolView::IDD)
{
	//{{AFX_DATA_INIT(CToolView)
	//}}AFX_DATA_INIT
  m_pToolSheet = NULL;
}

CToolView::~CToolView()
{
  if(m_pToolSheet)
    delete m_pToolSheet;
}

void CToolView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CToolView)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CToolView, CFormView)
	//{{AFX_MSG_MAP(CToolView)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToolView diagnostics

#ifdef _DEBUG
void CToolView::AssertValid() const
{
	CFormView::AssertValid();
}

void CToolView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CToolView message handlers

void CToolView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	
  g_pTools = this;

  SetTimer(2, 50, 0);
	
  CWnd *pWndPropSheetHolder = GetDlgItem(IDC_SHEET_HOLDER);
  m_pToolSheet = new CToolSheet("ToolSheet", pWndPropSheetHolder, 0);

  m_pToolSheet->AddPage(&(m_pToolSheet->m_TabNavigate));
  m_pToolSheet->AddPage(&(m_pToolSheet->m_EditTab));
  m_pToolSheet->AddPage(&(m_pToolSheet->m_ModelsTab));

  if(!m_pToolSheet->Create(pWndPropSheetHolder, WS_CHILD|WS_VISIBLE, 0))
  {
    delete m_pToolSheet;
    m_pToolSheet = NULL;
    return;
  }

  CRect rect;
  pWndPropSheetHolder->GetWindowRect(&rect);

  m_pToolSheet->SetWindowPos(NULL, 0, 0, rect.Width(), rect.Height(), SWP_NOZORDER|SWP_NOACTIVATE);

}


void CToolView::SetInfoText(CString str)
{
}

void CToolView::SetCameraPosInfo(float x, float y, float z)
{

}

void CToolView::Reset()
{
  m_pToolSheet->SetActivePage(0);
  m_pToolSheet->m_TabNavigate.Reset();
  m_pToolSheet->m_EditTab.Reset();
  m_pToolSheet->m_ModelsTab.InitializeModelList();
}


void CToolView::OnTimer(UINT nIDEvent) 
{
  float Pos[3];
  CString str;

  if((nIDEvent == 2)&&(g_pRenderWnd)&&(m_hWnd))
  {
    g_pRenderWnd->GetCameraPosition(Pos);
    str.Format("CameraLoc: %.2f, %.2f, %.2f", Pos[0], Pos[1], Pos[2]);
    ((CWnd*)GetDlgItem(IDC_CAMERA_LOCATION))->SetWindowText(str);

    str.Format("FrameRate: %.2f", gRender.GetFrameRate());
    ((CWnd*)GetDlgItem(IDC_FRAMERATE))->SetWindowText(str);
  }
		
	CFormView::OnTimer(nIDEvent);
}
