#if !defined(AFX_HELPCONTROLSDLG_H__DC5AC8E6_5572_43CC_9A25_A531597A17BA__INCLUDED_)
#define AFX_HELPCONTROLSDLG_H__DC5AC8E6_5572_43CC_9A25_A531597A17BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HelpControlsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHelpControlsDlg dialog

class CHelpControlsDlg : public CDialog
{
// Construction
public:
	CHelpControlsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHelpControlsDlg)
	enum { IDD = IDD_CONTROLS_HELP_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHelpControlsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CHelpControlsDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HELPCONTROLSDLG_H__DC5AC8E6_5572_43CC_9A25_A531597A17BA__INCLUDED_)
