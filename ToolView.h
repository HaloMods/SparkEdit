#ifndef __TOOLVIEW_H__
#define __TOOLVIEW_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ToolView.h : header file
//
#include "ToolSheet.h"

/////////////////////////////////////////////////////////////////////////////
// CToolView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CToolView : public CFormView
{
protected:
	CToolView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CToolView)

// Form Data
public:
	//{{AFX_DATA(CToolView)
	enum { IDD = IDD_TOOLSHEET };
	//}}AFX_DATA

// Attributes
public:

// Operations
public:
	void Reset(void);
	void SetCameraPosInfo(float x, float y, float z);
	void SetInfoText(CString str);
  CToolSheet *m_pToolSheet;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CToolView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CToolView)
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
