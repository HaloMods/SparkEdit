#if !defined(AFX_TOOLMULTIPLAYER_H__3BFFE88B_E8F9_48E2_BAA3_5AD8921F6860__INCLUDED_)
#define AFX_TOOLMULTIPLAYER_H__3BFFE88B_E8F9_48E2_BAA3_5AD8921F6860__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ToolMultiplayer.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CToolMultiplayer dialog

class CToolMultiplayer : public CPropertyPage
{
	DECLARE_DYNCREATE(CToolMultiplayer)

// Construction
public:
	CToolMultiplayer();
	~CToolMultiplayer();

// Dialog Data
	//{{AFX_DATA(CToolMultiplayer)
	enum { IDD = IDD_TOOL_TEMPLATE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CToolMultiplayer)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CToolMultiplayer)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLMULTIPLAYER_H__3BFFE88B_E8F9_48E2_BAA3_5AD8921F6860__INCLUDED_)
