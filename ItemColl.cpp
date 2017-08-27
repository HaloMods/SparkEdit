// ItemColl.cpp: implementation of the CItemColl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sparkedit.h"
#include "ItemColl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItemColl::CItemColl()
{
  m_pMapFile = 0;
  m_Magic = 0;
  m_pChunk1 = NULL;
}

CItemColl::~CItemColl()
{
  Cleanup(); 
}

void CItemColl::Initialize(CFile *pMapFile, UINT magic)
{
  m_pMapFile = pMapFile;
  m_Magic = magic;
}

void CItemColl::Cleanup()
{
  m_pMapFile = 0;
  m_Magic = 0;

  if(m_pChunk1)
    delete [] m_pChunk1;
  m_pChunk1 = NULL;
}

void CItemColl::Load(INDEX_ITEM *pItem)
{
  int i;
  //read in header
  m_pMapFile->Seek(pItem->offset, 0);
  m_pMapFile->Read(&m_Header, sizeof(m_Header));

  m_Header.chunk1.Offset -= m_Magic;

  printf("CHUNK1:  count = %d  offset = %08X\n", m_Header.chunk1.Count, m_Header.chunk1.Offset);
  printf("----------------------------------------------------\n");

  if(m_Header.chunk1.Count)
  {
    m_pChunk1 = new ITEMCOLL_CHUNK1[m_Header.chunk1.Count];

    m_pMapFile->Seek(m_Header.chunk1.Offset, 0);
    m_pMapFile->Read(m_pChunk1, sizeof(ITEMCOLL_CHUNK1)*m_Header.chunk1.Count);
    
    for(i=0; i<m_Header.chunk1.Count; i++)
    {
    }
  }
}
