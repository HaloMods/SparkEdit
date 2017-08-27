// ProfileSettings.h: interface for the CProfileSettings class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROFILESETTINGS_H__0C06D881_8183_45C6_B513_272386670FB0__INCLUDED_)
#define AFX_PROFILESETTINGS_H__0C06D881_8183_45C6_B513_272386670FB0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Util.h"

typedef struct STRUCT_SEL_SETTING
{
  BOOL bShowSelection;
  float HighDetailDrawDist;
}SEL_SETTING;

typedef struct STRUCT_PROFILE_SETTINGS
{
  int ProfileVersion;
  char BitmapPath[256];
  char CEBitmapPath[256];
  SEL_SETTING VehicleSel;
  SEL_SETTING ScenerySel;
  SEL_SETTING WeaponSel;
  SEL_SETTING EquipSel;
  SEL_SETTING MachinesSel;
  SEL_SETTING BipedsSel;
  float CullDistance;
  RECT InfoDlgRect;
  BOOL bInvertMouseLook;
}PROFILE_SETTINGS;

class CProfileSettings : public CUtil
{
public:
	float GetCullDistance(void);
	void SetProfile(PROFILE_SETTINGS *pProfile);
	void GetProfile(PROFILE_SETTINGS* pProfile);
	void GetInfoDlgPosition(RECT *pRect);
	void SetInfoDlgPosition(RECT rc);
	void SaveBinaryProfile(void);
	void LoadBinaryProfile(void);
	CString GetBitmapPath(void);
	CString GetCEBitmapPath(void);
	CProfileSettings();
	virtual ~CProfileSettings();

  PROFILE_SETTINGS m_BinProfile;

protected:

  CFile m_NewPers;
};

#endif // !defined(AFX_PROFILESETTINGS_H__0C06D881_8183_45C6_B513_272386670FB0__INCLUDED_)
