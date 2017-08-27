// ToolSheet.cpp : implementation file
//

#include "stdafx.h"
#include "sparkedit.h"
#include "ToolSheet.h"
#include "Perspective.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CPerspective *g_pRenderWnd;

IMPLEMENT_DYNAMIC(CToolSheet, CPropertySheet)

CToolSheet::CToolSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CToolSheet::CToolSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

CToolSheet::CToolSheet(CWnd *pParentWnd = NULL)
  :CPropertySheet(AFX_IDS_APP_TITLE, pParentWnd)
{
}


CToolSheet::~CToolSheet()
{
}


BEGIN_MESSAGE_MAP(CToolSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CToolSheet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

