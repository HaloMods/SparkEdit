#if !defined(AFX_OBJECTINFODLG_H__191B8524_812F_4F5B_B30B_151E61B55DA1__INCLUDED_)
#define AFX_OBJECTINFODLG_H__191B8524_812F_4F5B_B30B_151E61B55DA1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ObjectInfoDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CObjectInfoDlg dialog

class CObjectInfoDlg : public CDialog
{
// Construction
public:
	BOOL m_bUserTouched;
  BOOL m_bShowWindow;
	void SetObjectInfo(CString str);
	CObjectInfoDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CObjectInfoDlg)
	enum { IDD = IDD_OBJECT_INFO };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CObjectInfoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CObjectInfoDlg)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMove(int x, int y);
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJECTINFODLG_H__191B8524_812F_4F5B_B30B_151E61B55DA1__INCLUDED_)
