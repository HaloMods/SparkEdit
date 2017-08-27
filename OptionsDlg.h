#if !defined(AFX_OPTIONSDLG_H__7D13FAA6_2692_40CD_AD12_248B9C099A21__INCLUDED_)
#define AFX_OPTIONSDLG_H__7D13FAA6_2692_40CD_AD12_248B9C099A21__INCLUDED_

#include "ProfileSettings.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class COptionsDlg : public CDialog
{
// Construction
public:
	COptionsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COptionsDlg)
	enum { IDD = IDD_OPTIONS_DLG };
	BOOL	m_bInvertMouseLook;
	CString	m_BitmapPath;
	CString	m_CEBitmapPath;
	float	m_DrawDistBipeds;
	float	m_DrawDistDevices;
	float	m_DrawDistScenery;
	float	m_DrawDistVehicles;
	float	m_DrawDistWeapons;
	float	m_CullDistance;
	float	m_DrawDistEquipment;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

  PROFILE_SETTINGS m_Profile;
	// Generated message map functions
	//{{AFX_MSG(COptionsDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSDLG_H__7D13FAA6_2692_40CD_AD12_248B9C099A21__INCLUDED_)
