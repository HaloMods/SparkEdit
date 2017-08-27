// SparkEditView.h : interface of the CSparkEditView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPARKEDITVIEW_H__85EB35EF_C531_46ED_9D86_8B6238663C86__INCLUDED_)
#define AFX_SPARKEDITVIEW_H__85EB35EF_C531_46ED_9D86_8B6238663C86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CSparkEditView : public CView
{
protected: // create from serialization only
	CSparkEditView();
	DECLARE_DYNCREATE(CSparkEditView)

// Attributes
public:
	CSparkEditDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSparkEditView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSparkEditView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSparkEditView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in SparkEditView.cpp
inline CSparkEditDoc* CSparkEditView::GetDocument()
   { return (CSparkEditDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPARKEDITVIEW_H__85EB35EF_C531_46ED_9D86_8B6238663C86__INCLUDED_)
