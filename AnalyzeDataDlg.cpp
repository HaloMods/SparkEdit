// AnalyzeDataDlg.cpp : implementation file
//

#include "stdafx.h"
#include "sparkedit.h"
#include "AnalyzeDataDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAnalyzeDataDlg dialog


CAnalyzeDataDlg::CAnalyzeDataDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAnalyzeDataDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAnalyzeDataDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CAnalyzeDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAnalyzeDataDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAnalyzeDataDlg, CDialog)
	//{{AFX_MSG_MAP(CAnalyzeDataDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnalyzeDataDlg message handlers

void CAnalyzeDataDlg::OnOK() 
{
  CString str;

  ((CWnd*)GetDlgItem(IDC_ANALYZE_OFFSET))->GetWindowText(str);
  sscanf(str.GetBuffer(10), "%x", &m_Offset);
  str.ReleaseBuffer();
	
  ((CWnd*)GetDlgItem(IDC_ANALYZE_SIZE))->GetWindowText(str);
  sscanf(str.GetBuffer(10), "%x", &m_Size);
  str.ReleaseBuffer();

  ((CWnd*)GetDlgItem(IDC_ANALYZE_MAGIC))->GetWindowText(str);
  sscanf(str.GetBuffer(10), "%x", &m_XrefMagic);
  str.ReleaseBuffer();


  CDialog::OnOK();
}

BOOL CAnalyzeDataDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

  CString str;

  str.Format("%X", m_Offset);
  ((CWnd*)GetDlgItem(IDC_ANALYZE_OFFSET))->SetWindowText(str);

  str.Format("%X", m_Size);
  ((CWnd*)GetDlgItem(IDC_ANALYZE_SIZE))->SetWindowText(str);

  ((CWnd*)GetDlgItem(IDC_ANALYZE_MAGIC))->SetWindowText("0");
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
