#if !defined(AFX_TOOLTABEDIT_H__21D46F84_B000_47F2_8109_E0FFAE949335__INCLUDED_)
#define AFX_TOOLTABEDIT_H__21D46F84_B000_47F2_8109_E0FFAE949335__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HaloStructDefs.h"

#define RAD15 0.2617993878f
#define RAD360 6.283185307f

class CToolTabEdit : public CPropertyPage
{
	DECLARE_DYNCREATE(CToolTabEdit)

// Construction
public:
	void ActivateSelectionType(SELECTION_MODE sel_mode, int object_index, int type);
	void ResetTypePallette(void);
	void SetInfoText(CString str);
	void Reset(void);
	CToolTabEdit();
	~CToolTabEdit();

// Dialog Data
	//{{AFX_DATA(CToolTabEdit)
	enum { IDD = IDD_TOOL_EDIT };
	CComboBox	m_ObjectType;
	int		m_SelectionMode;
	BOOL	m_bViewTriggers;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CToolTabEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CToolTabEdit)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeSelectionMode();
	afx_msg void OnViewObjInfo();
	afx_msg void OnViewTriggers();
	afx_msg void OnSelchangeObjectType();
	afx_msg void OnResetRotation();
	afx_msg void OnSnapPitch();
	afx_msg void OnSnapRoll();
	afx_msg void OnSnapYaw();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

  CStringArray m_TypePallette[SELECTION_MODE_MAX];
  int m_TypePalletteCount[SELECTION_MODE_MAX];
  SELECTION_MODE m_ActiveSelMode;
  int m_ActiveObjectIndex;
  int m_LastReference;
  float m_SnapYaw;
  float m_SnapPitch;
  float m_SnapRoll;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLTABEDIT_H__21D46F84_B000_47F2_8109_E0FFAE949335__INCLUDED_)
