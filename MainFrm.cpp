// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "SparkEdit.h"

#include "MainFrm.h"
#include "Perspective.h"
#include "HaloMapFile.h"
#include "HelpControlsDlg.h"
//#include "LightwaveImporter.h"
#include "BspManager.h"
#include "ObjectInfoDlg.h"
#include "OptionsDlg.h"
#include "Render.h"
#include "SelectionManager.h"
#include "ImportScenarioTagDlg.h"
#include "TagManager.h"
#include "Scenario.h"
#include "AnalyzeDataDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CHaloMapFile HaloMap;
extern CBspManager gBspManager;
extern CObjectInfoDlg gInfoDlg;
extern CRender gRender;
extern CPerspective *g_pRenderWnd;
extern CSelectionManager gSelection;
extern CTagManager gTagManager;
extern CScenario gScenario;

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_SIZE()
	ON_COMMAND(ID_OPEN_CACHEFILE, OnOpenCachefile)
	ON_COMMAND(ID_HELP_CONTROLS, OnHelpControls)
	ON_COMMAND(ID_OPEN_COMPRESSED, OnOpenCompressed)
	ON_COMMAND(ID_FILE_EXPORTMESHTOOBJ, OnFileExportMeshToObj)
	ON_COMMAND(ID_FILE_SAVECHANGES, OnFileSavechanges)
	ON_COMMAND(ID_IMPORT_OBJ, OnImportObj)
	ON_COMMAND(ID_FILE_CLOSECACHEFILE, OnFileClosecachefile)
	ON_COMMAND(ID_FILE_COMPRESS_MAP, OnFileCompressMap)
	ON_COMMAND(ID_TOOLS_OPTIONS, OnToolsOptions)
	ON_COMMAND(ID_BUTTON_ROTATE_MODE, OnButtonRotateMode)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_ROTATE_MODE, OnUpdateButtonRotateMode)
	ON_COMMAND(ID_BUTTON_TRANSLATE_MODE, OnButtonTranslateMode)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_TRANSLATE_MODE, OnUpdateButtonTranslateMode)
	ON_COMMAND(ID_BUTTON_SELECT_MODE, OnButtonSelectMode)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_SELECT_MODE, OnUpdateButtonSelectMode)
	ON_COMMAND(ID_BUTTON_INFO_DLG, OnButtonInfoDlg)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_INFO_DLG, OnUpdateButtonInfoDlg)
	ON_COMMAND(ID_TOOLS_ROTATEMODE, OnToolsRotatemode)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_ROTATEMODE, OnUpdateToolsRotatemode)
	ON_COMMAND(ID_TOOLS_SELECTMODE, OnToolsSelectmode)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_SELECTMODE, OnUpdateToolsSelectmode)
	ON_COMMAND(ID_TOOLS_TRANSLATEMODE, OnToolsTranslatemode)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_TRANSLATEMODE, OnUpdateToolsTranslatemode)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_TOOLS_IMPORTTAGINTOSCENARIO, OnToolsImporttagintoscenario)
	ON_COMMAND(ID_TOOLS_DUMPTAGINFO, OnToolsDumptaginfo)
	ON_WM_CREATE()
	ON_COMMAND(ID_BUTTON_DEL, OnButtonDel)
	ON_COMMAND(ID_TOOLS_ADDTELEPORTERPAIR, OnToolsAddteleporterpair)
	ON_COMMAND(ID_TOOLS_EXPORTMETA, OnToolsExportmeta)
	ON_WM_CHAR()
	ON_COMMAND(ID_TOOLS_DELETESELECTION, OnButtonDel)
	ON_COMMAND(ID_TOOLS_DUPLICATESELECTION, OnEditCopy)
	ON_COMMAND(ID_TOOLS_ANALYZE_EXTERN, OnToolsAnalyzeExtern)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
  m_bSplittersInitialized = FALSE;
  m_EnableTranslateEdit = 1;
  m_EnableRotateEdit = 0;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

  //object info dialog
  gInfoDlg.Create(IDD_OBJECT_INFO, this);
  gInfoDlg.ShowWindow(SW_HIDE);
  gInfoDlg.m_bShowWindow = FALSE;
	
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	CRect cr;
  BOOL bStatus = TRUE;

	GetClientRect( &cr );

	if(!m_OutputSplitter.CreateStatic(this, 2, 1))
		bStatus = FALSE;


	if(!m_ToolSplitter.CreateStatic(&m_OutputSplitter, 1, 2, WS_CHILD | WS_VISIBLE,
		 m_OutputSplitter.IdFromRowCol(0, 0)))
		bStatus = FALSE;

	if(!m_OutputSplitter.CreateView(1, 0, RUNTIME_CLASS(COutputPane), 
     CSize(cr.Width(), cr.Height() - INFOBAR_SIZE), pContext))
		bStatus = FALSE;
 
	//the render window
  if(!m_ToolSplitter.CreateView(0, 0, RUNTIME_CLASS(CPerspective), 
     CSize(cr.Width() - INFOBAR_SIZE, cr.Height()), pContext))
		bStatus = FALSE;

  if(!m_ToolSplitter.CreateView(0, 1, RUNTIME_CLASS(CToolView), 
     CSize(INFOBAR_SIZE, cr.Height()), pContext))
		bStatus = FALSE;

  m_bSplittersInitialized = TRUE;

	return TRUE;
}

