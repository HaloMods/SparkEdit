// ObjectInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "sparkedit.h"
#include "ObjectInfoDlg.h"
#include "ProfileSettings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CProfileSettings gProfile;

CObjectInfoDlg::CObjectInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CObjectInfoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CObjectInfoDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
  m_bUserTouched = FALSE;
  m_bShowWindow = FALSE;
}


void CObjectInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CObjectInfoDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CObjectInfoDlg, CDialog)
	//{{AFX_MSG_MAP(CObjectInfoDlg)
	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectInfoDlg message handlers

void CObjectInfoDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

  CRect edit_rc, dlg_rc, dlg_pos;
  CWnd *pInfoWnd = (CWnd*)GetDlgItem(IDC_OBJECT_INFO);	

  if(m_hWnd&&pInfoWnd)
  {
    GetClientRect(&dlg_rc);
    pInfoWnd->GetClientRect(&edit_rc);

    pInfoWnd->SetWindowPos(NULL, 10, 10, dlg_rc.right-10, dlg_rc.bottom - 10, NULL);

    GetWindowRect(&dlg_pos);
    gProfile.SetInfoDlgPosition(dlg_pos);
  }
}

void CObjectInfoDlg::SetObjectInfo(CString str)
{
  ((CWnd*)GetDlgItem(IDC_OBJECT_INFO))->SetWindowText(str);
}

void CObjectInfoDlg::OnMove(int x, int y) 
{
	CDialog::OnMove(x, y);
	
  if(m_bUserTouched)
  {
    CRect dlg_pos;
    GetWindowRect(&dlg_pos);
    gProfile.SetInfoDlgPosition(dlg_pos);
  }
}

BOOL CObjectInfoDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  CRect dlg_pos;
  
  gProfile.GetInfoDlgPosition(dlg_pos);
  SetWindowPos(NULL, dlg_pos.left, 
               dlg_pos.top, 
               (dlg_pos.right - dlg_pos.left), 
               (dlg_pos.bottom - dlg_pos.top),
               NULL);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CObjectInfoDlg::OnClose() 
{
	m_bShowWindow = FALSE;
	
	CDialog::OnClose();
}
