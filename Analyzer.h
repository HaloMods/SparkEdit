// Analyzer.h: interface for the CAnalyzer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANALYZER_H__CDCECDDF_84EB_4285_817D_01C8ED8F69B1__INCLUDED_)
#define AFX_ANALYZER_H__CDCECDDF_84EB_4285_817D_01C8ED8F69B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Util.h"
#include "HaloStructDefs.h"

class CAnalyzer : public CUtil  
{
public:
	void Initialize(CFile *pMapFile, UINT magic);
	void AddTagString(CString tag_str);
	void Cleanup(void);
	void SetIndexInfo(UINT tag_count, UINT base_tag);
	void SearchForVector(float *v3, float *pBuf, int size, UINT base_addr);
	void SearchForIndexDistribution(CFile *pMapFile);
	void ConvertToFloat(CFile *pMapFile, UINT start_addr, UINT stop_addr);
	void SearchFileForNumber(CFile *pMapFile, UINT num);
	void SearchForPattern(CFile *pMapFile);
	void SearchForBspHeader(CFile *pMapFile);
	void SearchForFloats(CFile *pMapFile);
	void SetOffsetMinMax(int min, int max, int magic);
	void SearchMetaForValidOffsets(CFile *pMapFile, INDEX_ITEM *pItem, int index, CString tag, CString Name);
	CAnalyzer();
	virtual ~CAnalyzer();
  void SuperAnalyzeFileSection(UINT offset, UINT len);
protected:
  CString CheckForTag(UINT val);
	int SearchBufferForTextureCoords(BYTE *pBuf, DWORD buf_size, DWORD byte_align);
	int SearchBufferForFloatVectors(BYTE *pBuf, DWORD buf_size, DWORD byte_align);
	int SearchBufferForFloats(BYTE *pBuf, DWORD buf_size, DWORD byte_align);
	CString GetRawSectionText(CFile *pMapFile, int offset, CString tag);
  int m_OffsetMin;
  int m_OffsetMax;

  CFile *m_pMapFile;
  UINT m_Magic;
  UINT m_TagCount;
  UINT m_BaseTag;
  CStringArray m_TagList;
};

#endif // !defined(AFX_ANALYZER_H__CDCECDDF_84EB_4285_817D_01C8ED8F69B1__INCLUDED_)
