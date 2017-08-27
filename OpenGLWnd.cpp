#include "stdafx.h"
#include "SparkEdit.h"
#include "OpenGLWnd.h"
#include "Render.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(COpenGLWnd, CView)

extern CRender gRender;

/*-------------------------------------------------------------------
 * Name: COpenGLWnd()
 * Description:
 *   
 *-----------------------------------------------------------------*/
COpenGLWnd::COpenGLWnd()
{
	m_RenderScene = 0;
}

/*-------------------------------------------------------------------
 * Name: ~COpenGLWnd()
 * Description:
 *   
 *-----------------------------------------------------------------*/
COpenGLWnd::~COpenGLWnd()
{
}

/*-------------------------------------------------------------------
 * Name: SetupPixelFormat()
 * Description:
 *   
 *-----------------------------------------------------------------*/
BOOL COpenGLWnd::SetupPixelFormat()
{
	static PIXELFORMATDESCRIPTOR pfd = 
	{
		sizeof(PIXELFORMATDESCRIPTOR),    // size of this pfd
		1,                                // version number
		PFD_DRAW_TO_WINDOW |              // support window
		PFD_SUPPORT_OPENGL |              // support OpenGL
		PFD_DOUBLEBUFFER,                 // double buffered
		PFD_TYPE_RGBA,                    // RGBA type
		24,                               // 24-bit color depth
		0, 0, 0, 0, 0, 0,                 // color bits ignored
		0,                                // no alpha buffer
		0,                                // shift bit ignored
		0,                                // no accumulation buffer
		0, 0, 0, 0,                       // accumulation bits ignored
		16,                               // 16-bit z-buffer
		0,                                // no stencil buffer
		0,                                // no auxiliary buffer
		PFD_MAIN_PLANE,                   // main layer
		0,                                // reserved
		0, 0, 0                           // layer masks ignored
	};

   int m_nPixelFormat = ::ChoosePixelFormat( m_pDC->GetSafeHdc(), &pfd );

    if ( m_nPixelFormat == 0 )
        return FALSE;

    return ::SetPixelFormat( m_pDC->GetSafeHdc(), m_nPixelFormat, &pfd );
}

/*-------------------------------------------------------------------
 * Name: InitOpenGL()
 * Description:
 *   
 *-----------------------------------------------------------------*/
BOOL COpenGLWnd::InitOpenGL()
{
	//Get a DC for the Client Area
	m_pDC = new CClientDC(this);

	//Failure to Get DC
	if( m_pDC == NULL )
		return FALSE;

	if( !SetupPixelFormat() )
		return FALSE;

    //Create Rendering Context
	m_hRC = ::wglCreateContext( m_pDC->GetSafeHdc() );

    //Failure to Create Rendering Context
    if( m_hRC == 0 )
        return FALSE;

	//Make the RC Current
	if( ::wglMakeCurrent( m_pDC->GetSafeHdc(), m_hRC ) == FALSE )
		return FALSE;

	// Usual OpenGL stuff

	glClearColor( 0.4f, 0.4f, 0.4f, 0.0f );
//	glClearColor( 1.0f, 1.0f, 1.0f, 0.0f );
	glClearDepth(1.0f);
	glEnable( GL_DEPTH_TEST );
	glLineWidth( 2.0f );
	glPointSize( 1.0f );
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return TRUE;
}

/*-------------------------------------------------------------------
 * Name: RenderScene()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void COpenGLWnd::RenderScene()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |GL_ACCUM_BUFFER_BIT);

  if(gRender.RenderModel() == FALSE)
  {
    gRender.RenderVisibleBsp(FALSE);
  }
}

BEGIN_MESSAGE_MAP(COpenGLWnd, CView)
	//{{AFX_MSG_MAP(COpenGLWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*-------------------------------------------------------------------
 * Name: OnDraw()
 * Description:
 *   Override the default draw behavior to implement OpenGL drawing.
 *-----------------------------------------------------------------*/
void COpenGLWnd::OnDraw(CDC* pDC)
{
	SetContext();
	RenderScene();
	SwapGLBuffers();
}

#ifdef _DEBUG
/*-------------------------------------------------------------------
 * Name: AssertValid()
 * Description:
 *   Debug function.
 *-----------------------------------------------------------------*/
void COpenGLWnd::AssertValid() const
{
	CView::AssertValid();
}

/*-------------------------------------------------------------------
 * Name: Dump()
 * Description:
 *   Debug function.
 *-----------------------------------------------------------------*/
void COpenGLWnd::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG


/*-------------------------------------------------------------------
 * Name: OnCreate()
 * Description:
 *   
 *-----------------------------------------------------------------*/
int COpenGLWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	if ( !InitOpenGL() )
	{
		MessageBox( "Error setting up OpenGL!", "Init Error!",
			MB_OK | MB_ICONERROR );
		return -1;
	}

	return 0;
}

/*-------------------------------------------------------------------
 * Name: OnDestroy()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void COpenGLWnd::OnDestroy()
{
	CView::OnDestroy();

	wglMakeCurrent(0,0);
	wglDeleteContext(m_hRC);
	if( m_pDC )
	{
		delete m_pDC;
	}
	m_pDC = NULL;
}

/*-------------------------------------------------------------------
 * Name: OnEraseBkgnd()
 * Description:
 *   Override this to prevent flickering.
 *-----------------------------------------------------------------*/
BOOL COpenGLWnd::OnEraseBkgnd(CDC* pDC) 
{	
	return TRUE;
}

/*-------------------------------------------------------------------
 * Name: PreCreateWindow()
 * Description:
 *   
 *-----------------------------------------------------------------*/
BOOL COpenGLWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.lpszClass = ::AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_OWNDC,
		::LoadCursor(NULL, IDC_ARROW), NULL, NULL);
	cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	
	return CView::PreCreateWindow(cs);
}
