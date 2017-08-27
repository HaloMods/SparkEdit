#if !defined(AFX_TOOLTABMODELS_H__172ABF34_A827_4EE0_BE4E_0D4DD670AB36__INCLUDED_)
#define AFX_TOOLTABMODELS_H__172ABF34_A827_4EE0_BE4E_0D4DD670AB36__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ToolTabModels.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CToolTabModels dialog

class CToolTabModels : public CPropertyPage
{
	DECLARE_DYNCREATE(CToolTabModels)

// Construction
public:
	CToolTabModels();
	~CToolTabModels();
	void InitializeModelList(void);
  float m_fOriginalCamSpeed;

// Dialog Data
	//{{AFX_DATA(CToolTabModels)
	enum { IDD = IDD_TOOL_MODELS };
	CComboBox	m_MarkerList;
	CListBox	m_ModelList;
	int		m_ModelLOD;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CToolTabModels)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CToolTabModels)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeModelList();
	afx_msg void OnSelchangeModelLod();
	afx_msg void OnSelchangeModelMarkers();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLTABMODELS_H__172ABF34_A827_4EE0_BE4E_0D4DD670AB36__INCLUDED_)
