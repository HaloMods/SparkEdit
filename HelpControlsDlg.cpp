// HelpControlsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "sparkedit.h"
#include "HelpControlsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHelpControlsDlg dialog


CHelpControlsDlg::CHelpControlsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHelpControlsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHelpControlsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CHelpControlsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHelpControlsDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHelpControlsDlg, CDialog)
	//{{AFX_MSG_MAP(CHelpControlsDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHelpControlsDlg message handlers
