// ModelManager.h: interface for the CModelManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MODELMANAGER_H__0ED37192_48EA_41EB_A84C_F0F58746614C__INCLUDED_)
#define AFX_MODELMANAGER_H__0ED37192_48EA_41EB_A84C_F0F58746614C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Util.h"
#include "PcModel.h"
#include "XboxModel.h"

class CModelManager : public CUtil  
{
public:
	CString GetModelName(int index);
	void GetModel(int index, CPcModel **ppPcModel, CXboxModel **ppXboxModel);
	UINT GetModelCount(void);
	void CompileModelTextures(void);
	void FindMatchingXboxModel(UINT TopLevelObjTag, CXboxModel** ppPcModel);
	void FindMatchingPcModel(UINT TopLevelObjTag, CPcModel** ppPcModel);
	
  CXboxModel* AddXboxModel(UINT tag_id, INDEX_ITEM *pIndexItem);
	CPcModel* AddPcModel(UINT tag_id, INDEX_ITEM *pIndexItem);
	void Initialize(CFile *pMapFile, UINT magic, UINT vert_offset, UINT indices_offset, UINT version);
	void Cleanup(void);
	CModelManager();
	virtual ~CModelManager();

  CPcModel m_PcModels[500];
  CXboxModel m_XboxModels[500];
  UINT m_TagIdList[500];
  UINT m_PcModelCount;
  UINT m_XboxModelCount;
  CFile *m_pMapFile;
  UINT m_Magic;
  UINT m_PcModelVertOffset;
  UINT m_PcModelIndOffset;
  UINT m_Version;
};

#endif // !defined(AFX_MODELMANAGER_H__0ED37192_48EA_41EB_A84C_F0F58746614C__INCLUDED_)
