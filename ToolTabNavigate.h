#if !defined(AFX_TOOLTABNAVIGATE_H__DDA9848F_7140_4426_ABA6_EA11E5BDEEE7__INCLUDED_)
#define AFX_TOOLTABNAVIGATE_H__DDA9848F_7140_4426_ABA6_EA11E5BDEEE7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ToolTabNavigate.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CToolTabNavigate dialog

class CToolTabNavigate : public CPropertyPage
{
	DECLARE_DYNCREATE(CToolTabNavigate)

// Construction
public:
	void Reset(void);
	CToolTabNavigate();
	~CToolTabNavigate();

// Dialog Data
	//{{AFX_DATA(CToolTabNavigate)
	enum { IDD = IDD_TOOL_NAVIGATE };
	CComboBox	m_RenderModeCombo;
	int		m_CameraSpeed;
	float	m_FindCoordX;
	float	m_FindCoordY;
	float	m_FindCoordZ;
	int		m_SelectedBsp;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CToolTabNavigate)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CToolTabNavigate)
	afx_msg void OnFindCoord();
	afx_msg void OnResetCamera();
	afx_msg void OnReleasedcaptureTravelSpeed(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeRenderMode();
	afx_msg void OnSelchangeActiveBsp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLTABNAVIGATE_H__DDA9848F_7140_4426_ABA6_EA11E5BDEEE7__INCLUDED_)
