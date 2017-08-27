// SparkEdit.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "SparkEdit.h"

#include "MainFrm.h"
#include "SparkEditDoc.h"
#include "SparkEditView.h"
#include "HaloMapFile.h"
#include "ShaderManager.h"
#include "Render.h"
#include "Perspective.h"
#include "SelectionManager.h"
#include "Scenario.h"
#include "ProfileSettings.h"
#include "BspManager.h"
#include "ModelManager.h"
#include "Frustum1.h"
#include "TagManager.h"
#include "ObjectInfoDlg.h"
#include "Encounters.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSparkEditApp

BEGIN_MESSAGE_MAP(CSparkEditApp, CWinApp)
	//{{AFX_MSG_MAP(CSparkEditApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

CShaderManager gShaderManager;
CHaloMapFile HaloMap;
CRender gRender;
COutputPane *g_pOutput = NULL;
CPerspective *g_pRenderWnd = NULL;
CSelectionManager gSelection;
CScenario gScenario;
CToolView *g_pTools = NULL;
CProfileSettings gProfile;
CBspManager gBspManager;
CModelManager gModelManager;
CFrustum gFrustum;
CTagManager gTagManager;
CEncounters gAI;

CString gSE_WorkingDir;
CObjectInfoDlg gInfoDlg;

CSparkEditApp::CSparkEditApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSparkEditApp object

CSparkEditApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSparkEditApp initialization

BOOL CSparkEditApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

  AfxInitRichEdit();

	// Change the registry key under which our settings are stored.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));


  char tmp[257];
  memset(tmp, 0, 257);
  _getcwd(tmp, 256);  
  gSE_WorkingDir = tmp;

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

  gProfile.LoadBinaryProfile();

	// Register document templates

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CSparkEditDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CSparkEditView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CSparkEditApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CSparkEditApp message handlers


void CSparkEditApp::OnFileOpen() 
{
}

int CSparkEditApp::ExitInstance() 
{
  //save any profile changes to disk
	gProfile.SaveBinaryProfile();

	return CWinApp::ExitInstance();
}
