#if !defined(AFX_OUTPUTPANE_H__9AE9995E_6F8D_4443_96F8_710C7DE71980__INCLUDED_)
#define AFX_OUTPUTPANE_H__9AE9995E_6F8D_4443_96F8_710C7DE71980__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OutputPane.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COutputPane form view

#include "resource.h"
#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#define LOG_RED   RGB(255, 0, 0)
#define LOG_BLACK RGB(0, 0, 0)
#define LOG_GREY  RGB(128, 128, 128)
#define LOG_BLUE  RGB(0, 0, 255)

class COutputPane : public CFormView
{
protected:
	COutputPane();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(COutputPane)

// Form Data
public:
	//{{AFX_DATA(COutputPane)
	enum { IDD = IDD_OUTPUT_PANE };
	CRichEditCtrl	m_OutputWin;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:
	void PostText(CString msg, COLORREF color);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COutputPane)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~COutputPane();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(COutputPane)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OUTPUTPANE_H__9AE9995E_6F8D_4443_96F8_710C7DE71980__INCLUDED_)
