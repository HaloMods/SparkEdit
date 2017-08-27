#if !defined(AFX_ANALYZEDATADLG_H__6927563D_8CD7_4EA1_BF8D_17173E170C67__INCLUDED_)
#define AFX_ANALYZEDATADLG_H__6927563D_8CD7_4EA1_BF8D_17173E170C67__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AnalyzeDataDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAnalyzeDataDlg dialog

class CAnalyzeDataDlg : public CDialog
{
// Construction
public:

  UINT m_Offset;
  UINT m_Size;
  UINT m_XrefMagic;

  CAnalyzeDataDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAnalyzeDataDlg)
	enum { IDD = IDD_ANALYZE_DATA_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAnalyzeDataDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAnalyzeDataDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANALYZEDATADLG_H__6927563D_8CD7_4EA1_BF8D_17173E170C67__INCLUDED_)
