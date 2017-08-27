// SparkEditView.cpp : implementation of the CSparkEditView class
//

#include "stdafx.h"
#include "SparkEdit.h"

#include "SparkEditDoc.h"
#include "SparkEditView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CSparkEditView

IMPLEMENT_DYNCREATE(CSparkEditView, CView)

BEGIN_MESSAGE_MAP(CSparkEditView, CView)
	//{{AFX_MSG_MAP(CSparkEditView)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSparkEditView construction/destruction

CSparkEditView::CSparkEditView()
{
}

CSparkEditView::~CSparkEditView()
{
}

BOOL CSparkEditView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CSparkEditView drawing

void CSparkEditView::OnDraw(CDC* pDC)
{
	CSparkEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
}

/////////////////////////////////////////////////////////////////////////////
// CSparkEditView printing

BOOL CSparkEditView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CSparkEditView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

void CSparkEditView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

/////////////////////////////////////////////////////////////////////////////
// CSparkEditView diagnostics

#ifdef _DEBUG
void CSparkEditView::AssertValid() const
{
	CView::AssertValid();
}

void CSparkEditView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSparkEditDoc* CSparkEditView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSparkEditDoc)));
	return (CSparkEditDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSparkEditView message handlers

void CSparkEditView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	
}
