#if !defined(AFX_IMPORTSCENARIOTAGDLG_H__5D448021_33CF_46D2_88E4_3FD477E23E7B__INCLUDED_)
#define AFX_IMPORTSCENARIOTAGDLG_H__5D448021_33CF_46D2_88E4_3FD477E23E7B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ImportScenarioTagDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CImportScenarioTagDlg dialog
typedef struct STRUCT_PALLETTE_ADD
{
  int ItemCount;
  UINT TagId;
  UINT RawName;
  CString GuiName;
}PALLETTE_ADD;

class CImportScenarioTagDlg : public CDialog
{
// Construction
public:
	void SearchForMatchingTags(void);
	CImportScenarioTagDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CImportScenarioTagDlg)
	enum { IDD = IDD_IMPORT_SCENARIO_TAG };
	int		m_AvailableTagIndex;
	int		m_PalletteType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImportScenarioTagDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
  PALLETTE_ADD m_UnregSceneryPallette[100];
  PALLETTE_ADD m_UnregVehiclePallette[100];
  CStringArray m_SceneryItemNames;
  CStringArray m_VehicleItemNames;
  //UINT m_SceneryIds[500];
  //UINT m_VehicleIds[500];
  int m_VehicleItemCount;
  int m_SceneryItemCount;
	// Generated message map functions
	//{{AFX_MSG(CImportScenarioTagDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangePalletteType();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMPORTSCENARIOTAGDLG_H__5D448021_33CF_46D2_88E4_3FD477E23E7B__INCLUDED_)