void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	CFrameWnd::OnSize(nType, cx, cy);

  CRect cr;
	
	if(m_bSplittersInitialized && nType != SIZE_MINIMIZED )
	{
		// Adjust the row/column info to compensate for
		// the new size
		m_OutputSplitter.SetColumnInfo(0, cx, 0);
		m_OutputSplitter.SetRowInfo(0, cy - INFOBAR_SIZE, 0);
		m_OutputSplitter.SetRowInfo(1, INFOBAR_SIZE, 0);

    
		// Get the client rectangle for the viewports
		m_OutputSplitter.GetClientRect( &cr );

		// Use the viewport client height rather than
		// the OnSize param cy to get an acurate look.
		//m_ToolSplitter.SetRowInfo( 0, cr.Height(), 0 );
		//m_ToolSplitter.SetRowInfo( 1, cr.Height() / 2, 0 );
		m_ToolSplitter.SetColumnInfo( 0, (cx - TOOL_PANE_WIDTH), 0 );
		m_ToolSplitter.SetColumnInfo( 1, TOOL_PANE_WIDTH, 0 );

		// Apply our changes to the splitters
		m_OutputSplitter.RecalcLayout();
		m_ToolSplitter.RecalcLayout();
	}
}

void CMainFrame::OnOpenCachefile() 
{
  CString path;
  CString tmp;
  CFileDialog dlg(TRUE, "*.*", 0, 0, "Halo Cache Files  (*.map)|*.map|All Files  (*.*)|*.*|");

  gRender.EnableRender(FALSE);
  if(dlg.DoModal() == IDOK)
  {
    path = dlg.GetPathName();
    if(HaloMap.OpenMapFile(path))
    {
      tmp.Format("%s - SparkEdit", path);
      SetWindowText(tmp);
    }
    else
    {
      SetWindowText("Untitled - SparkEdit");
    }
  }
  gRender.EnableRender(TRUE);
}

void CMainFrame::OnHelpControls() 
{
  CHelpControlsDlg dlg;

  gRender.EnableRender(FALSE);
  dlg.DoModal();
  gRender.EnableRender(TRUE);
}

void CMainFrame::OnOpenCompressed() 
{
  CString path, tmp;
  CFileDialog dlg(TRUE, "*.*", 0, 0, "Halo Compressed Files  (*.map)|*.map|All Files (*.*)|*.*|");

  gRender.EnableRender(FALSE);
  if(dlg.DoModal() == IDOK)
  {
    path = dlg.GetPathName();
    tmp.Format("%s - SparkEdit", path);
    SetWindowText(tmp);
    HaloMap.OpenCompressedFile(path);
  }
  gRender.EnableRender(TRUE);
}

void CMainFrame::OnFileExportMeshToObj() 
{
  CString path, default_file;
  int start;

  default_file = gBspManager.GetActiveBspName();
  start = default_file.ReverseFind('\\');
  default_file = default_file.Right(default_file.GetLength() - start - 1);
  
  CFileDialog dlg(FALSE, "*.obj", default_file, 0, "Wavefront Model  (*.obj)|*.obj|All Files (*.*)|*.*|");


  gRender.EnableRender(FALSE);
  if(dlg.DoModal() == IDOK)
  {
    path = dlg.GetPathName();
    gBspManager.ExportActiveBspToObj(path);
    AfxMessageBox("Export complete.");
  }
  gRender.EnableRender(TRUE);
}

