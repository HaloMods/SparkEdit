// Encounters.h: interface for the CEncounters class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENCOUNTERS_H__57E0F45E_CA10_4864_BC66_A85A42C651EA__INCLUDED_)
#define AFX_ENCOUNTERS_H__57E0F45E_CA10_4864_BC66_A85A42C651EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HaloStructDefs.h"

class CEncounters  
{
public:
	int GetSquadSpawnCount(int encounter, int squad);
	void GetSquadSpawnCoord(int index, float *pCoord);
	int GetSquadCount(int EncounterIndex);
	int GetEncounterCount(void);
	void ActivateSquad(int encounter, int squad);
	void LoadEncounters(REFLEXIVE EncInfo);
	void Cleanup(void);
	CEncounters();
	virtual ~CEncounters();
  void Initialize(CFile *pMapFile, int magic, UINT version);

protected:
  
  ENCOUNTER *m_pEncounters;
  ENCOUNTER_INFO *m_pEncounterInfo;
 
  int m_ActiveEncounter;
  int m_ActiveSquad;

  UINT m_EncounterCount;
  int m_Magic;
  UINT m_Version;
  CFile *m_pMapFile;
};

#endif // !defined(AFX_ENCOUNTERS_H__57E0F45E_CA10_4864_BC66_A85A42C651EA__INCLUDED_)
