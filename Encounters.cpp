// Encounters.cpp: implementation of the CEncounters class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sparkedit.h"
#include "Encounters.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEncounters::CEncounters()
{
  m_pEncounters = NULL;
  m_pEncounterInfo = NULL;

  m_ActiveEncounter = 0;
  m_ActiveSquad = 0;

  m_Magic = 0;
  m_Version = 0;
  m_pMapFile = NULL;
  m_EncounterCount = 0;
}

CEncounters::~CEncounters()
{
  Cleanup();
}

void CEncounters::Initialize(CFile *pMapFile, int magic, UINT version)
{
  m_pMapFile = pMapFile;
  m_Magic = magic;
  m_Version = version;
}

void CEncounters::Cleanup()
{
  for(int i=0; i<m_EncounterCount; i++)
  {
    for(int j=0; j<m_pEncounters[i].Squads.Count; j++)
    {
      delete [] m_pEncounterInfo[i].ppSquadSpawns[j];
    }

    delete [] m_pEncounterInfo[i].ppSquadSpawns;
    delete [] m_pEncounterInfo[i].pSquads;
  }

  if(m_pEncounters)
    delete [] m_pEncounters;
  m_pEncounters = NULL;

  if(m_pEncounterInfo)
    delete [] m_pEncounterInfo;
  m_pEncounterInfo = NULL;

  m_ActiveEncounter = 0;
  m_ActiveSquad = 0;

  m_EncounterCount = 0;
  m_Magic = 0;
  m_Version = 0;
  m_pMapFile = NULL;
}

void CEncounters::LoadEncounters(REFLEXIVE EncInfo)
{
  int i, j;
  ENC_SQUAD *pSquad;

  if(EncInfo.Count > 0)
  {
    m_EncounterCount = EncInfo.Count;
    m_pEncounters = new ENCOUNTER[EncInfo.Count];
    m_pEncounterInfo= new ENCOUNTER_INFO[EncInfo.Count];

    m_pMapFile->Seek(EncInfo.Offset, 0);
    m_pMapFile->Read(m_pEncounters, sizeof(ENCOUNTER)*EncInfo.Count);
    ZeroMemory(m_pEncounterInfo, sizeof(ENCOUNTER_INFO)*EncInfo.Count);

    for(i=0; i<EncInfo.Count; i++)
    {
      //Load Squads
      if(m_pEncounters[i].Squads.Count > 0)
      {
        m_pEncounters[i].Squads.Offset -= m_Magic;

        m_pEncounterInfo[i].pSquads = new ENC_SQUAD[m_pEncounters[i].Squads.Count];
        
        m_pEncounterInfo[i].ppSquadSpawns = new ENC_SQUAD_SPAWN*[m_pEncounters[i].Squads.Count];
        ZeroMemory(m_pEncounterInfo[i].ppSquadSpawns, m_pEncounters[i].Squads.Count*sizeof(ENC_SQUAD_SPAWN*));

        m_pMapFile->Seek(m_pEncounters[i].Squads.Offset, 0);
        m_pMapFile->Read(m_pEncounterInfo[i].pSquads, sizeof(ENC_SQUAD)*m_pEncounters[i].Squads.Count);

        for(j=0; j<m_pEncounters[i].Squads.Count; j++)
        {
          pSquad = &m_pEncounterInfo[i].pSquads[j];

          if(pSquad->StartLocations.Count > 0)
          {
            pSquad->StartLocations.Offset -= m_Magic;
            m_pEncounterInfo[i].ppSquadSpawns[j] = new ENC_SQUAD_SPAWN[pSquad->StartLocations.Count];
            
            m_pMapFile->Seek(pSquad->StartLocations.Offset, 0);
            m_pMapFile->Read(m_pEncounterInfo[i].ppSquadSpawns[j], sizeof(ENC_SQUAD_SPAWN)*pSquad->StartLocations.Count);
          }
        }
      }


      if(m_pEncounters[i].Platoons.Count > 0)
        m_pEncounters[i].Platoons.Offset -= m_Magic;

      if(m_pEncounters[i].FiringPositions.Count > 0)
        m_pEncounters[i].FiringPositions.Offset -= m_Magic;
      
      if(m_pEncounters[i].PlayerStartLocations.Count > 0)
        m_pEncounters[i].PlayerStartLocations.Offset -= m_Magic;
    }
  }
  
//  AnalyzeFileSection(m_pMapFile, m_Header.Encounters.Offset, 
//    (m_Header.Squads.Offset - m_Header.Encounters.Offset), m_Magic);

}

void CEncounters::ActivateSquad(int encounter, int squad)
{
  m_ActiveEncounter = encounter;
  m_ActiveSquad = squad;
}

int CEncounters::GetEncounterCount()
{
  return(m_EncounterCount);
}

int CEncounters::GetSquadCount(int EncounterIndex)
{
  int squad_count = 0;

  if(EncounterIndex < m_EncounterCount)
    squad_count = m_pEncounters[EncounterIndex].Squads.Count;

  return(squad_count);
}

void CEncounters::GetSquadSpawnCoord(int index, float *pCoord)
{
  memcpy(pCoord, &m_pEncounterInfo[m_ActiveEncounter].ppSquadSpawns[m_ActiveSquad][index].x, 16);
}

int CEncounters::GetSquadSpawnCount(int encounter, int squad)
{
  int spawn_count = 0;

  spawn_count = m_pEncounterInfo[encounter].pSquads[squad].StartLocations.Count;

  return(spawn_count);
}
