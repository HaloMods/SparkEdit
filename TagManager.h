#if !defined(AFX_TAGMANAGER_H__0C7C3623_753B_4660_9BBE_F7540E3E3FA8__INCLUDED_)
#define AFX_TAGMANAGER_H__0C7C3623_753B_4660_9BBE_F7540E3E3FA8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Util.h"
#include "PcModel.h"
#include "XboxModel.h"

typedef enum ENUM_BITMAP_STATUS
{
  BITMAP_DISABLED,
  BITMAP_ENABLED,
  LIGHTMAP
}BITMAP_STATUS;

typedef struct STRUCT_TAG_LOOKUP
{
  UINT tagclass[3];
  int  tag_id;
  UINT meta_offset;
  UINT meta_size;
  UINT ModelTag;
  UINT CollisionModelTag;
  UINT ShaderTag;
  UINT BaseTextureTag;
  UINT MultiTexture[5];
  UINT PhysicsTag;
  UINT WeaponTag;
  UINT EquipmentTag;
  CPcModel *pPcModel;
  CXboxModel *pXboxModel;
  UINT RawNameOffset;
  int  BitmapIndex;
  BITMAP_STATUS BitmapStatus;
  char name[128];
}TAG_LOOKUP;


class CTagManager : public CUtil  
{
public:
	void ExportTagInfo(CString filename);
	void GetHighLevelTagInfo(UINT tag_type, int index, UINT *pTagId, UINT *pRawNameOffset, CString *pStr);
	UINT GetHighLevelTagCount(UINT tag_type);
	void ActivateLightmap(UINT lightmap_tag);
	CString GetTagDescription(UINT tag_id);
	BITMAP_STATUS GetTextureStatus(int bitmap_tagid);
	CString GetTagName(UINT tag_id);
	void CompileReferences(void);
	void DumpTags(void);
	void SetPcModelTag(UINT tag_id, CPcModel *pPcModel);
	void SetXboxModelTag(UINT tag_id, CXboxModel *pXboxModel);
	void FindMatchingSubTags(int tag_index, UINT *pSearchBuf, UINT count);
	void CompileTagList(void);
	void GetTagPhysics(UINT tag_id);
	void InitTag(int index, INDEX_ITEM *pItem);
	void Cleanup();
  BOOL CheckForTag(UINT val);
  void GetTagXboxModel(UINT tag_id, CXboxModel **ppXboxModel);
	void GetTagPcModel(UINT tag_id, CPcModel **ppPcModel);
	CTagManager();
	virtual ~CTagManager();
  void Initialize(CFile *pMapFile, UINT magic, UINT tag_count, UINT base_tag);
  int GetBaseTextureIndex(UINT shader_tag_id);
  void GetShaderTextures(UINT shader_tag_id, int &base_texture_index, 
                         int &detail1_texture_index, int &detail2_texture_index);

protected:
  TAG_LOOKUP *m_pTagLookup;
  UINT m_Magic;
  UINT m_TagCount;
  UINT m_BaseTag;
  int m_BitmapCount;
  CStringArray m_TagNames;
  CFile *m_pMapFile;
};

#endif // !defined(AFX_TAGMANAGER_H__0C7C3623_753B_4660_9BBE_F7540E3E3FA8__INCLUDED_)
