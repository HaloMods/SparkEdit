#if !defined(AFX_TOOLSHEET_H__C71152B3_8009_4619_A970_E2BD7A2341DB__INCLUDED_)
#define AFX_TOOLSHEET_H__C71152B3_8009_4619_A970_E2BD7A2341DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ToolSheet.h : header file
//

#include "ToolTabEdit.h"
#include "ToolTabNavigate.h"
#include "ToolTabModels.h"

/////////////////////////////////////////////////////////////////////////////
// CToolSheet

class CToolSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CToolSheet)

// Construction
public:
  CToolSheet(CWnd *pParentWnd);
	CToolSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CToolSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:
  CToolTabNavigate m_TabNavigate;
  CToolTabEdit m_EditTab;
  CToolTabModels m_ModelsTab;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolSheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CToolSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CToolSheet)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLSHEET_H__C71152B3_8009_4619_A970_E2BD7A2341DB__INCLUDED_)
