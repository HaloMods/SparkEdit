// SparkEditDoc.cpp : implementation of the CSparkEditDoc class
//

#include "stdafx.h"
#include "SparkEdit.h"

#include "SparkEditDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSparkEditDoc

IMPLEMENT_DYNCREATE(CSparkEditDoc, CDocument)

BEGIN_MESSAGE_MAP(CSparkEditDoc, CDocument)
	//{{AFX_MSG_MAP(CSparkEditDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSparkEditDoc construction/destruction

CSparkEditDoc::CSparkEditDoc()
{
}

CSparkEditDoc::~CSparkEditDoc()
{
}

BOOL CSparkEditDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CSparkEditDoc serialization

void CSparkEditDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
	}
	else
	{
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSparkEditDoc diagnostics

#ifdef _DEBUG
void CSparkEditDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSparkEditDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSparkEditDoc commands
