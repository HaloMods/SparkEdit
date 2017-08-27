// ToolMultiplayer.cpp : implementation file
//

#include "stdafx.h"
#include "sparkedit.h"
#include "ToolMultiplayer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CToolMultiplayer property page

IMPLEMENT_DYNCREATE(CToolMultiplayer, CPropertyPage)

CToolMultiplayer::CToolMultiplayer() : CPropertyPage(CToolMultiplayer::IDD)
{
	//{{AFX_DATA_INIT(CToolMultiplayer)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CToolMultiplayer::~CToolMultiplayer()
{
}

void CToolMultiplayer::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CToolMultiplayer)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CToolMultiplayer, CPropertyPage)
	//{{AFX_MSG_MAP(CToolMultiplayer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToolMultiplayer message handlers

BOOL CToolMultiplayer::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
