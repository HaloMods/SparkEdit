// BspManager.h: interface for the CBspManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BSPMANAGER_H__73848368_3201_4767_B074_DADBB9677147__INCLUDED_)
#define AFX_BSPMANAGER_H__73848368_3201_4767_B074_DADBB9677147__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HaloStructDefs.h"
#include "Util.h"
#include "VisibleBspEx.h"

class CBspManager : public CUtil
{
public:
	void ReportBspInfo(CStdioFile *pFile);
	CString GetMeshInfo(int mesh_index);
	CString GetActiveBspName(void);
	void ExportActiveBspToObj(CString path);
	UINT GetActiveBspSubmeshCount(void);
	SUBMESH_INFO* GetActiveBspSubmesh(UINT mesh_index);
	void GetActiveBspCentroid(float *x, float *y, float *z);
	void LoadBspSections(void);
	void Cleanup();
	CBspManager();
	virtual ~CBspManager();
  void Initialize(CFile *pMapFile, UINT magic);
  void ActivateBsp(int bsp_index);
  void LoadVisibleBspInfo(REFLEXIVE BspChunk, UINT version);

  CStringArray m_BspNames;

  //todo: make protected again
  CVisibleBspEx *m_pBsp;
  UINT m_ActiveBsp;
  SCENARIO_BSP_INFO *m_pBspInfo;

protected:
	UINT m_Version;
  CFile *m_pMapFile;
  UINT m_Magic;
  UINT m_BspCount;
};

#endif // !defined(AFX_BSPMANAGER_H__73848368_3201_4767_B074_DADBB9677147__INCLUDED_)
