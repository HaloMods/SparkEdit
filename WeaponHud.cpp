// WeaponHud.cpp: implementation of the CWeaponHud class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sparkedit.h"
#include "WeaponHud.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWeaponHud::CWeaponHud()
{
  m_pMapFile = NULL;
  m_Magic = 0;
}

CWeaponHud::~CWeaponHud()
{

}

void CWeaponHud::Initialize(CFile *pMapFile, UINT magic)
{
  m_pMapFile = pMapFile;
  m_Magic = magic;
}

void CWeaponHud::LoadWeaponHud(INDEX_ITEM *pItem)
{
  CString name;
  INDEX_ITEM *pNext;
  int size;

  pNext = pItem + 1;
  size = pNext->offset - pItem->offset;

  name = CheckForReference(m_pMapFile, pItem->stringoffset, m_Magic);

  printf("\n\n--------- %s\n", name);
  AnalyzeFileSection(m_pMapFile, pItem->offset, size, m_Magic, 0);
}
