// HtmlReport.h: interface for the CHtmlReport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HTMLREPORT_H__61089705_AA26_4D04_9B31_5DA05FF3BD42__INCLUDED_)
#define AFX_HTMLREPORT_H__61089705_AA26_4D04_9B31_5DA05FF3BD42__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CHtmlReport  
{
public:
	void DumpMapInfo(void);
	CHtmlReport();
	virtual ~CHtmlReport();

protected:
  CStdioFile m_HtmlFile;
};

#endif // !defined(AFX_HTMLREPORT_H__61089705_AA26_4D04_9B31_5DA05FF3BD42__INCLUDED_)
