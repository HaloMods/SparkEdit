#if !defined(AFX_PERSPECTIVE_H__433761F5_5AAF_4EBF_B3BB_F2CD2E20F500__INCLUDED_)
#define AFX_PERSPECTIVE_H__433761F5_5AAF_4EBF_B3BB_F2CD2E20F500__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Perspective.h : header file
//

#include "OpenGLWnd.h"
#include "Camera.h"
#include "glCamera.h"
#include "timer.h"

/////////////////////////////////////////////////////////////////////////////
// CPerspective view

// Implements COpenGLWnd to create a perspective
// viewport.

class CPerspective : public COpenGLWnd
{
protected:
	CPerspective();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CPerspective)

// Attributes
public:
	float m_zoom,
		  m_xpos,
		  m_ypos,
		  m_xrot,
		  m_yrot;
	int	m_lastMouseX,
		  m_lastMouseY;

  float m_LastSelectionPos[3];

// Operations
public:
	void EnableRotationEdit(BOOL bEnable);
	void SetCamForModel(BOOL bModelMode, float dim);
	BOOL m_bLookMode;
	void SetRenderWindowOptions(float cull_dist, BOOL bInvertMouse);
	void GetCameraPosition(float *pPosition);
	void CheckKeyboard(void);
	void RefreshWindow(void);
	void SetCameraSpeed(float scale);
  void OnInitialUpdate();
  float GetCameraSpeed(void);

  CCamera m_Camera;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPerspective)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CPerspective();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	void PerformRotationEdit(UINT nFlags, int dx, int dy);
	void PerformTranslationEdit(UINT nFlags, CPoint point);
  BOOL m_bInvertMouse;
  BOOL m_bEnableRotationMode;
  int   m_CameraAccCount;
	float m_fCameraAcc;
  float m_CameraAccMax;
  CTimer m_CameraAccTimer;
  //glCamera m_Camera;
	float m_fCameraSpeed;
	//{{AFX_MSG(CPerspective)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PERSPECTIVE_H__433761F5_5AAF_4EBF_B3BB_F2CD2E20F500__INCLUDED_)