void CMainFrame::OnFileSavechanges() 
{
  gRender.EnableRender(FALSE);
  HaloMap.SaveChanges();
  AfxMessageBox("Changes saved.");
  gRender.EnableRender(TRUE);
}

void CMainFrame::OnImportObj() 
{
  /*
  CString path;
  CString tmp;
  CFileDialog dlg(TRUE, "*.obj", 0, 0, "Wavefront Model  (*.obj)|*.obj|All Files  (*.*)|*.*|");
  CLightwaveImporter import;

  if(dlg.DoModal() == IDOK)
  {
    path = dlg.GetPathName();
    import.OpenObjFile(path);
    import.ParseObjFile();
    import.SaveCollisionVertFormat("temp.col");
    if(import.VerifyAgainstLoadedMap())
    {
      import.LoadImportedDataIntoBsp();
      import.UpdateCollisionMesh();
    }
    import.Cleanup();
  }
  */
}

void CMainFrame::OnFileClosecachefile() 
{
  gRender.EnableRender(FALSE);
  if(AfxMessageBox("Are you sure you want to close this file?", MB_YESNO) == IDYES)
  {
    HaloMap.CloseMapFile();
  }
  gRender.EnableRender(TRUE);
}

void CMainFrame::OnFileCompressMap() 
{
  CString path;
  CString tmp;
  CFileDialog dlg(FALSE, "*.*", 0, 0, "Halo Cache Files  (*.map)|*.map|All Files  (*.*)|*.*|");

  gRender.EnableRender(FALSE);
  if(dlg.DoModal() == IDOK)
  {
    path = dlg.GetPathName();
    HaloMap.CompressMapFile(path);
  }
  gRender.EnableRender(TRUE);
  //check to see if file is open

  //check to see if file is version 5


}

void CMainFrame::OnToolsOptions() 
{
  COptionsDlg dlg;

  gRender.EnableRender(FALSE);
  dlg.DoModal();
  gRender.EnableRender(TRUE);
}

void CMainFrame::OnButtonRotateMode() 
{
  gSelection.SetCurrentEditorMode(EDITOR_MODE_ROTATE);
  g_pRenderWnd->EnableRotationEdit(TRUE);
}

void CMainFrame::OnUpdateButtonRotateMode(CCmdUI* pCmdUI) 
{
  EDITOR_MODE mode = gSelection.GetCurrentEditorMode();

  pCmdUI->SetCheck(mode == EDITOR_MODE_ROTATE);
}

void CMainFrame::OnButtonTranslateMode() 
{
  gSelection.SetCurrentEditorMode(EDITOR_MODE_TRANSLATE);
  g_pRenderWnd->EnableRotationEdit(FALSE);
}

void CMainFrame::OnUpdateButtonTranslateMode(CCmdUI* pCmdUI) 
{
  EDITOR_MODE mode = gSelection.GetCurrentEditorMode();

  pCmdUI->SetCheck(mode == EDITOR_MODE_TRANSLATE);
}

void CMainFrame::OnButtonSelectMode() 
{
  gSelection.SetCurrentEditorMode(EDITOR_MODE_SELECT);
}

void CMainFrame::OnUpdateButtonSelectMode(CCmdUI* pCmdUI) 
{
  EDITOR_MODE mode = gSelection.GetCurrentEditorMode();

  pCmdUI->SetCheck(mode == EDITOR_MODE_SELECT);
}

void CMainFrame::OnButtonInfoDlg() 
{
  if(gInfoDlg.m_bShowWindow)
  {
    gInfoDlg.ShowWindow(SW_HIDE);
    gInfoDlg.m_bShowWindow = FALSE;
  }
  else
  {
    gInfoDlg.ShowWindow(SW_SHOW);
    gInfoDlg.m_bShowWindow = TRUE;
  }
}

void CMainFrame::OnUpdateButtonInfoDlg(CCmdUI* pCmdUI) 
{
  pCmdUI->SetCheck(gInfoDlg.m_bShowWindow); 
}

void CMainFrame::OnToolsRotatemode() 
{
  gSelection.SetCurrentEditorMode(EDITOR_MODE_ROTATE);
}

