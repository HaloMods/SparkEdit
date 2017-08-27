// ItemColl.h: interface for the CItemColl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITEMCOLL_H__398795B0_23C8_4FD0_B72E_D1AC84042D08__INCLUDED_)
#define AFX_ITEMCOLL_H__398795B0_23C8_4FD0_B72E_D1AC84042D08__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Util.h"
#include "HaloStructDefs.h"

typedef struct STRUCT_ITEMCOLL_HEADER
{
  REFLEXIVE chunk1;
  UINT unk[20];
}ITEMCOLL_HEADER;

typedef struct STRUCT_ITEMCOLL_CHUNK1
{
  UINT unk1[8];
  UINT unk_data;
  UINT ItemType;  //tag type
  UINT Offset2;
  UINT unk2;
  UINT ItemTagId;
  UINT unk3[8];
  SHORT UnkCount1a;
  SHORT UnkCount1b;
  float unk4;
}ITEMCOLL_CHUNK1;


class CItemColl : public CUtil  
{
public:
	void Load(INDEX_ITEM *pItem);
	void Cleanup(void);
	void Initialize(CFile *pMapFile, UINT magic);
	CItemColl();
	virtual ~CItemColl();

  UINT m_Magic;
  CFile *m_pMapFile;

  ITEMCOLL_HEADER m_Header;

  ITEMCOLL_CHUNK1 *m_pChunk1;
};

#endif // !defined(AFX_ITEMCOLL_H__398795B0_23C8_4FD0_B72E_D1AC84042D08__INCLUDED_)
