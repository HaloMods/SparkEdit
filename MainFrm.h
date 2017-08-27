// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__80E4A9AB_CC58_4756_AEAC_6DAFAB6AFDC2__INCLUDED_)
#define AFX_MAINFRM_H__80E4A9AB_CC58_4756_AEAC_6DAFAB6AFDC2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ToolView.h"
#include "OutputPane.h"

#define INFOBAR_SIZE     150
#define TOOL_PANE_WIDTH  300

class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
  CSplitterWnd m_ToolSplitter;
  CSplitterWnd m_OutputSplitter;
  BOOL m_bSplittersInitialized;
  int m_EnableTranslateEdit;
  int m_EnableRotateEdit;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnOpenCachefile();
	afx_msg void OnHelpControls();
	afx_msg void OnOpenCompressed();
	afx_msg void OnFileExportMeshToObj();
	afx_msg void OnFileSavechanges();
	afx_msg void OnImportObj();
	afx_msg void OnFileClosecachefile();
	afx_msg void OnFileCompressMap();
	afx_msg void OnToolsOptions();
	afx_msg void OnButtonRotateMode();
	afx_msg void OnUpdateButtonRotateMode(CCmdUI* pCmdUI);
	afx_msg void OnButtonTranslateMode();
	afx_msg void OnUpdateButtonTranslateMode(CCmdUI* pCmdUI);
	afx_msg void OnButtonSelectMode();
	afx_msg void OnUpdateButtonSelectMode(CCmdUI* pCmdUI);
	afx_msg void OnButtonInfoDlg();
	afx_msg void OnUpdateButtonInfoDlg(CCmdUI* pCmdUI);
	afx_msg void OnToolsRotatemode();
	afx_msg void OnUpdateToolsRotatemode(CCmdUI* pCmdUI);
	afx_msg void OnToolsSelectmode();
	afx_msg void OnUpdateToolsSelectmode(CCmdUI* pCmdUI);
	afx_msg void OnToolsTranslatemode();
	afx_msg void OnUpdateToolsTranslatemode(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnToolsImporttagintoscenario();
	afx_msg void OnToolsDumptaginfo();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnButtonDel();
	afx_msg void OnToolsAddteleporterpair();
	afx_msg void OnToolsExportmeta();
	afx_msg void OnToolsAnalyzeExtern();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__80E4A9AB_CC58_4756_AEAC_6DAFAB6AFDC2__INCLUDED_)
