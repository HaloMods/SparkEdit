// OutputPane.cpp : implementation file
//

#include "stdafx.h"
#include "SparkEdit.h"
#include "OutputPane.h"

extern COutputPane *g_pOutput;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputPane

IMPLEMENT_DYNCREATE(COutputPane, CFormView)

COutputPane::COutputPane()
	: CFormView(COutputPane::IDD)
{
	//{{AFX_DATA_INIT(COutputPane)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

COutputPane::~COutputPane()
{
}

void COutputPane::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COutputPane)
	DDX_Control(pDX, IDC_OUTPUT_WIN, m_OutputWin);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COutputPane, CFormView)
	//{{AFX_MSG_MAP(COutputPane)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COutputPane diagnostics

#ifdef _DEBUG
void COutputPane::AssertValid() const
{
	CFormView::AssertValid();
}

void COutputPane::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// COutputPane message handlers

void COutputPane::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);

	CRect cr;

  GetClientRect(&cr);
  if(m_OutputWin.m_hWnd != NULL)
    m_OutputWin.MoveWindow(cr.left, cr.top, cr.Width(), cr.Height());
}

void COutputPane::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	
	g_pOutput = this;

  PostText("SparkEdit Version 3.02\n", LOG_BLUE);
  PostText("Created by Grenadiac\n", LOG_BLUE);
  PostText("Visit www.halomods.com for more information.\n", LOG_BLUE);
}

void COutputPane::PostText(CString msg, COLORREF color)
{
  int iTotalTextLength;
  CHARFORMAT cf;
  PARAFORMAT pf;

  /* Grab the first character of the last line */ 
  iTotalTextLength = m_OutputWin.GetWindowTextLength();

  /* Gather the formatting information */ 
  cf.cbSize = sizeof(CHARFORMAT);
  cf.dwMask  = CFM_COLOR | CFM_UNDERLINE | CFM_BOLD;
  cf.dwEffects = (unsigned long)~(CFE_AUTOCOLOR | CFE_UNDERLINE | CFE_BOLD);
  cf.crTextColor = color;  

  /* Add the actual text */ 
  m_OutputWin.SetSel(iTotalTextLength, iTotalTextLength);  // Set carat to last line
  m_OutputWin.SetSelectionCharFormat(cf);     // Apply formatting prior to text
  m_OutputWin.HideSelection(TRUE, FALSE);     // Show the current selection
  m_OutputWin.ReplaceSel((LPCTSTR)msg); // Add the text

  /* Force the paragraph to be left-justified so the horizontal scroll won't go
     off to the right. */ 
  pf.cbSize = sizeof(PARAFORMAT);
  pf.dwMask = PFM_ALIGNMENT;
  pf.wAlignment = PFA_LEFT;
  m_OutputWin.SetParaFormat(pf);

  /* Vertical scroll one line, horizontal scroll 0 characters */ 
  m_OutputWin.LineScroll(1, 0);
}
