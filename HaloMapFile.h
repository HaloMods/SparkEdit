/*-------------------------------------------------------------------
 *  Original Author: Grenadiac
 *
 *  Contributing Authors:
 *
 *  File Description: 
 *    
 *
 *-----------------------------------------------------------------*/
#if !defined(AFX_HALOMAPFILE_H__C9451644_A33B_47C4_89FD_E93B73C2BEFF__INCLUDED_)
#define AFX_HALOMAPFILE_H__C9451644_A33B_47C4_89FD_E93B73C2BEFF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HaloStructDefs.h"
//#include "HaloBmp.h"
#include "Analyzer.h"
#include ".\zlib\zlib.h"
#include "VisibleBsp.h"
#include "XboxModel.h"

class CHaloMapFile : public CUtil
{
public:
	void GetHeaders(MAPFILE_HDR *pMapfileHdr, INDEX_HDR *pIndexHdr);
	void LoadWeaponHuds(void);
	void ReportMapInfo(CStdioFile *pFile);
	BOOL CompressMapFile(CString output_file);
	void LoadScenery(void);
	void LoadModels(void);
	void CloseMapFile(void);
	void SaveChanges(void);
	void LoadScenario(void);
	void OpenCompressedFile(CString path);
	BOOL OpenMapFile(CString path);
	void LoadShaders(void);
	void ConvertFile(void);
	void SearchForFloats(void);
	void ExtractObjects(void);
	void ReadFileIndexing(void);
	BOOL ParseCmdLine(CString cmdline);
	CHaloMapFile();
	virtual ~CHaloMapFile();
  CAnalyzer m_Analyzer;
  CFile m_InputFile;
  int m_Magic;

protected:
	void LoadTagManager(void);
	void Cleanup(void);
	CString GetAddrRange(UINT start_addr);
	void CreateObjectPath(CString path);
  CString m_InputFilename;
  

  MAPFILE_HDR m_MapfileHdr;
  INDEX_HDR   m_IndexHdr;
  INDEX_ITEM  *m_pIndexItem;
  CStringArray m_IndexStr;
};

#endif // !defined(AFX_HALOMAPFILE_H__C9451644_A33B_47C4_89FD_E93B73C2BEFF__INCLUDED_)
