// WeaponHud.h: interface for the CWeaponHud class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WEAPONHUD_H__3F32FCBA_F1B8_4DB9_A15A_1ED8B22164EB__INCLUDED_)
#define AFX_WEAPONHUD_H__3F32FCBA_F1B8_4DB9_A15A_1ED8B22164EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Util.h"

class CWeaponHud : public CUtil  
{
public:
	void LoadWeaponHud(INDEX_ITEM *pItem);
	void Initialize(CFile *pMapFile, UINT magic);
	CWeaponHud();
	virtual ~CWeaponHud();

  CFile *m_pMapFile;
  UINT m_Magic;
};

#endif // !defined(AFX_WEAPONHUD_H__3F32FCBA_F1B8_4DB9_A15A_1ED8B22164EB__INCLUDED_)
