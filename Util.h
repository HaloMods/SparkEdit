/*-------------------------------------------------------------------
 *  Original Author: Grenadiac
 *
 *  Contributing Authors:
 *
 *  File Description: 
 *    Utility class written to avoid rewriting analysis code over
 *    and over.  Meant to be parent class of Bitmaps, sounds, models,
 *    etc.
 *
 *-----------------------------------------------------------------*/
#if !defined(AFX_UTIL_H__EBD8366A_75B5_4D64_8374_05979978EA03__INCLUDED_)
#define AFX_UTIL_H__EBD8366A_75B5_4D64_8374_05979978EA03__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HaloStructDefs.h"

typedef struct STRUCT_OFFSET_XREF
{
  UINT source;
  UINT dest;
}OFFSET_XREF;


class CUtil  
{
public:
	void DumpReflexiveInfo(REFLEXIVE* pRefl, CString Name, int struct_size, UINT magic);
	BOOL CompressFloatToShort(float input, SHORT &output);
	void AppendXrefs(UINT *pIn, UINT in_count, UINT buffer_base, UINT min, UINT max, OFFSET_XREF**pOut, UINT *out_count, UINT magic);
  void TruncateAsciiGarbage(char* pBuf, int buf_size);
  BOOL IsAscii(float val, CString * pStr);
  void AnalyzeFileSection3(CFile *pMapFile, UINT offset, UINT len, UINT magic, UINT xref_magic);
  void AnalyzeFileSection2(CFile *pMapFile, UINT offset, UINT len, UINT magic);
	float DotProduct(float *pV1, float *pV2);
	float CalculateCameraDist(float *pCoord);
	void CalculateCrossProduct(float *pU, float *pV, float *pUVcross);
  BOOL CompressVector(UINT& comp_vector, float *pVector);
  float DecompressShortToFloat(SHORT comp_vector);
	void DecompressIntToVector(UINT cvect, float *pVector);
	CString CheckForReference(CFile *pMapFile, UINT val, UINT magic);
	void AnalyzeFileSection(CFile *pMapFile, UINT offset, UINT len, UINT magic, UINT xref_magic);
	void DumpBufferContents(float *pFloat, UINT buffer_base, UINT float_count);
	CUtil();
	virtual ~CUtil();
	void DumpBlockInfo(void);
  virtual void ReadHeader(CFile *pMapFile, INDEX_ITEM *item, int magic, CString Name);
  void DumpInfo(UINT offset, UINT size, CString Name);

protected:
	CString ConvertToText(UINT val);
  BOOL CheckForAscii(int val);
  CString GetAddrRange(UINT addr);
	void ExtractFilename(CString Input, CString *pPath, CString *pFilename);
	CString CreateItemDirectory(CString Name);
	void UpdateBlockInfo(UINT offset, UINT size);
  CFile m_OutputFile;
  CString m_ObjTag;

  int  m_ObjCount;
  UINT m_ObjStart;
  UINT m_ObjEnd;
  
  CString m_OutputPathRoot;
  CString m_WorkingDir;
};

#endif // !defined(AFX_UTIL_H__EBD8366A_75B5_4D64_8374_05979978EA03__INCLUDED_)
