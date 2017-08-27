// SparkEdit.h : main header file for the SPARKEDIT application
//

#if !defined(AFX_SPARKEDIT_H__63B7E240_ECC5_412F_8746_00757DD279BE__INCLUDED_)
#define AFX_SPARKEDIT_H__63B7E240_ECC5_412F_8746_00757DD279BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CSparkEditApp:
// See SparkEdit.cpp for the implementation of this class
//

class CSparkEditApp : public CWinApp
{
public:
	CSparkEditApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSparkEditApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CSparkEditApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFileOpen();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPARKEDIT_H__63B7E240_ECC5_412F_8746_00757DD279BE__INCLUDED_)