void CMainFrame::OnUpdateToolsRotatemode(CCmdUI* pCmdUI) 
{
  EDITOR_MODE mode = gSelection.GetCurrentEditorMode();

  pCmdUI->SetCheck(mode == EDITOR_MODE_ROTATE);
}

void CMainFrame::OnToolsSelectmode() 
{
  gSelection.SetCurrentEditorMode(EDITOR_MODE_SELECT);
}

void CMainFrame::OnUpdateToolsSelectmode(CCmdUI* pCmdUI) 
{
  EDITOR_MODE mode = gSelection.GetCurrentEditorMode();

  pCmdUI->SetCheck(mode == EDITOR_MODE_SELECT);
}

void CMainFrame::OnToolsTranslatemode() 
{
  gSelection.SetCurrentEditorMode(EDITOR_MODE_TRANSLATE);
}

void CMainFrame::OnUpdateToolsTranslatemode(CCmdUI* pCmdUI) 
{
  EDITOR_MODE mode = gSelection.GetCurrentEditorMode();

  pCmdUI->SetCheck(mode == EDITOR_MODE_TRANSLATE);
}

void CMainFrame::OnEditCopy() 
{
  if(!gSelection.DuplicateActiveSelection())
    AfxMessageBox("Could not duplicate selection.");
}

void CMainFrame::OnToolsImporttagintoscenario() 
{
  CImportScenarioTagDlg dlg;

  gRender.EnableRender(FALSE);
  dlg.DoModal();
  gRender.EnableRender(TRUE);
}

void CMainFrame::OnToolsDumptaginfo() 
{
  CString path;
  CString tmp;
  CFileDialog dlg(FALSE, "*.*", 0, 0, "Text Files  (*.txt)|*.txt|All Files  (*.*)|*.*|");

  gRender.EnableRender(FALSE);
  if(dlg.DoModal() == IDOK)
  {
    path = dlg.GetPathName();
    gTagManager.ExportTagInfo(path);
  }
  gRender.EnableRender(TRUE);
}

void CMainFrame::OnButtonDel() 
{
  gSelection.DeleteActiveSelection();
}

void CMainFrame::OnToolsAddteleporterpair() 
{
  float coord[3] = {0,0,0};
  g_pRenderWnd->m_Camera.GetCameraFrontCoord(coord);
  TRACE("SpawnCoord = %.3f, %.3f, %.3f\n", coord[0], coord[1], coord[2]);
  gScenario.AddTeleporter(coord);
  gSelection.SetSelectionMode(MP_FLAGS);
}

void CMainFrame::OnToolsExportmeta() 
{
  CAnalyzeDataDlg dlg;
  
  if(HaloMap.m_InputFile.m_hFile == (HANDLE)0xffffffff)
  {
    AfxMessageBox("You do not have a mapfile open.");
  }
  else
  {
    dlg.m_Offset = 0x800;
    dlg.m_Size = 0xb1000;//HaloMap.m_InputFile.GetLength();
    dlg.m_XrefMagic = 0x818F4800;

    
    if(dlg.DoModal() == IDOK)
    {
      CWaitCursor wait;

      if((dlg.m_Offset + dlg.m_Size) > HaloMap.m_InputFile.GetLength())
        AfxMessageBox("Offsets are out of range in the mapfile.");
      else
        HaloMap.AnalyzeFileSection3(&HaloMap.m_InputFile, dlg.m_Offset, dlg.m_Size, HaloMap.m_Magic, dlg.m_XrefMagic);
    }
  }
}

void CMainFrame::OnToolsAnalyzeExtern() 
{
  CFileDialog dlg(TRUE, "*.*", 0, 0, "All Files  (*.*)|*.*|");
  CString str;
  
  if(dlg.DoModal() == IDOK)
  {
    CFile infile;
    str = dlg.GetPathName();
    if(!infile.Open(str, CFile::modeRead))
    {
      AfxMessageBox("File open failed.");
    }
    else
    {
      UINT len = infile.GetLength();

      CAnalyzeDataDlg dlg;

      dlg.m_Offset = 0;
      dlg.m_Size = len;

      if(dlg.DoModal() == IDOK)
      {
        CWaitCursor wait;
        HaloMap.AnalyzeFileSection3(&infile, dlg.m_Offset, dlg.m_Size, 0, dlg.m_XrefMagic);
      }

      infile.Close();
    }
    str.ReleaseBuffer();
  }
}
