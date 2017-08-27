// SparkEditDoc.h : interface of the CSparkEditDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPARKEDITDOC_H__71992CCB_E753_48E6_B387_4AFE33B61159__INCLUDED_)
#define AFX_SPARKEDITDOC_H__71992CCB_E753_48E6_B387_4AFE33B61159__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CSparkEditDoc : public CDocument
{
protected: // create from serialization only
	CSparkEditDoc();
	DECLARE_DYNCREATE(CSparkEditDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSparkEditDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSparkEditDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSparkEditDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPARKEDITDOC_H__71992CCB_E753_48E6_B387_4AFE33B61159__INCLUDED_)
