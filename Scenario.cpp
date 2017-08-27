// Scenario.cpp: implementation of the CScenario class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sparkedit.h"
#include "Scenario.h"
#include "CollBsp.h"
#include "BspManager.h"
#include "ModelManager.h"
#include "TagManager.h"
#include "Encounters.h"
#include "ObjectInfoDlg.h"
#include "ToolView.h"
#include "SelectionManager.h"
#include "Render.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CBspManager gBspManager;
extern CModelManager gModelManager;
extern CTagManager gTagManager;
extern CObjectInfoDlg gInfoDlg;
extern CEncounters gAI;
extern CToolView *g_pTools;
extern CSelectionManager gSelection;
extern CRender gRender;

/*-------------------------------------------------------------------
 * Name: CScenario()
 * Description:
 *   
 *-----------------------------------------------------------------*/
CScenario::CScenario()
{
  m_Magic = 0;
  m_pVehicles = NULL;
  m_pScenery = NULL;
  m_pMapFile = NULL;
  m_pWeapons = NULL;
  m_pMpFlags = NULL;
  m_pMpEquip = NULL;
  m_pSceneryRef = NULL;
  m_pVehicleRef = NULL;

  m_pMovePositions = NULL;
  m_pStartingProfile = NULL;
  m_pBspTriggers = NULL;
  m_pDeviceGroups = NULL;
  m_pObjectNames = NULL;
  m_pTriggerVolumes = NULL;
  m_pBiped = NULL;
  m_pBipedRef = NULL;
  m_pEquip = NULL;
  m_pEquipRef = NULL;
  m_pWeaponRef = NULL;
  m_pMachines = NULL;
  m_pMachinesRef = NULL;
  m_pSounds = NULL;
  m_pSoundRef = NULL;

  m_pActorVarRef = NULL;
  m_pAiAnimationRef = NULL;

  m_pAiScriptRef = NULL;
  m_pAiRecordingRef = NULL;
  m_pScriptTriggers = NULL;
  m_pCutScenes = NULL;
  m_pReferences = NULL;
  m_pSkyBox = NULL;
  m_pDecals = NULL;
  m_pDecalsRef = NULL;
  m_pStartingEquip = NULL;
  m_pScriptCrap = NULL;
  m_pControls = NULL;
  m_pControlsRef = NULL;
  m_pLightFixtures = NULL;
  m_pLightFixturesRef = NULL;


  m_bMultiplayer = FALSE;
  m_ScenarioHeaderOffset = 0xFFFFFFFF;

  for(int i=0; i<SELECTION_MODE_MAX; i++)
  {
    m_TagInfo[i].PcModelPointers = NULL;
    m_TagInfo[i].XboxModelPointers = NULL;
  }

  ZeroMemory(m_MaxPalletteCount, sizeof(UINT)*PALLETTE_TYPES_MAX);

  m_MpFlagTypes.Add("CTF - Flag");
  m_MpFlagTypes.Add("unknown (1)");
  m_MpFlagTypes.Add("Oddball Spawn");
  m_MpFlagTypes.Add("Rally Flag");
  m_MpFlagTypes.Add("unknown (4)");
  m_MpFlagTypes.Add("unknown (5)");
  m_MpFlagTypes.Add("Teleport Origin");
  m_MpFlagTypes.Add("Teleport Destination");
  m_MpFlagTypes.Add("Hill");
}

/*-------------------------------------------------------------------
 * Name: ~CScenario()
 * Description:
 *   
 *-----------------------------------------------------------------*/
CScenario::~CScenario()
{
  Cleanup();

  m_MpFlagTypes.RemoveAll();
}

/*-------------------------------------------------------------------
 * Name: LoadScenario()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CScenario::LoadScenario(CFile *pMapFile, int magic, UINT ScnrOffset, UINT version, BOOL bMultiplayer)
{
  Cleanup();

  m_Magic = magic;
  m_pMapFile = pMapFile;
  m_Version = version;
  m_bMultiplayer = bMultiplayer;
  m_ScenarioHeaderOffset = ScnrOffset;
//26268E0 - offset to teleporter coordinate

  pMapFile->Seek(ScnrOffset, 0);
  pMapFile->Read(&m_Header, sizeof(m_Header));

  memcpy(&m_EditedHeader, &m_Header, sizeof(m_Header));
  memcpy(&m_OriginalHeader, &m_Header, sizeof(m_Header));

  //pMapFile->Seek(ScnrOffset, 0);
  //pMapFile->Write(&m_Header, sizeof(m_Header));

  m_Header.SkyBox.Offset -= magic;
  m_Header.ObjectNames.Offset -= magic;
  m_Header.Scenery.Offset -= magic;
  m_Header.SceneryRef.Offset -= magic;
  m_Header.Biped.Offset -= magic;
  m_Header.BipedRef.Offset -= magic;
  m_Header.Vehicle.Offset -= magic;
  m_Header.VehicleRef.Offset -= magic;
  m_Header.Equip.Offset -= magic;
  m_Header.EquipRef.Offset -= magic;
  m_Header.Weap.Offset -= magic;
  m_Header.WeapRef.Offset -= magic;
  m_Header.DeviceGroups.Offset -= magic;
  m_Header.Machine.Offset -= magic;
  m_Header.MachineRef.Offset -= magic;
  m_Header.Control.Offset -= magic;
  m_Header.ControlRef.Offset -= magic;
  m_Header.LightFixture.Offset -= magic;
  m_Header.LightFixtureRef.Offset -= magic;
  m_Header.SoundScenery.Offset -= magic;
  m_Header.SoundSceneryRef.Offset -= magic;

  m_Header.PlayerStartingProfile.Offset -= magic;
  m_Header.PlayerSpawn.Offset -= magic;
  m_Header.TriggerVolumes.Offset -= magic;
  m_Header.Animations.Offset -= magic;
  m_Header.MultiplayerFlags.Offset -= magic;
  m_Header.MpEquip.Offset -= magic;
  m_Header.StartingEquip.Offset -= magic;

  m_Header.BspSwitchTrigger.Offset -= magic;
  m_Header.Decals.Offset -= magic;
  m_Header.DecalsRef.Offset -= magic;
  m_Header.DetailObjCollRef.Offset -= magic;

  m_Header.ActorVariantRef.Offset -= magic;
  m_Header.Encounters.Offset -= magic;
  m_Header.StartingLocations.Offset -= magic;
  m_Header.Platoons.Offset -= magic;
  m_Header.PlayerStartingProfile.Offset -= magic;
  m_Header.CommandLists.Offset -= magic;
  m_Header.Commands.Offset -= magic;
  m_Header.Points.Offset -= magic;
  
  m_Header.AiAnimationRefs.Offset -= magic;
  m_Header.GlobalsVerified.Offset -= magic;
  m_Header.AiRecordingRefs.Offset -= magic;
  m_Header.AiConversations.Offset -= magic;
  m_Header.Participants.Offset -= magic;
  m_Header.Lines.Offset -= magic;
  m_Header.ScriptTriggers.Offset -= magic;
  m_Header.VerifyCutscenes.Offset -= magic;
  m_Header.VerifyCutsceneTitle.Offset -= magic;
  m_Header.SourceFiles.Offset -= magic;
  m_Header.CutsceneFlags.Offset -= magic;
  m_Header.CutsceneCameraPoi.Offset -= magic;
  m_Header.CutsceneTitles.Offset -= magic;
  m_Header.StructBsp.Offset -= magic;
  m_Header.ScriptCrap.Offset -= magic;

  //AnalyzeFileSection(m_pMapFile, ScnrOffset, m_Header.StructBsp.Offset - ScnrOffset, m_Magic);

  if(m_bMultiplayer)
    MapSlackSpace();

  CString str;
  str = CheckForReference(m_pMapFile, m_Header.unneeded1[4], m_Magic);
  
  gAI.Initialize(m_pMapFile, m_Magic, version);
  gAI.LoadEncounters(m_Header.Encounters);

  LoadLightFixtures();
  LoadControls();
  LoadScriptCrap();
  LoadDecals();
  LoadStartingEquip();
  LoadSkybox();
  LoadBipeds();
  LoadVehicles();
  LoadScenery();
  LoadWeapons();
  LoadEquipment();
  LoadMachines();
  LoadSoundScenery();

  LoadAI();

  LoadPlayerSpawns();
  LoadMultiplayer();
  LoadBspTriggers();
  LoadStartingProfiles();
  LoadDeviceGroups();
  //LoadMovePositions();
  LoadObjectNames();
  LoadTriggerVolumes();
  LoadBspInfo(version);
}

/*-------------------------------------------------------------------
 * Name: LoadVehicles()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CScenario::LoadVehicles()
{
  CString temp;
  CPcModel *pPcModel = NULL;

  m_TagInfo[VEHICLES].GroupFilePointer = m_Header.Vehicle.Offset;
  m_TagInfo[VEHICLES].MaxEditCount = GetMaxEditCount(VEHICLES, m_Header.Vehicle.Count);

  if(m_Header.Vehicle.Count > 0)
  {
    m_pVehicles = new VEHICLE_SPAWN[m_TagInfo[VEHICLES].MaxEditCount];
    m_pMapFile->Seek(m_Header.Vehicle.Offset, 0);
    m_pMapFile->Read(m_pVehicles, sizeof(VEHICLE_SPAWN)*m_Header.Vehicle.Count);
  }

  if(m_Header.VehicleRef.Count > 0)
  {
    m_MaxPalletteCount[VEHICLE_REF] = 100;

    m_pVehicleRef = new PALLETTE[m_MaxPalletteCount[VEHICLE_REF]];
    m_TagInfo[VEHICLES].PcModelPointers = new CPcModel*[m_MaxPalletteCount[VEHICLE_REF]];
    m_TagInfo[VEHICLES].XboxModelPointers = new CXboxModel*[m_MaxPalletteCount[VEHICLE_REF]];

    m_pMapFile->Seek(m_Header.VehicleRef.Offset, 0);
    m_pMapFile->Read(m_pVehicleRef, sizeof(PALLETTE)*m_Header.VehicleRef.Count);

    for(int i=0; i<m_Header.VehicleRef.Count; i++)
    {
      m_TagInfo[VEHICLES].PcModelPointers[i] = 0;
      m_TagInfo[VEHICLES].XboxModelPointers[i] = 0;

      gTagManager.GetTagPcModel(m_pVehicleRef[i].ObjectTagId,
                                &m_TagInfo[VEHICLES].PcModelPointers[i]);

      gTagManager.GetTagXboxModel(m_pVehicleRef[i].ObjectTagId,
                                &m_TagInfo[VEHICLES].XboxModelPointers[i]);

    }
  }
}

/*-------------------------------------------------------------------
 * Name: Cleanup()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CScenario::Cleanup()
{
  gAI.Cleanup();

  if(m_pMovePositions)
    delete [] m_pMovePositions;
  m_pMovePositions = NULL;

  if(m_pStartingProfile)
    delete [] m_pStartingProfile;
  m_pStartingProfile = NULL;

  if(m_pBspTriggers)
    delete [] m_pBspTriggers;
  m_pBspTriggers = NULL;

  if(m_pDeviceGroups)
    delete [] m_pDeviceGroups;
  m_pDeviceGroups = NULL;

  if(m_pObjectNames)
    delete [] m_pObjectNames;
  m_pObjectNames = NULL;

  if(m_pTriggerVolumes)
    delete [] m_pTriggerVolumes;
  m_pTriggerVolumes = NULL;

  if(m_pVehicles)
    delete [] m_pVehicles;
  m_pVehicles = NULL;

  if(m_pScenery)
    delete [] m_pScenery;
  m_pScenery = NULL;

  if(m_pSceneryRef)
    delete [] m_pSceneryRef;
  m_pSceneryRef = NULL;

  if(m_pPlayerSpawns)
    delete [] m_pPlayerSpawns;
  m_pPlayerSpawns = NULL;

  if(m_pMpFlags)
    delete [] m_pMpFlags;
  m_pMpFlags = NULL;

  if(m_pMpEquip)
    delete [] m_pMpEquip;
  m_pMpEquip = NULL;

  if(m_pVehicleRef)
    delete [] m_pVehicleRef;
  m_pVehicleRef = NULL;

  if(m_pWeapons)
    delete [] m_pWeapons;
  m_pWeapons = NULL;

  if(m_pBiped)
    delete [] m_pBiped;
  m_pBiped = NULL;

  if(m_pBipedRef)
    delete [] m_pBipedRef;
  m_pBipedRef = NULL;

  if(m_pEquip)
    delete [] m_pEquip;
  m_pEquip = NULL;

  if(m_pEquipRef)
    delete [] m_pEquipRef;
  m_pEquipRef = NULL;

  if(m_pWeaponRef)
    delete [] m_pWeaponRef;
  m_pWeaponRef = NULL;

  if(m_pMachines)
    delete [] m_pMachines;
  m_pMachines = NULL;

  if(m_pMachinesRef)
    delete [] m_pMachinesRef;
  m_pMachinesRef = NULL;

  if(m_pSounds)
    delete [] m_pSounds;
  m_pSounds = NULL;

  if(m_pSoundRef)
    delete [] m_pSoundRef;
  m_pSoundRef = NULL;

  if(m_pActorVarRef)
    delete [] m_pActorVarRef;
  m_pActorVarRef = NULL;
 
  if(m_pAiAnimationRef)
    delete [] m_pAiAnimationRef;
  m_pAiAnimationRef = NULL;
 
  if(m_pAiScriptRef)
    delete [] m_pAiScriptRef;
  m_pAiScriptRef = NULL;
 
  if(m_pAiRecordingRef)
    delete [] m_pAiRecordingRef;
  m_pAiRecordingRef = NULL;
 
  if(m_pScriptTriggers)
    delete [] m_pScriptTriggers;
  m_pScriptTriggers = NULL;
 
  if(m_pCutScenes)
    delete [] m_pCutScenes;
  m_pCutScenes = NULL;
 
  if(m_pReferences)
    delete [] m_pReferences;
  m_pReferences = NULL;

  if(m_pSkyBox)
    delete [] m_pSkyBox;
  m_pSkyBox = NULL;

  if(m_pDecals)
    delete [] m_pDecals;
  m_pDecals = NULL;

  if(m_pDecalsRef)
    delete [] m_pDecalsRef;
  m_pDecalsRef = NULL;

  if(m_pStartingEquip)
    delete [] m_pStartingEquip;
  m_pStartingEquip = NULL;

  if(m_pScriptCrap)
    delete [] m_pScriptCrap;
  m_pScriptCrap = NULL;

  if(m_pControls)
    delete [] m_pControls;
  m_pControls = NULL;

  if(m_pControlsRef)
    delete [] m_pControlsRef;
  m_pControlsRef = NULL;

  if(m_pLightFixtures)
    delete [] m_pLightFixtures;
  m_pLightFixtures = NULL;

  if(m_pLightFixturesRef)
    delete [] m_pLightFixturesRef;
  m_pLightFixturesRef = NULL;


  for(int i=0; i<SELECTION_MODE_MAX; i++)
  {
    if(m_TagInfo[i].PcModelPointers)
      delete [] m_TagInfo[i].PcModelPointers;
    m_TagInfo[i].PcModelPointers = NULL;
    
    if(m_TagInfo[i].XboxModelPointers)
      delete [] m_TagInfo[i].XboxModelPointers;
    m_TagInfo[i].XboxModelPointers = NULL;
  }

  m_Magic = 0;
  m_pMapFile = NULL;
  m_Version = 0;

  m_SoundNames.RemoveAll();

  ZeroMemory(&m_TagInfo, sizeof(m_TagInfo));
}

/*-------------------------------------------------------------------
 * Name: LoadScenery()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CScenario::LoadScenery()
{
  CString temp;
  int i;
  CPcModel *pPcModel = NULL;

  m_TagInfo[SCENERY].GroupFilePointer = m_Header.Scenery.Offset;
  m_TagInfo[SCENERY].MaxEditCount = GetMaxEditCount(SCENERY, m_Header.Scenery.Count);

  if(m_Header.Scenery.Count > 0)
  {
    m_pScenery = new SCENERY_SPAWN[m_TagInfo[SCENERY].MaxEditCount];
    m_pMapFile->Seek(m_Header.Scenery.Offset, 0);
    m_pMapFile->Read(m_pScenery, sizeof(SCENERY_SPAWN)*m_Header.Scenery.Count);
  }

  if(m_Header.SceneryRef.Count > 0)
  {
    m_MaxPalletteCount[SCENERY_REF] = 500;

    m_pSceneryRef = new PALLETTE[m_MaxPalletteCount[SCENERY_REF]];

    m_TagInfo[SCENERY].PcModelPointers = new CPcModel*[m_MaxPalletteCount[SCENERY_REF]];
    m_TagInfo[SCENERY].XboxModelPointers = new CXboxModel*[m_MaxPalletteCount[SCENERY_REF]];

    m_pMapFile->Seek(m_Header.SceneryRef.Offset, 0);
    m_pMapFile->Read(m_pSceneryRef, sizeof(PALLETTE)*m_Header.SceneryRef.Count);

    for(i=0; i<m_Header.SceneryRef.Count; i++)
    {
      m_TagInfo[SCENERY].PcModelPointers[i] = 0;
      m_TagInfo[SCENERY].XboxModelPointers[i] = 0;

      gTagManager.GetTagPcModel(m_pSceneryRef[i].ObjectTagId,
                                &m_TagInfo[SCENERY].PcModelPointers[i]);

      gTagManager.GetTagXboxModel(m_pSceneryRef[i].ObjectTagId,
                                &m_TagInfo[SCENERY].XboxModelPointers[i]);
    }
  }
  //TODO: add edit scenery properties?

}

/*-------------------------------------------------------------------
 * Name: EditSelectedTag()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CScenario::EditSelectedTag(SELECTION_MODE sel_mode, int index, float *move_vector)
{
  switch(sel_mode)
  {
  case VEHICLES:
    TranslateTagCoord(move_vector, &(m_pVehicles[index].x), TRUE);
    break;

  case SCENERY:
    TranslateTagCoord(move_vector, &(m_pScenery[index].x), TRUE);
    break;

  case WEAPONS:
    TranslateTagCoord(move_vector, &(m_pWeapons[index].x), TRUE);
    break;

  case PLAYER_SPAWNS:
    TranslateTagCoord(move_vector, &(m_pPlayerSpawns[index].x), FALSE);
    break;

  case MP_FLAGS:
    TranslateTagCoord(move_vector, &(m_pMpFlags[index].x), FALSE);
    break;

  case MP_EQUIPMENT:
    TranslateTagCoord(move_vector, &(m_pMpEquip[index].x), FALSE);
    break;

  case EQUIPMENT:
    TranslateTagCoord(move_vector, &(m_pEquip[index].x), TRUE);
    break; 

  case MACHINES:
    TranslateTagCoord(move_vector, &(m_pMachines[index].x), TRUE);
    break; 

  case BIPEDS:
    TranslateTagCoord(move_vector, &(m_pBiped[index].x), TRUE);
    break; 
  }
}


/*-------------------------------------------------------------------
 * Name: MoveSelection()
 * Description:
 *   Called by gSelection to determine how many items are in each 
 *   SELECTION_MODE group.
 *-----------------------------------------------------------------*/
int CScenario::GetTagCount(SELECTION_MODE sel_mode)
{
  ASSERT(sel_mode < SELECTION_MODE_MAX);
  int tag_count = 0;

  switch(sel_mode)
  {
  case VEHICLES:
    tag_count = m_Header.Vehicle.Count;
    break;

  case SCENERY:
    tag_count = m_Header.Scenery.Count;
    break;

  case WEAPONS:
    tag_count = m_Header.Weap.Count;
    break;

  case PLAYER_SPAWNS:
    tag_count = m_Header.PlayerSpawn.Count;
    break;

  case MP_FLAGS:
    tag_count = m_Header.MultiplayerFlags.Count;
    break;

  case MP_EQUIPMENT:
    tag_count = m_Header.MpEquip.Count;
    break;

  case EQUIPMENT:
    tag_count = m_Header.Equip.Count;
    break; 

  case MACHINES:
    tag_count = m_Header.Machine.Count;
    break; 

  case BIPEDS:
    tag_count = m_Header.Biped.Count;
    break; 

  case TRIGGERS:
    tag_count = m_Header.TriggerVolumes.Count;
    break; 
  }

  return(tag_count);
}

/*-------------------------------------------------------------------
 * Name: MoveSelection()
 * Description:
 *   Called by gSelection to determine how many items are in each 
 *   SELECTION_MODE group.
 *-----------------------------------------------------------------*/
BOOL CScenario::GetTagCoord(SELECTION_MODE sel_mode, int index, float *pCoord)
{
  //set rotation (changed 3 to 4)
  switch(sel_mode)
  {
  case VEHICLES:
    memcpy(pCoord, &(m_pVehicles[index].x), sizeof(float)*6);
    break;

  case SCENERY:
    memcpy(pCoord, &(m_pScenery[index].x), sizeof(float)*6);
    break;

  case WEAPONS:
    memcpy(pCoord, &(m_pWeapons[index].x), sizeof(float)*6);
    break;

  case PLAYER_SPAWNS:
    memcpy(pCoord, &(m_pPlayerSpawns[index].x), sizeof(float)*4);
    pCoord[4] = 0;
    pCoord[5] = 0;
    break;

  case MP_FLAGS:
    memcpy(pCoord, &(m_pMpFlags[index].x), sizeof(float)*4);
    pCoord[4] = 0;
    pCoord[5] = 0;
    break;

  case MP_EQUIPMENT:
    memcpy(pCoord, &(m_pMpEquip[index].x), sizeof(float)*4);
    pCoord[4] = 0;
    pCoord[5] = 0;
    break;

  case EQUIPMENT:
    memcpy(pCoord, &(m_pEquip[index].x), sizeof(float)*6);
    pCoord[4] = 0;
    pCoord[5] = 0;
    break; 

  case MACHINES:
    memcpy(pCoord, &(m_pMachines[index].x), sizeof(float)*6);
    break; 

  case BIPEDS:
    memcpy(pCoord, &(m_pBiped[index].x), sizeof(float)*6);
    break; 
  }

  return(FALSE);
}

/*-------------------------------------------------------------------
 * Name: TranslateTagCoord()
 * Description:
 *   Changes the existing tag coord by a translation amount.  Direction
 *   depends on the editing mode.
 *-----------------------------------------------------------------*/
void CScenario::TranslateTagCoord(float *move_vector, float *pData, BOOL bFullRot)
{
  // translate x,y,z
  pData[0] += move_vector[0];
  pData[1] += move_vector[1];
  pData[2] += move_vector[2];

  //update rotation
  pData[3] += move_vector[3];

  if(bFullRot)
  {
    pData[4] += move_vector[4];
    pData[5] += move_vector[5];
  }
}

/*-------------------------------------------------------------------
 * Name: SaveChanges()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CScenario::SaveChanges()
{
  
  // Save vehicle spawn edits
  if(m_Header.Vehicle.Count > 0)
  {
    m_pMapFile->Seek(m_TagInfo[VEHICLES].GroupFilePointer, 0);
    m_pMapFile->Write(m_pVehicles, sizeof(VEHICLE_SPAWN)*m_Header.Vehicle.Count);
  }
  
  // Save scenery spawn edits
  if(m_Header.Scenery.Count > 0)
  {
    m_pMapFile->Seek(m_TagInfo[SCENERY].GroupFilePointer, 0);
    m_pMapFile->Write(m_pScenery, sizeof(SCENERY_SPAWN)*m_Header.Scenery.Count);
  }
  
  // Save player spawn edits
  if(m_Header.PlayerSpawn.Count > 0)
  {
    m_pMapFile->Seek(m_TagInfo[PLAYER_SPAWNS].GroupFilePointer, 0);
    m_pMapFile->Write(m_pPlayerSpawns, sizeof(PLAYER_SPAWN)*m_Header.PlayerSpawn.Count);
  }
  
  // Save item MP flag edits
  if(m_Header.MultiplayerFlags.Count > 0)
  {
    m_pMapFile->Seek(m_TagInfo[MP_FLAGS].GroupFilePointer, 0);
    m_pMapFile->Write(m_pMpFlags, sizeof(MP_FLAG)*m_Header.MultiplayerFlags.Count);
  }
  
  // Save item MP Equipment edits
  if(m_Header.MpEquip.Count > 0)
  {
    m_pMapFile->Seek(m_TagInfo[MP_EQUIPMENT].GroupFilePointer, 0);
    m_pMapFile->Write(m_pMpEquip, sizeof(MP_EQUIP)*m_Header.MpEquip.Count);
  }
  
  // Save item SP Equipment edits
  if(m_Header.Equip.Count > 0)
  {
    m_pMapFile->Seek(m_TagInfo[EQUIPMENT].GroupFilePointer, 0);
    m_pMapFile->Write(m_pEquip, sizeof(EQUIP)*m_Header.Equip.Count);
  }
  
  // Save item Machine edits
  if(m_Header.Machine.Count > 0)
  {
    m_pMapFile->Seek(m_TagInfo[MACHINES].GroupFilePointer, 0);
    m_pMapFile->Write(m_pMachines, sizeof(MACHINE)*m_Header.Machine.Count);
  }
  
  // Save item Biped edits
  if(m_Header.Biped.Count > 0)
  {
    m_pMapFile->Seek(m_TagInfo[BIPEDS].GroupFilePointer, 0);
    m_pMapFile->Write(m_pBiped, sizeof(BIPED)*m_Header.Biped.Count);
  }
}

/*-------------------------------------------------------------------
 * Name: LoadWeapons()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CScenario::LoadWeapons()
{
  CString temp;

  m_TagInfo[WEAPONS].GroupFilePointer = m_Header.Weap.Offset;
  m_TagInfo[WEAPONS].MaxEditCount = GetMaxEditCount(WEAPONS, m_Header.Weap.Count);

  if(m_Header.Weap.Count > 0)
  {
    m_pWeapons = new WEAPON_SPAWN[m_TagInfo[WEAPONS].MaxEditCount];
    m_pMapFile->Seek(m_Header.Weap.Offset, 0);
    m_pMapFile->Read(m_pWeapons, sizeof(WEAPON_SPAWN)*m_Header.Weap.Count);
  }

  if(m_Header.WeapRef.Count > 0)
  {
    m_pWeaponRef = new PALLETTE[m_Header.WeapRef.Count];
    m_TagInfo[WEAPONS].PcModelPointers = new CPcModel*[m_Header.WeapRef.Count];
    m_TagInfo[WEAPONS].XboxModelPointers = new CXboxModel*[m_Header.WeapRef.Count];

    m_pMapFile->Seek(m_Header.WeapRef.Offset, 0);
    m_pMapFile->Read(m_pWeaponRef, sizeof(PALLETTE)*m_Header.WeapRef.Count);

    for(int i=0; i<m_Header.WeapRef.Count; i++)
    {
      m_TagInfo[WEAPONS].PcModelPointers[i] = 0;
      m_TagInfo[WEAPONS].XboxModelPointers[i] = 0;

      gTagManager.GetTagPcModel(m_pWeaponRef[i].ObjectTagId,
                                &m_TagInfo[WEAPONS].PcModelPointers[i]);

      gTagManager.GetTagXboxModel(m_pWeaponRef[i].ObjectTagId,
                                &m_TagInfo[WEAPONS].XboxModelPointers[i]);
    }
  }
}

/*-------------------------------------------------------------------
 * Name: LoadPlayerSpawns()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CScenario::LoadPlayerSpawns()
{
  m_TagInfo[PLAYER_SPAWNS].GroupFilePointer = m_Header.PlayerSpawn.Offset;
  m_TagInfo[PLAYER_SPAWNS].MaxEditCount = GetMaxEditCount(PLAYER_SPAWNS, m_Header.PlayerSpawn.Count);

  if(m_Header.PlayerSpawn.Count > 0)
  {
    m_pPlayerSpawns = new PLAYER_SPAWN[m_TagInfo[PLAYER_SPAWNS].MaxEditCount];
    m_pMapFile->Seek(m_Header.PlayerSpawn.Offset, 0);
    m_pMapFile->Read(m_pPlayerSpawns, sizeof(PLAYER_SPAWN)*m_Header.PlayerSpawn.Count);
  }
}

/*-------------------------------------------------------------------
 * Name: LoadMultiplayer()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CScenario::LoadMultiplayer()
{
  CString temp;

  m_TagInfo[MP_FLAGS].GroupFilePointer = m_Header.MultiplayerFlags.Offset;
  m_TagInfo[MP_FLAGS].MaxEditCount = GetMaxEditCount(MP_FLAGS, m_Header.MultiplayerFlags.Count);

  if(m_Header.MultiplayerFlags.Count > 0)
  {
    m_pMpFlags = new MP_FLAG[m_TagInfo[MP_FLAGS].MaxEditCount];
    m_pMapFile->Seek(m_Header.MultiplayerFlags.Offset, 0);
    m_pMapFile->Read(m_pMpFlags, sizeof(MP_FLAG)*m_Header.MultiplayerFlags.Count);
  }

  //MP equipment (weapons, powerups, etc)
  m_TagInfo[MP_EQUIPMENT].GroupFilePointer = m_Header.MpEquip.Offset;
  m_TagInfo[MP_EQUIPMENT].MaxEditCount = GetMaxEditCount(MP_EQUIPMENT, m_Header.MpEquip.Count);

  if(m_Header.MpEquip.Count > 0)
  {
    m_pMpEquip = new MP_EQUIP[m_TagInfo[MP_EQUIPMENT].MaxEditCount];

    if(m_Version == 7)
      m_TagInfo[MP_EQUIPMENT].PcModelPointers = new CPcModel*[m_TagInfo[MP_EQUIPMENT].MaxEditCount];
    else if(m_Version == 5)
      m_TagInfo[MP_EQUIPMENT].XboxModelPointers = new CXboxModel*[m_TagInfo[MP_EQUIPMENT].MaxEditCount];

    m_pMapFile->Seek(m_Header.MpEquip.Offset, 0);
    m_pMapFile->Read(m_pMpEquip, sizeof(MP_EQUIP)*m_Header.MpEquip.Count);

    for(int i=0; i<m_Header.MpEquip.Count; i++)
    {
      if(m_Version == 7)
      {
        m_TagInfo[MP_EQUIPMENT].PcModelPointers[i] = 0;
        gTagManager.GetTagPcModel(m_pMpEquip[i].ObjectTagId,
                                  &m_TagInfo[MP_EQUIPMENT].PcModelPointers[i]);

      }
      else if(m_Version == 5)
      {
        m_TagInfo[MP_EQUIPMENT].XboxModelPointers[i] = 0;
        gTagManager.GetTagXboxModel(m_pMpEquip[i].ObjectTagId,
                                  &m_TagInfo[MP_EQUIPMENT].XboxModelPointers[i]);
      }
    }
  }
}

/*-------------------------------------------------------------------
 * Name: UpdateTools()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CScenario::UpdateTools(SELECTION_MODE sel_mode, int index)
{
  CString temp, str;
  float loc[6];

  if(sel_mode != BSP_MESH_INFO)
  {
    temp.Format("Object %d out of %d objects (0-based index)\r\n", index, GetTagCount(sel_mode));
    str += temp;
    
    GetTagCoord(sel_mode, index, loc);
    temp.Format("Object Location: %.3f, %.3f, %.3f\r\n", loc[0], loc[1], loc[2]);
    str += temp;
    
    str += "------------------------------------------------------\r\n";
    temp.Format("Object Rotation (radians):  Yaw= %.3f Pitch=%.3f Roll= %.3f\r\n", loc[3], loc[4], loc[5]);
    str += temp;
    
    temp.Format("Object Rotation (degrees):  Yaw= %.3f Pitch=%.3f Roll= %.3f\r\n", loc[3]*RAD_TO_DEG, 
      loc[4]*RAD_TO_DEG, loc[5]*RAD_TO_DEG);
    str += temp;
    
    str += "------------------------------------------------------\r\n";
    str += GetModelDescription(sel_mode, index);
  }
  
  if(sel_mode==MP_FLAGS)
  {
    temp.Format("%s  #%d", m_MpFlagTypes[m_pMpFlags[index].index1], m_pMpFlags[index].index2);
    str += temp;
    gInfoDlg.SetObjectInfo(str);
  }
  else if(sel_mode==VEHICLES)
  {
    if(m_pVehicles[index].numid != -1)
    {
      g_pTools->m_pToolSheet->m_EditTab.ActivateSelectionType(sel_mode, index, m_pVehicles[index].numid);
      str += gTagManager.GetTagDescription(m_pVehicleRef[m_pVehicles[index].numid].ObjectTagId);
      gInfoDlg.SetObjectInfo(str);
    }
  }
  else if(sel_mode==WEAPONS)
  {
    if(m_pWeapons[index].numid != -1)
    {
      g_pTools->m_pToolSheet->m_EditTab.ActivateSelectionType(sel_mode, index, m_pWeapons[index].numid);
      str += gTagManager.GetTagDescription(m_pWeaponRef[m_pWeapons[index].numid].ObjectTagId);
      gInfoDlg.SetObjectInfo(str);
    }
  }
  else if(sel_mode == SCENERY)
  {
    if(m_pScenery[index].numid != -1)
    {
      g_pTools->m_pToolSheet->m_EditTab.ActivateSelectionType(sel_mode, index, m_pScenery[index].numid);
      str += gTagManager.GetTagDescription(m_pSceneryRef[m_pScenery[index].numid].ObjectTagId);
      gInfoDlg.SetObjectInfo(str);
    }
  }
  else if(sel_mode == MP_EQUIPMENT)
  {
    str += gTagManager.GetTagDescription(m_pMpEquip[index].ObjectTagId);
    gInfoDlg.SetObjectInfo(str);
  }
  else if(sel_mode == BSP_MESH_INFO)
  {
    str += gBspManager.GetMeshInfo(index);
    gInfoDlg.SetObjectInfo(str);
  }
  else if(sel_mode == EQUIPMENT)
  {
    g_pTools->m_pToolSheet->m_EditTab.ActivateSelectionType(sel_mode, index, m_pEquip[index].numid);
    str += gTagManager.GetTagDescription(m_pEquipRef[m_pEquip[index].numid].ObjectTagId);
    gInfoDlg.SetObjectInfo(str);
  }
  else if(sel_mode==MACHINES)
  {
    if(m_pMachines[index].MachineType != -1)
    {
      g_pTools->m_pToolSheet->m_EditTab.ActivateSelectionType(sel_mode, index, m_pMachines[index].MachineType);
      str += gTagManager.GetTagDescription(m_pMachinesRef[m_pMachines[index].MachineType].ObjectTagId);
      gInfoDlg.SetObjectInfo(str);
    }
  }
  else if(sel_mode==BIPEDS)
  {
    if(m_pBiped[index].BipedType != -1)
    {
      g_pTools->m_pToolSheet->m_EditTab.ActivateSelectionType(sel_mode, index, m_pBiped[index].BipedType);
      str += gTagManager.GetTagDescription(m_pBipedRef[m_pBiped[index].BipedType].ObjectTagId);
      gInfoDlg.SetObjectInfo(str);
    }
  }
  else if(sel_mode==TRIGGERS)
  {
    str = m_pTriggerVolumes[index].name;
    gInfoDlg.SetObjectInfo(str);
  }
  else
  {
    gInfoDlg.SetObjectInfo(str);
  }
}

/*-------------------------------------------------------------------
 * Name: LoadBspInfo()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CScenario::LoadBspInfo(UINT version)
{
  gBspManager.LoadVisibleBspInfo(m_Header.StructBsp, version);
}

/*-------------------------------------------------------------------
 * Name: LoadBspTriggers()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CScenario::LoadBspTriggers()
{
  CString temp;

  if(m_Header.BspSwitchTrigger.Count > 0)
  {
    m_pBspTriggers = new BSP_TRIGGER[m_Header.BspSwitchTrigger.Count];
    m_pMapFile->Seek(m_Header.BspSwitchTrigger.Offset, 0);
    m_pMapFile->Read(m_pBspTriggers, sizeof(BSP_TRIGGER)*m_Header.BspSwitchTrigger.Count);
  }
}

/*-------------------------------------------------------------------
 * Name: LoadStartingProfiles()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CScenario::LoadStartingProfiles()
{
  if(m_Header.PlayerStartingProfile.Count > 0)
  {
//    m_pStartingProfile = new PLAYER_STARTING_PROFILE[m_Header.PlayerStartingProfile.Count];
//    m_pMapFile->Seek(m_Header.PlayerStartingProfile.Offset, 0);
//    m_pMapFile->Read(m_pStartingProfile, sizeof(PLAYER_STARTING_PROFILE)*m_Header.PlayerStartingProfile.Count);
  }
}

/*-------------------------------------------------------------------
 * Name: LoadDeviceGroups()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CScenario::LoadDeviceGroups()
{
  if(m_Header.DeviceGroups.Count > 0)
  {
    m_pDeviceGroups = new DEVICE_GROUP[m_Header.DeviceGroups.Count];
    m_pMapFile->Seek(m_Header.DeviceGroups.Offset, 0);
    m_pMapFile->Read(m_pDeviceGroups, sizeof(DEVICE_GROUP)*m_Header.DeviceGroups.Count);
  }
}

/*-------------------------------------------------------------------
 * Name: LoadMovePositions()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CScenario::LoadMovePositions()
{
}

/*-------------------------------------------------------------------
 * Name: LoadObjectNames()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CScenario::LoadObjectNames()
{
  if(m_Header.ObjectNames.Count > 0)
  {
    m_pObjectNames = new OBJECT_NAMES[m_Header.ObjectNames.Count];
    m_pMapFile->Seek(m_Header.ObjectNames.Offset, 0);
    m_pMapFile->Read(m_pObjectNames, sizeof(OBJECT_NAMES)*m_Header.ObjectNames.Count);
  }
}

/*-------------------------------------------------------------------
 * Name: LoadTriggerVolumes()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CScenario::LoadTriggerVolumes()
{
  m_TagInfo[TRIGGERS].MaxEditCount = GetMaxEditCount(TRIGGERS, m_Header.TriggerVolumes.Count);

  if(m_Header.TriggerVolumes.Count > 0)
  {
    m_pTriggerVolumes = new TRIGGER_VOLUMES[m_Header.TriggerVolumes.Count];
    m_pMapFile->Seek(m_Header.TriggerVolumes.Offset, 0);
    m_pMapFile->Read(m_pTriggerVolumes, sizeof(TRIGGER_VOLUMES)*m_Header.TriggerVolumes.Count);
  }

  //Trigger Volumes are a special case
  m_TagInfo[TRIGGERS].GroupFilePointer = m_Header.TriggerVolumes.Offset;
}

/*-------------------------------------------------------------------
 * Name: LoadBipeds()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CScenario::LoadBipeds()
{
  int i;
  CString temp;

  m_TagInfo[BIPEDS].GroupFilePointer = m_Header.Biped.Offset;
  m_TagInfo[BIPEDS].MaxEditCount = GetMaxEditCount(BIPEDS, m_Header.Biped.Count);

  if(m_Header.Biped.Count > 0)
  {
    m_pBiped = new BIPED[m_TagInfo[BIPEDS].MaxEditCount];
    m_pMapFile->Seek(m_Header.Biped.Offset, 0);
    m_pMapFile->Read(m_pBiped, sizeof(BIPED)*m_Header.Biped.Count);
  }

  if(m_Header.BipedRef.Count > 0)
  {
    m_pBipedRef = new PALLETTE[m_Header.BipedRef.Count];
    m_TagInfo[BIPEDS].PcModelPointers = new CPcModel*[m_Header.BipedRef.Count];
    m_TagInfo[BIPEDS].XboxModelPointers = new CXboxModel*[m_Header.BipedRef.Count];

    m_pMapFile->Seek(m_Header.BipedRef.Offset, 0);
    m_pMapFile->Read(m_pBipedRef, sizeof(PALLETTE)*m_Header.BipedRef.Count);

    for(i=0; i<m_Header.BipedRef.Count; i++)
    {
      m_TagInfo[BIPEDS].PcModelPointers[i] = 0;
      m_TagInfo[BIPEDS].XboxModelPointers[i] = 0;

      gTagManager.GetTagPcModel(m_pBipedRef[i].ObjectTagId,
                                &m_TagInfo[BIPEDS].PcModelPointers[i]);

      gTagManager.GetTagXboxModel(m_pBipedRef[i].ObjectTagId,
                                &m_TagInfo[BIPEDS].XboxModelPointers[i]);
    }
  }
}

/*-------------------------------------------------------------------
 * Name: LoadEquipment()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CScenario::LoadEquipment()
{
  int i;
  CString temp;

  m_TagInfo[EQUIPMENT].GroupFilePointer = m_Header.Equip.Offset;
  m_TagInfo[EQUIPMENT].MaxEditCount = GetMaxEditCount(EQUIPMENT, m_Header.Equip.Count);

  if(m_Header.Equip.Count > 0)
  {
    m_pEquip = new EQUIP[m_TagInfo[EQUIPMENT].MaxEditCount];
    m_pMapFile->Seek(m_Header.Equip.Offset, 0);
    m_pMapFile->Read(m_pEquip, sizeof(EQUIP)*m_Header.Equip.Count);
  }

  if(m_Header.EquipRef.Count > 0)
  {
    m_pEquipRef = new PALLETTE[m_Header.EquipRef.Count];
    m_TagInfo[EQUIPMENT].PcModelPointers = new CPcModel*[m_Header.EquipRef.Count];
    m_TagInfo[EQUIPMENT].XboxModelPointers = new CXboxModel*[m_Header.EquipRef.Count];

    m_pMapFile->Seek(m_Header.EquipRef.Offset, 0);
    m_pMapFile->Read(m_pEquipRef, sizeof(PALLETTE)*m_Header.EquipRef.Count);

    for(i=0; i<m_Header.EquipRef.Count; i++)
    {
      m_TagInfo[EQUIPMENT].PcModelPointers[i] = 0;
      m_TagInfo[EQUIPMENT].XboxModelPointers[i] = 0;

      gTagManager.GetTagPcModel(m_pEquipRef[i].ObjectTagId,
                                &m_TagInfo[EQUIPMENT].PcModelPointers[i]);

      gTagManager.GetTagXboxModel(m_pEquipRef[i].ObjectTagId,
                                &m_TagInfo[EQUIPMENT].XboxModelPointers[i]);
    }
  }
}

/*-------------------------------------------------------------------
 * Name: LoadMachines()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CScenario::LoadMachines()
{
  int i;
  CString temp;

  m_TagInfo[MACHINES].GroupFilePointer = m_Header.Machine.Offset;
  m_TagInfo[MACHINES].MaxEditCount = GetMaxEditCount(MACHINES, m_Header.Machine.Count);

  if(m_Header.Machine.Count > 0)
  {
    m_pMachines = new MACHINE[m_TagInfo[MACHINES].MaxEditCount];
    m_pMapFile->Seek(m_Header.Machine.Offset, 0);
    m_pMapFile->Read(m_pMachines, sizeof(MACHINE)*m_Header.Machine.Count);
  }

  if(m_Header.MachineRef.Count > 0)
  {
    m_pMachinesRef = new PALLETTE[m_Header.MachineRef.Count];
    m_TagInfo[MACHINES].PcModelPointers = new CPcModel*[m_Header.MachineRef.Count];
    m_TagInfo[MACHINES].XboxModelPointers = new CXboxModel*[m_Header.MachineRef.Count];

    m_pMapFile->Seek(m_Header.MachineRef.Offset, 0);
    m_pMapFile->Read(m_pMachinesRef, sizeof(PALLETTE)*m_Header.MachineRef.Count);

    for(i=0; i<m_Header.MachineRef.Count; i++)
    {
      m_TagInfo[MACHINES].PcModelPointers[i] = 0;
      m_TagInfo[MACHINES].XboxModelPointers[i] = 0;

      gTagManager.GetTagPcModel(m_pMachinesRef[i].ObjectTagId,
                                &m_TagInfo[MACHINES].PcModelPointers[i]);

      gTagManager.GetTagXboxModel(m_pMachinesRef[i].ObjectTagId,
                                &m_TagInfo[MACHINES].XboxModelPointers[i]);
    }
  }
}

/*-------------------------------------------------------------------
 * Name: LoadSoundScenery()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CScenario::LoadSoundScenery()
{
  int i;
  CString temp;

  if(m_Header.SoundScenery.Count > 0)
  {
    m_pSounds = new SOUND_SCENERY[m_Header.SoundScenery.Count];
    m_pMapFile->Seek(m_Header.SoundScenery.Offset, 0);
    m_pMapFile->Read(m_pSounds, sizeof(SOUND_SCENERY)*m_Header.SoundScenery.Count);

    //for(i =0; i<m_Header.Machine.Count; i++)
    //  TRACE("%6d %6d\n", m_pMachines[i].MachineType, m_pMachines[i].unk2);
  }

  if(m_Header.SoundSceneryRef.Count > 0)
  {
    m_pSoundRef = new PALLETTE[m_Header.SoundSceneryRef.Count];
    m_pMapFile->Seek(m_Header.SoundSceneryRef.Offset, 0);
    m_pMapFile->Read(m_pSoundRef, sizeof(PALLETTE)*m_Header.SoundSceneryRef.Count);

    for(i=0; i<m_Header.SoundSceneryRef.Count; i++)
    {
      temp = CheckForReference(m_pMapFile, m_pSoundRef[i].NamePtr, m_Magic);
      m_SoundNames.Add(temp);
    }
  }
}

/*-------------------------------------------------------------------
 * Name: LoadAI()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CScenario::LoadAI()
{
  int i;
  CString temp;

  if(0)//m_Header.ActorVariantRef.Count > 0)
  {
    m_pActorVarRef = new ACTOR_VARIANT_REF[m_Header.ActorVariantRef.Count];
    m_pMapFile->Seek(m_Header.ActorVariantRef.Offset, 0);
    m_pMapFile->Read(m_pActorVarRef, sizeof(ACTOR_VARIANT_REF)*m_Header.ActorVariantRef.Count);

    for(i=0; i<m_Header.ActorVariantRef.Count; i++)
    {
      temp = CheckForReference(m_pMapFile, m_pActorVarRef[i].NamePtr, m_Magic);
      //m_SoundNames.Add(temp);
    }
  }

  if(0)//m_Header.AiAnimationRefs.Count > 0)
  {
    m_pAiAnimationRef = new AI_ANIMATION_REF[m_Header.AiAnimationRefs.Count];
    m_pMapFile->Seek(m_Header.AiAnimationRefs.Offset, 0);
    m_pMapFile->Read(m_pAiAnimationRef, sizeof(AI_ANIMATION_REF)*m_Header.AiAnimationRefs.Count);

    for(i=0; i<m_Header.ActorVariantRef.Count; i++)
    {
      //temp = CheckForReference(m_pMapFile, m_pAiAnimationRef[i].NamePtr, m_Magic);
      //m_SoundNames.Add(temp);
    }
  }

  if(0)//m_Header.GlobalsVerified.Count > 0)
  {
    m_pAiScriptRef = new AI_SCRIPT_REF[m_Header.GlobalsVerified.Count];
    m_pMapFile->Seek(m_Header.GlobalsVerified.Offset, 0);
    m_pMapFile->Read(m_pAiScriptRef, sizeof(AI_SCRIPT_REF)*m_Header.GlobalsVerified.Count);
  }

  if(0)//m_Header.AiRecordingRefs.Count > 0)
  {
    m_pAiRecordingRef = new AI_RECORDING_REF[m_Header.AiRecordingRefs.Count];
    m_pMapFile->Seek(m_Header.AiRecordingRefs.Offset, 0);
    m_pMapFile->Read(m_pAiRecordingRef, sizeof(AI_RECORDING_REF)*m_Header.AiRecordingRefs.Count);

    for(i=0; i<m_Header.AiRecordingRefs.Count; i++)
    {
      temp = CheckForReference(m_pMapFile, m_pAiRecordingRef[i].NamePtr, m_Magic);
      //m_SoundNames.Add(temp);
    }
  }

  if(0)//m_Header.ScriptTriggers.Count > 0)
  {
    m_pScriptTriggers = new SCRIPT_TRIGGER[m_Header.ScriptTriggers.Count];
    m_pMapFile->Seek(m_Header.ScriptTriggers.Offset, 0);
    m_pMapFile->Read(m_pScriptTriggers, sizeof(SCRIPT_TRIGGER)*m_Header.ScriptTriggers.Count);

    for(i=0; i<m_Header.ScriptTriggers.Count; i++)
    {
      //temp = CheckForReference(m_pMapFile, m_pAiRecordingRef[i].NamePtr, m_Magic);
      //m_SoundNames.Add(temp);
    }
  }

  if(0)//m_Header.VerifyCutscenes.Count > 0)
  {
    m_pCutScenes = new GLOBALS[m_Header.VerifyCutscenes.Count];
    m_pMapFile->Seek(m_Header.VerifyCutscenes.Offset, 0);
    m_pMapFile->Read(m_pCutScenes, sizeof(GLOBALS)*m_Header.VerifyCutscenes.Count);

    for(i=0; i<m_Header.VerifyCutscenes.Count; i++)
    {
      //temp = CheckForReference(m_pMapFile, m_pGlobals[i].NamePtr, m_Magic);
      //m_SoundNames.Add(temp);
    }
  }

  if(0)//m_Header.VerifyCutsceneTitle.Count > 0)
  {
    m_pReferences = new REFERENCES[m_Header.VerifyCutsceneTitle.Count];
    m_pMapFile->Seek(m_Header.VerifyCutsceneTitle.Offset, 0);
    m_pMapFile->Read(m_pReferences, sizeof(REFERENCES)*m_Header.VerifyCutsceneTitle.Count);

    for(i=0; i<m_Header.VerifyCutsceneTitle.Count; i++)
    {
      //temp = CheckForReference(m_pMapFile, m_pReferences[i].NamePtr, m_Magic);
      //m_SoundNames.Add(temp);
    }
  }
}

void CScenario::GetTagModel(SELECTION_MODE sel_mode, int index, CPcModel **pPcModel, CXboxModel **pXboxModel)
{
  *pPcModel = NULL;
  *pXboxModel = NULL;

  if(m_Version == 0x261)
  {
    switch(sel_mode)
    {
    case VEHICLES:
      if((m_pVehicles[index].numid >= 0)&&(m_pVehicles[index].numid < m_Header.VehicleRef.Count))
        *pPcModel = m_TagInfo[sel_mode].PcModelPointers[m_pVehicles[index].numid];
      break;
      
    case SCENERY:
      if((m_pScenery[index].numid >= 0)&&(m_pScenery[index].numid < m_Header.SceneryRef.Count))
        *pPcModel = m_TagInfo[sel_mode].PcModelPointers[m_pScenery[index].numid];
      break;
      
    case WEAPONS:
      if((m_pWeapons[index].numid  >= 0)&&(m_pWeapons[index].numid < m_Header.WeapRef.Count))
        *pPcModel = m_TagInfo[sel_mode].PcModelPointers[m_pWeapons[index].numid];
      break;
      
    case PLAYER_SPAWNS:
		//*pPcModel = m_TagInfo[sel_mode].PcModelPointers[m_pBiped[index].BipedType];
      break;
      
    case MP_FLAGS:
      break;
      
    case MP_EQUIPMENT:
      //*pPcModel = m_TagInfo[sel_mode].PcModelPointers[index];
      break;

    case EQUIPMENT:
      if(m_pEquip[index].numid != -1)
        *pPcModel = m_TagInfo[sel_mode].PcModelPointers[m_pEquip[index].numid];
      break; 

    case MACHINES:
      if(m_pMachines[index].MachineType != -1)
        *pPcModel = m_TagInfo[sel_mode].PcModelPointers[m_pMachines[index].MachineType];
      break; 

    case BIPEDS:
      if(m_pBiped[index].BipedType != -1)
        *pPcModel = m_TagInfo[sel_mode].PcModelPointers[m_pBiped[index].BipedType];
      break; 
    }
  }
  else if(m_Version == 7)
  {
    switch(sel_mode)
    {
    case VEHICLES:
      if((m_pVehicles[index].numid >= 0)&&(m_pVehicles[index].numid < m_Header.VehicleRef.Count))
        *pPcModel = m_TagInfo[sel_mode].PcModelPointers[m_pVehicles[index].numid];
      break;
      
    case SCENERY:
      if((m_pScenery[index].numid >= 0)&&(m_pScenery[index].numid < m_Header.SceneryRef.Count))
        *pPcModel = m_TagInfo[sel_mode].PcModelPointers[m_pScenery[index].numid];
      break;
      
    case WEAPONS:
      if((m_pWeapons[index].numid  >= 0)&&(m_pWeapons[index].numid < m_Header.WeapRef.Count))
        *pPcModel = m_TagInfo[sel_mode].PcModelPointers[m_pWeapons[index].numid];
      break;
      
    case PLAYER_SPAWNS:
      break;
      
    case MP_FLAGS:
      break;
      
    case MP_EQUIPMENT:
      *pPcModel = m_TagInfo[sel_mode].PcModelPointers[index];
      break;

    case EQUIPMENT:
      if(m_pEquip[index].numid != -1)
        *pPcModel = m_TagInfo[sel_mode].PcModelPointers[m_pEquip[index].numid];
      break; 

    case MACHINES:
      if(m_pMachines[index].MachineType != -1)
        *pPcModel = m_TagInfo[sel_mode].PcModelPointers[m_pMachines[index].MachineType];
      break; 

    case BIPEDS:
      if(m_pBiped[index].BipedType != -1)
        *pPcModel = m_TagInfo[sel_mode].PcModelPointers[m_pBiped[index].BipedType];
      break; 
    }
  }
  else if(m_Version == 5)
  {
    switch(sel_mode)
    {
    case VEHICLES:
      if(m_pVehicles[index].numid != -1)
        *pXboxModel = m_TagInfo[sel_mode].XboxModelPointers[m_pVehicles[index].numid];
      break;
      
    case SCENERY:
      if(m_pScenery[index].numid != -1)
        *pXboxModel = m_TagInfo[sel_mode].XboxModelPointers[m_pScenery[index].numid];
      break;
      
    case WEAPONS:
      if(m_pWeapons[index].numid != -1)
        *pXboxModel = m_TagInfo[sel_mode].XboxModelPointers[m_pWeapons[index].numid];
      break;
      
    case PLAYER_SPAWNS:
      break;
      
    case MP_FLAGS:
      break;
      
    case MP_EQUIPMENT:
      *pXboxModel = m_TagInfo[sel_mode].XboxModelPointers[index];
      break;

    case EQUIPMENT:
      if(m_pEquip[index].numid != -1)
        *pXboxModel = m_TagInfo[sel_mode].XboxModelPointers[m_pEquip[index].numid];
      break; 

    case MACHINES:
      if(m_pMachines[index].MachineType != -1)
        *pXboxModel = m_TagInfo[sel_mode].XboxModelPointers[m_pMachines[index].MachineType];
      break; 

    case BIPEDS:
      if(m_pBiped[index].BipedType != -1)
        *pXboxModel = m_TagInfo[sel_mode].XboxModelPointers[m_pBiped[index].BipedType];
      break; 
    }
  }
}

CString CScenario::GetModelDescription(SELECTION_MODE sel_mode, int index)
{
  CString temp, desc;
  CPcModel *pPcModel = NULL;
  CXboxModel *pXboxModel = NULL;

  GetTagModel(sel_mode, index, &pPcModel, &pXboxModel);

  if(pPcModel)
  {
    desc = pPcModel->GetDescription();
  }
  else if(pXboxModel)
  {
    desc = pXboxModel->GetDescription();
  }

  return(desc);
}

void CScenario::GetTriggerVolume(int index, BOUNDING_BOX *pBox)
{
  memcpy(pBox, &m_pTriggerVolumes[index].box, sizeof(BOUNDING_BOX));
}

int CScenario::GetPalletteCount(SELECTION_MODE sel_mode)
{
  int count = 0;

  switch(sel_mode)
  {
    case VEHICLES:
      count = m_Header.VehicleRef.Count;
      break;
      
    case SCENERY:
      count = m_Header.SceneryRef.Count;
      break;
      
    case WEAPONS:
      count = m_Header.WeapRef.Count;
      break;
      
    case MP_EQUIPMENT:
      //special case
      break;
      
    case EQUIPMENT:
      count = m_Header.EquipRef.Count;
      break;
      
    case MACHINES:
      count = m_Header.MachineRef.Count;
      break;
      
    case BIPEDS:
      count = m_Header.BipedRef.Count;
      break;
  }

  return(count);
}

CString CScenario::GetPalletteString(SELECTION_MODE sel_mode, int index)
{
  CString str;

  switch(sel_mode)
  {
    case VEHICLES:
      if(index < m_Header.VehicleRef.Count)
      {
        str = CheckForReference(m_pMapFile, m_pVehicleRef[index].NamePtr, m_Magic);
      }
      break;
      
    case SCENERY:
      if(index < m_Header.SceneryRef.Count)
      {
        str = CheckForReference(m_pMapFile, m_pSceneryRef[index].NamePtr, m_Magic);
      }
      break;
      
    case WEAPONS:
      if(index < m_Header.WeapRef.Count)
      {
        str = CheckForReference(m_pMapFile, m_pWeaponRef[index].NamePtr, m_Magic);
      }
      break;
      
    case MP_EQUIPMENT:
      //special case
      break;
      
    case EQUIPMENT:
      if(index < m_Header.EquipRef.Count)
      {
        str = CheckForReference(m_pMapFile, m_pEquipRef[index].NamePtr, m_Magic);
      }
      break;
      
    case MACHINES:
      if(index < m_Header.MachineRef.Count)
      {
        str = CheckForReference(m_pMapFile, m_pMachinesRef[index].NamePtr, m_Magic);
      }
      break;
      
    case BIPEDS:
      if(index < m_Header.BipedRef.Count)
      {
        str = CheckForReference(m_pMapFile, m_pBipedRef[index].NamePtr, m_Magic);
      }
      break;
  }

  return(str);
}

void CScenario::SetObjectType(SELECTION_MODE sel_mode, int object_index, int new_type)
{

  switch(sel_mode)
  {
    case VEHICLES:
      if(new_type < m_Header.VehicleRef.Count)
      {
        m_pVehicles[object_index].numid = new_type;
      }
      break;
      
    case SCENERY:
      if(new_type < m_Header.SceneryRef.Count)
      {
        m_pScenery[object_index].numid = new_type;
      }
      break;
      
    case WEAPONS:
      if(new_type < m_Header.WeapRef.Count)
      {
        m_pWeapons[object_index].numid = new_type;
      }
      break;
      
    case MP_EQUIPMENT:
      //special case
      break;
      
    case EQUIPMENT:
      if(new_type < m_Header.EquipRef.Count)
      {
        m_pEquip[object_index].numid = new_type;
      }
      break;
      
    case MACHINES:
      if(new_type < m_Header.MachineRef.Count)
      {
        m_pMachines[object_index].MachineType = new_type;
      }
      break;
      
    case BIPEDS:
      if(new_type < m_Header.BipedRef.Count)
      {
        m_pBiped[object_index].BipedType = new_type;
      }
      break;
  }
}

void CScenario::MapSlackSpace()
{
  UINT start, end, delta;

  if(m_Header.Scenery.Count > 0)
    start = m_Header.Scenery.Offset;

  if(m_Header.SceneryRef.Count > 0)
    start = m_Header.SceneryRef.Offset;

  if(m_Header.Vehicle.Count > 0)
    start = m_Header.Vehicle.Offset;

  if(m_Header.VehicleRef.Count > 0)
    start = m_Header.VehicleRef.Offset;

  if(m_Header.SoundScenery.Count > 0)
    start = m_Header.SoundScenery.Offset;

  if(m_Header.SoundSceneryRef.Count > 0)
    start = m_Header.SoundSceneryRef.Offset;


  if(m_Header.PlayerStartingProfile.Count > 0)
    start = m_Header.PlayerStartingProfile.Offset;

  if(m_Header.PlayerSpawn.Count > 0)
    start = m_Header.PlayerSpawn.Offset;

  if(m_Header.MultiplayerFlags.Count > 0)
    start = m_Header.MultiplayerFlags.Offset;

  if(m_Header.MpEquip.Count > 0)
    start = m_Header.MpEquip.Offset;

  if(m_Header.StartingEquip.Count > 0)
    start = m_Header.StartingEquip.Offset;


  if(m_Header.Decals.Count > 0)
    start = m_Header.Decals.Offset;

  if(m_Header.DecalsRef.Count > 0)
    start = m_Header.DecalsRef.Offset + sizeof(DECAL_REF)*m_Header.DecalsRef.Count;

  if(m_Header.DetailObjCollRef.Count > 0)
    start = m_Header.DetailObjCollRef.Offset;

  start += 1000;
  UINT temp[10];
  m_pMapFile->Seek(start, 0);
  m_pMapFile->Read(temp, 40);

  end = m_Header.StructBsp.Offset;
  delta = end - start;
}

/*-------------------------------------------------------------------
 * Name: DuplicateScenery()
 * Description:
 *   
 *-----------------------------------------------------------------*/
BOOL CScenario::DuplicateScenery(int index)
{
  BOOL bSuccess = FALSE;

  if(m_Header.Scenery.Count < m_TagInfo[SCENERY].MaxEditCount)
  {
    memcpy(&m_pScenery[m_Header.Scenery.Count], &m_pScenery[index], sizeof(SCENERY_SPAWN));
    m_pScenery[m_Header.Scenery.Count].x += 1;
    m_pScenery[m_Header.Scenery.Count].y += 1;
    m_Header.Scenery.Count++;

    UpdateTools(gSelection.GetActiveSelectionMode(), m_Header.Scenery.Count-1);

    bSuccess = TRUE;
  }

  return(bSuccess);
}

/*-------------------------------------------------------------------
 * Name: DuplicateScenery()
 * Description:
 *   
 *-----------------------------------------------------------------*/
BOOL CScenario::DuplicateMachine(int index)
{
  BOOL bSuccess = FALSE;

  if(m_Header.Machine.Count < m_TagInfo[MACHINES].MaxEditCount)
  {
    memcpy(&m_pMachines[m_Header.Machine.Count], &m_pMachines[index], sizeof(MACHINE));
    m_pMachines[m_Header.Machine.Count].x += 1;
    m_pMachines[m_Header.Machine.Count].y += 1;
    m_Header.Machine.Count++;

    UpdateTools(gSelection.GetActiveSelectionMode(), m_Header.Machine.Count-1);

    bSuccess = TRUE;
  }

  return(bSuccess);
}

/*-------------------------------------------------------------------
 * Name: DuplicateVehicles()
 * Description:
 *   
 *-----------------------------------------------------------------*/
BOOL CScenario::DuplicateVehicles(int index)
{
  BOOL bSuccess = FALSE;

  if(m_Header.Vehicle.Count < m_TagInfo[VEHICLES].MaxEditCount)
  {
    memcpy(&m_pVehicles[m_Header.Vehicle.Count], &m_pVehicles[index], sizeof(VEHICLE_SPAWN));
    m_pVehicles[m_Header.Vehicle.Count].x += 1;
    m_pVehicles[m_Header.Vehicle.Count].y += 1;
    m_Header.Vehicle.Count++;
    bSuccess = TRUE;
  }

  return(bSuccess);
}

BOOL CScenario::DuplicateMpEquip(int index)
{
  BOOL bSuccess = FALSE;
  /*
  MP_EQUIP *pTemp = NULL;//(MP_EQUIP*)m_pScratchBuf;

  memcpy(pTemp, m_pMpEquip, sizeof(MP_EQUIP)*m_Header.MpEquip.Count);

  delete [] m_pMpEquip;
  m_pMpEquip = NULL;

  m_pMpEquip = new MP_EQUIP[m_Header.MpEquip.Count + 1];
  
  //copy original data into original spot
  memcpy(m_pMpEquip, pTemp, sizeof(MP_EQUIP)*m_Header.MpEquip.Count);
  
  //copy new data to end of spawn buffer
  memcpy(&m_pMpEquip[m_Header.MpEquip.Count], &pTemp[index], sizeof(MP_EQUIP));
  m_pMpEquip[m_Header.MpEquip.Count].x += 1;
  m_pMpEquip[m_Header.MpEquip.Count].y += 1;
*/
  if(m_Header.MpEquip.Count < m_TagInfo[MP_EQUIPMENT].MaxEditCount)
  {
    memcpy(&m_pMpEquip[m_Header.MpEquip.Count], &m_pMpEquip[index], sizeof(MP_EQUIP));
    m_pMpEquip[m_Header.MpEquip.Count].x += 1;
    m_pMpEquip[m_Header.MpEquip.Count].y += 1;

    if(m_Version == 7)
    {
      m_TagInfo[MP_EQUIPMENT].PcModelPointers[m_Header.MpEquip.Count] =
        m_TagInfo[MP_EQUIPMENT].PcModelPointers[index];
    }
    else if(m_Version == 5)
    {
      m_TagInfo[MP_EQUIPMENT].XboxModelPointers[m_Header.MpEquip.Count] =
        m_TagInfo[MP_EQUIPMENT].XboxModelPointers[index];
    }
    m_Header.MpEquip.Count++;

    UpdateTools(gSelection.GetActiveSelectionMode(), m_Header.MpEquip.Count-1);

    bSuccess = TRUE;
  }

  return(bSuccess);
}

/*-------------------------------------------------------------------
 * Name: GetMaxEditCount()
 * Description:
 *   Added this function as a workaround to VC6 reallocation compiler
 *   problems.
 *-----------------------------------------------------------------*/
int CScenario::GetMaxEditCount(SELECTION_MODE sel_mode, UINT count)
{
  int edit_count = 0;
  
  switch(sel_mode)
  {
  case VEHICLES:
    edit_count = 200;
    break;

  case SCENERY:
    edit_count = count*3;
    if(edit_count < 400)
      edit_count = 400;
    break;

  case WEAPONS:
    edit_count = count;
    break;

  case PLAYER_SPAWNS:
    edit_count = count;
    break;

  case MP_FLAGS:
    edit_count = count*2;
    break;

  case MP_EQUIPMENT:
    edit_count = count*3;
    if(edit_count < 400)
      edit_count = 400;
    break;

  case EQUIPMENT:
    edit_count = count;
    break; 

  case MACHINES:
    edit_count = count*10;
    if(edit_count < 400)
      edit_count = 400;
    break; 

  case BIPEDS:
    edit_count = count;
    break; 

  case TRIGGERS:
    edit_count = count;
    break; 
  }

  return(edit_count);
}

int CScenario::GetEditCount(SELECTION_MODE sel_mode)
{
  return(m_TagInfo[sel_mode].MaxEditCount);
}

void CScenario::SaveMpChanges()
{
  UINT of1, of2;
  int bias = 0;

  //pull an unaltered copy into the edit 
  memcpy(&m_EditedHeader, &m_OriginalHeader, sizeof(m_Header));

  // Save scenario header
  TRACE("(Header       )Offset:  %08X Offset To Next: %08X\n", m_ScenarioHeaderOffset, m_ScenarioHeaderOffset+sizeof(SCNR_HEADER));
  m_pMapFile->Seek(m_ScenarioHeaderOffset, 0);

  //Unmapped space, skybox
  of1 = m_Header.SkyBox.Offset;
  of2 = m_Header.SkyBox.Offset + sizeof(SKYBOX)*m_Header.SkyBox.Count;
  TRACE("(SkyBox       )Offset:  %08X Offset To Next: %08X\n", of1, of2);

  //  ----<START OF EDITS>----

  // Save scenery spawn edits
  if(m_Header.Scenery.Count > 0)
  {
    of1 = m_Header.Scenery.Offset;
    of2 = m_Header.Scenery.Offset + sizeof(SCENERY_SPAWN)*m_Header.Scenery.Count;
    TRACE("(Scenery      )Offset:  %08X Offset To Next: %08X\n", of1, of2);

    m_EditedHeader.Scenery.Count = m_Header.Scenery.Count;
    bias += sizeof(SCENERY_SPAWN)*(m_Header.Scenery.Count - m_OriginalHeader.Scenery.Count);
    //The first offset doesn't need to be edited, so just leave it

    //Perform the first seek, the remaining chunks will fall inline (no additional seeks necessary)
    m_pMapFile->Seek(m_Header.Scenery.Offset, 0);
    TRACE("(Scenery      ) %08X  %08X  %08X\n", 
      m_OriginalHeader.Scenery.Offset, m_EditedHeader.Scenery.Offset,
      m_EditedHeader.Scenery.Offset - m_OriginalHeader.Scenery.Offset);
    m_pMapFile->Write(m_pScenery, sizeof(SCENERY_SPAWN)*m_Header.Scenery.Count);
  }
  
  // Save scenery ref edits
  if(m_Header.SceneryRef.Count > 0)
  {
    of1 = m_Header.SceneryRef.Offset;
    of2 = m_Header.SceneryRef.Offset + sizeof(PALLETTE)*m_Header.SceneryRef.Count;
    TRACE("(SceneryRef   )Offset:  %08X Offset To Next: %08X\n", of1, of2);

    m_EditedHeader.SceneryRef.Count = m_Header.SceneryRef.Count;
    m_EditedHeader.SceneryRef.Offset += bias;
    bias += sizeof(PALLETTE)*(m_Header.SceneryRef.Count - m_OriginalHeader.SceneryRef.Count);

    TRACE("(SceneryRef   ) %08X  %08X  %08X\n", 
      m_OriginalHeader.SceneryRef.Offset, m_EditedHeader.SceneryRef.Offset,
      m_EditedHeader.SceneryRef.Offset - m_OriginalHeader.SceneryRef.Offset);
    m_pMapFile->Write(m_pSceneryRef, sizeof(PALLETTE)*m_Header.SceneryRef.Count);
  }

  // Save biped ref
  if(m_Header.BipedRef.Count > 0)
  {
    of1 = m_Header.BipedRef.Offset;
    of2 = m_Header.BipedRef.Offset + sizeof(PALLETTE)*m_Header.BipedRef.Count;
    TRACE("(BipedsRef    )Offset:  %08X Offset To Next: %08X\n", of1, of2);

    m_EditedHeader.BipedRef.Count = m_Header.BipedRef.Count;
    m_EditedHeader.BipedRef.Offset += bias;
    bias += sizeof(PALLETTE)*(m_Header.BipedRef.Count - m_OriginalHeader.BipedRef.Count);

    TRACE("(BipedRef     ) %08X  %08X  %08X\n", 
      m_OriginalHeader.BipedRef.Offset, m_EditedHeader.BipedRef.Offset,
      m_EditedHeader.BipedRef.Offset - m_OriginalHeader.BipedRef.Offset);
    m_pMapFile->Write(m_pBipedRef, sizeof(PALLETTE)*m_Header.BipedRef.Count);
  }

  // Save vehicle spawn edits
  if(m_Header.Vehicle.Count > 0)
  {
    of1 = m_Header.Vehicle.Offset;
    of2 = m_Header.Vehicle.Offset + sizeof(VEHICLE_SPAWN)*m_Header.Vehicle.Count;
    TRACE("(Vehicle      )Offset:  %08X Offset To Next: %08X\n", of1, of2);

    m_EditedHeader.Vehicle.Count = m_Header.Vehicle.Count;
    m_EditedHeader.Vehicle.Offset += bias;
    bias += sizeof(VEHICLE_SPAWN)*(m_Header.Vehicle.Count - m_OriginalHeader.Vehicle.Count);

    TRACE("(Vehicle      ) %08X  %08X  %08X\n", 
      m_OriginalHeader.Vehicle.Offset, m_EditedHeader.Vehicle.Offset,
      m_EditedHeader.Vehicle.Offset - m_OriginalHeader.Vehicle.Offset);
    m_pMapFile->Write(m_pVehicles, sizeof(VEHICLE_SPAWN)*m_Header.Vehicle.Count);
  }
  
  if(m_Header.VehicleRef.Count > 0)
  {
    of1 = m_Header.VehicleRef.Offset;
    of2 = m_Header.VehicleRef.Offset + sizeof(PALLETTE)*m_Header.VehicleRef.Count;
    TRACE("(VehicleRef   )Offset:  %08X Offset To Next: %08X\n", of1, of2);

    m_EditedHeader.VehicleRef.Count = m_Header.VehicleRef.Count;
    m_EditedHeader.VehicleRef.Offset += bias;
    bias += sizeof(PALLETTE)*(m_Header.VehicleRef.Count - m_OriginalHeader.VehicleRef.Count);

    TRACE("(VehicleRef   ) %08X  %08X  %08X\n", 
      m_OriginalHeader.VehicleRef.Offset, m_EditedHeader.VehicleRef.Offset,
      m_EditedHeader.VehicleRef.Offset - m_OriginalHeader.VehicleRef.Offset);
    m_pMapFile->Write(m_pVehicleRef, sizeof(PALLETTE)*m_Header.VehicleRef.Count);
  }

  //Machines
  if(m_Header.Machine.Count > 0)
  {
    of1 = m_Header.Machine.Offset;
    of2 = m_Header.Machine.Offset + sizeof(MACHINE)*m_Header.Machine.Count;
    TRACE("(Machines     )Offset:  %08X Offset To Next: %08X\n", of1, of2);

    m_EditedHeader.Machine.Count = m_Header.Machine.Count;
    m_EditedHeader.Machine.Offset += bias;
    bias += sizeof(MACHINE)*(m_Header.Machine.Count - m_OriginalHeader.Machine.Count);

    TRACE("(Machines     ) %08X  %08X  %08X\n", 
      m_OriginalHeader.Machine.Offset, m_EditedHeader.Machine.Offset,
      m_EditedHeader.Machine.Offset - m_OriginalHeader.Machine.Offset);
    m_pMapFile->Write(m_pMachines, sizeof(MACHINE)*m_Header.Machine.Count);
  }

  if(m_Header.MachineRef.Count > 0)
  {
    of1 = m_Header.MachineRef.Offset;
    of2 = m_Header.MachineRef.Offset + sizeof(PALLETTE)*m_Header.MachineRef.Count;
    TRACE("(MachinesRef  )Offset:  %08X Offset To Next: %08X\n", of1, of2);

    m_EditedHeader.MachineRef.Count = m_Header.MachineRef.Count;
    m_EditedHeader.MachineRef.Offset += bias;
    bias += sizeof(PALLETTE)*(m_Header.MachineRef.Count - m_OriginalHeader.MachineRef.Count);

    TRACE("(MachinesRef  ) %08X  %08X  %08X\n", 
      m_OriginalHeader.MachineRef.Offset, m_EditedHeader.MachineRef.Offset,
      m_EditedHeader.MachineRef.Offset - m_OriginalHeader.MachineRef.Offset);
    m_pMapFile->Write(m_pMachinesRef, sizeof(PALLETTE)*m_Header.MachineRef.Count);
  }

  //Controls
  if(m_Header.Control.Count > 0)
  {
    of1 = m_Header.Control.Offset;
    of2 = m_Header.Control.Offset + sizeof(CONTROL)*m_Header.Control.Count;
    TRACE("(Control      )Offset:  %08X Offset To Next: %08X\n", of1, of2);

    m_EditedHeader.Control.Count = m_Header.Control.Count;
    m_EditedHeader.Control.Offset += bias;
    bias += sizeof(CONTROL)*(m_Header.Control.Count - m_OriginalHeader.Control.Count);

    TRACE("(Control      ) %08X  %08X  %08X\n", 
      m_OriginalHeader.Control.Offset, m_EditedHeader.Control.Offset,
      m_EditedHeader.Control.Offset - m_OriginalHeader.Control.Offset);
    m_pMapFile->Write(m_pControls, sizeof(CONTROL)*m_Header.Control.Count);
  }

  if(m_Header.ControlRef.Count > 0)
  {
    of1 = m_Header.ControlRef.Offset;
    of2 = m_Header.ControlRef.Offset + sizeof(PALLETTE)*m_Header.ControlRef.Count;
    TRACE("(ControlRef   )Offset:  %08X Offset To Next: %08X\n", of1, of2);

    m_EditedHeader.ControlRef.Count = m_Header.ControlRef.Count;
    m_EditedHeader.ControlRef.Offset += bias;
    bias += sizeof(PALLETTE)*(m_Header.ControlRef.Count - m_OriginalHeader.ControlRef.Count);

    TRACE("(ControlRef   ) %08X  %08X  %08X\n", 
      m_OriginalHeader.ControlRef.Offset, m_EditedHeader.ControlRef.Offset,
      m_EditedHeader.ControlRef.Offset - m_OriginalHeader.ControlRef.Offset);
    m_pMapFile->Write(m_pControlsRef, sizeof(PALLETTE)*m_Header.ControlRef.Count);
  }

  //Light Fixtures
  if(m_Header.LightFixture.Count > 0)
  {
    of1 = m_Header.LightFixture.Offset;
    of2 = m_Header.LightFixture.Offset + sizeof(LIGHT_FIXTURE)*m_Header.LightFixture.Count;
    TRACE("(LightFixt    )Offset:  %08X Offset To Next: %08X\n", of1, of2);

    m_EditedHeader.LightFixture.Count = m_Header.LightFixture.Count;
    m_EditedHeader.LightFixture.Offset += bias;
    bias += sizeof(LIGHT_FIXTURE)*(m_Header.LightFixture.Count - m_OriginalHeader.LightFixture.Count);

    TRACE("(LightFixt    ) %08X  %08X  %08X\n", 
      m_OriginalHeader.LightFixture.Offset, m_EditedHeader.LightFixture.Offset,
      m_EditedHeader.LightFixture.Offset - m_OriginalHeader.LightFixture.Offset);
    m_pMapFile->Write(m_pLightFixtures, sizeof(LIGHT_FIXTURE)*m_Header.LightFixture.Count);
  }

  if(m_Header.LightFixtureRef.Count > 0)
  {
    of1 = m_Header.LightFixtureRef.Offset;
    of2 = m_Header.LightFixtureRef.Offset + sizeof(PALLETTE)*m_Header.LightFixtureRef.Count;
    TRACE("(LightFixtRef )Offset:  %08X Offset To Next: %08X\n", of1, of2);

    m_EditedHeader.LightFixtureRef.Count = m_Header.LightFixtureRef.Count;
    m_EditedHeader.LightFixtureRef.Offset += bias;
    bias += sizeof(PALLETTE)*(m_Header.LightFixtureRef.Count - m_OriginalHeader.LightFixtureRef.Count);

    TRACE("(LightFixtRef ) %08X  %08X  %08X\n", 
      m_OriginalHeader.LightFixtureRef.Offset, m_EditedHeader.LightFixtureRef.Offset,
      m_EditedHeader.LightFixtureRef.Offset - m_OriginalHeader.LightFixtureRef.Offset);
    m_pMapFile->Write(m_pLightFixturesRef, sizeof(PALLETTE)*m_Header.LightFixtureRef.Count);
  }


  if(m_Header.SoundScenery.Count > 0)
  {
    of1 = m_Header.SoundScenery.Offset;
    of2 = m_Header.SoundScenery.Offset + sizeof(SOUND_SCENERY)*m_Header.SoundScenery.Count;
    TRACE("(SoundScenery )Offset:  %08X Offset To Next: %08X\n", of1, of2);

    m_EditedHeader.SoundScenery.Count = m_Header.SoundScenery.Count;
    m_EditedHeader.SoundScenery.Offset += bias;
    bias += sizeof(SOUND_SCENERY)*(m_Header.SoundScenery.Count - m_OriginalHeader.SoundScenery.Count);

    TRACE("(SoundScenery ) %08X  %08X  %08X\n", 
      m_OriginalHeader.SoundScenery.Offset, m_EditedHeader.SoundScenery.Offset,
      m_EditedHeader.SoundScenery.Offset - m_OriginalHeader.SoundScenery.Offset);
    m_pMapFile->Write(m_pSounds, sizeof(SOUND_SCENERY)*m_Header.SoundScenery.Count);
  }

  if(m_Header.SoundSceneryRef.Count > 0)
  {
    of1 = m_Header.SoundSceneryRef.Offset;
    of2 = m_Header.SoundSceneryRef.Offset + sizeof(PALLETTE)*m_Header.SoundSceneryRef.Count;
    TRACE("(SoundScenRef )Offset:  %08X Offset To Next: %08X\n", of1, of2);

    m_EditedHeader.SoundSceneryRef.Count = m_Header.SoundSceneryRef.Count;
    m_EditedHeader.SoundSceneryRef.Offset += bias;
    bias += sizeof(PALLETTE)*(m_Header.SoundSceneryRef.Count - m_OriginalHeader.SoundSceneryRef.Count);

    TRACE("(SoundScenRef ) %08X  %08X  %08X\n", 
      m_OriginalHeader.SoundSceneryRef.Offset, m_EditedHeader.SoundSceneryRef.Offset,
      m_EditedHeader.SoundSceneryRef.Offset - m_OriginalHeader.SoundSceneryRef.Offset);
    m_pMapFile->Write(m_pSoundRef, sizeof(PALLETTE)*m_Header.SoundSceneryRef.Count);
  }

  // Save player spawn edits
  if(m_Header.PlayerSpawn.Count > 0)
  {
    of1 = m_Header.PlayerSpawn.Offset;
    of2 = m_Header.PlayerSpawn.Offset + sizeof(PLAYER_SPAWN)*m_Header.PlayerSpawn.Count;
    TRACE("(PlayerSpawn  )Offset:  %08X Offset To Next: %08X\n", of1, of2);

    m_EditedHeader.PlayerSpawn.Count = m_Header.PlayerSpawn.Count;
    m_EditedHeader.PlayerSpawn.Offset += bias;
    bias += sizeof(PLAYER_SPAWN)*(m_Header.PlayerSpawn.Count - m_OriginalHeader.PlayerSpawn.Count);

    TRACE("(PlayerSpawn  ) %08X  %08X  %08X\n", 
      m_OriginalHeader.PlayerSpawn.Offset, m_EditedHeader.PlayerSpawn.Offset,
      m_EditedHeader.PlayerSpawn.Offset - m_OriginalHeader.PlayerSpawn.Offset);
    m_pMapFile->Write(m_pPlayerSpawns, sizeof(PLAYER_SPAWN)*m_Header.PlayerSpawn.Count);
  }
  
  // Save item MP flag edits
  if(m_Header.MultiplayerFlags.Count > 0)
  {
    of1 = m_Header.MultiplayerFlags.Offset;
    of2 = m_Header.MultiplayerFlags.Offset + sizeof(MP_FLAG)*m_Header.MultiplayerFlags.Count;
    TRACE("(MP Flags     )Offset:  %08X Offset To Next: %08X\n", of1, of2);

    m_EditedHeader.MultiplayerFlags.Count = m_Header.MultiplayerFlags.Count;
    m_EditedHeader.MultiplayerFlags.Offset += bias;
    bias += sizeof(MP_FLAG)*(m_Header.MultiplayerFlags.Count - m_OriginalHeader.MultiplayerFlags.Count);

    TRACE("(MP Flags     ) %08X  %08X  %08X\n", 
      m_OriginalHeader.MultiplayerFlags.Offset, m_EditedHeader.MultiplayerFlags.Offset,
      m_EditedHeader.MultiplayerFlags.Offset - m_OriginalHeader.MultiplayerFlags.Offset);
    m_pMapFile->Write(m_pMpFlags, sizeof(MP_FLAG)*m_Header.MultiplayerFlags.Count);
  }
  
  // Save item MP Equipment edits
  if(m_Header.MpEquip.Count > 0)
  {
    of1 = m_Header.MpEquip.Offset;
    of2 = m_Header.MpEquip.Offset + sizeof(MP_EQUIP)*m_Header.MpEquip.Count;
    TRACE("(MP Equip     )Offset:  %08X Offset To Next: %08X\n", of1, of2);

    m_EditedHeader.MpEquip.Count = m_Header.MpEquip.Count;
    m_EditedHeader.MpEquip.Offset += bias;
    bias += sizeof(MP_EQUIP)*(m_Header.MpEquip.Count - m_OriginalHeader.MpEquip.Count);

    TRACE("(MP Equip     ) %08X  %08X  %08X\n", 
      m_OriginalHeader.MpEquip.Offset, m_EditedHeader.MpEquip.Offset,
      m_EditedHeader.MpEquip.Offset - m_OriginalHeader.MpEquip.Offset);
    m_pMapFile->Write(m_pMpEquip, sizeof(MP_EQUIP)*m_Header.MpEquip.Count);
  }

  if(m_Header.StartingEquip.Count > 0)
  {
    of1 = m_Header.StartingEquip.Offset;
    of2 = m_Header.StartingEquip.Offset + sizeof(STARTING_EQUIP)*m_Header.StartingEquip.Count;
    TRACE("(StartingEquip)Offset:  %08X Offset To Next: %08X\n", of1, of2);

    m_EditedHeader.StartingEquip.Count = m_Header.StartingEquip.Count;
    m_EditedHeader.StartingEquip.Offset += bias;
    bias += sizeof(STARTING_EQUIP)*(m_Header.StartingEquip.Count - m_OriginalHeader.StartingEquip.Count);

    TRACE("(StartingEquip) %08X  %08X  %08X\n", 
      m_OriginalHeader.StartingEquip.Offset, m_EditedHeader.StartingEquip.Offset,
      m_EditedHeader.StartingEquip.Offset - m_OriginalHeader.StartingEquip.Offset);
    m_pMapFile->Write(m_pStartingEquip, sizeof(STARTING_EQUIP)*m_Header.StartingEquip.Count);
  }

  // Save Decals
  if(m_Header.Decals.Count > 0)
  {
    of1 = m_Header.Decals.Offset;
    of2 = m_Header.Decals.Offset + sizeof(DECAL)*m_Header.Decals.Count;
    TRACE("(Decals       )Offset:  %08X Offset To Next: %08X\n", of1, of2);

    m_EditedHeader.Decals.Count = m_Header.Decals.Count;
    m_EditedHeader.Decals.Offset += bias;
    bias += sizeof(DECAL)*(m_Header.Decals.Count - m_OriginalHeader.Decals.Count);

    TRACE("(Decals       ) %08X  %08X  %08X\n", 
      m_OriginalHeader.Decals.Offset, m_EditedHeader.Decals.Offset,
      m_EditedHeader.Decals.Offset - m_OriginalHeader.Decals.Offset);
    m_pMapFile->Write(m_pDecals, sizeof(DECAL)*m_Header.Decals.Count);
  }

  if(m_Header.DecalsRef.Count > 0)
  {
    of1 = m_Header.DecalsRef.Offset;
    of2 = m_Header.DecalsRef.Offset + sizeof(DECAL_REF)*m_Header.DecalsRef.Count;
    TRACE("(DecalsRef    )Offset:  %08X Offset To Next: %08X\n", of1, of2);

    m_EditedHeader.DecalsRef.Count = m_Header.DecalsRef.Count;
    m_EditedHeader.DecalsRef.Offset += bias;
    bias += sizeof(DECAL_REF)*(m_Header.DecalsRef.Count - m_OriginalHeader.DecalsRef.Count);

    TRACE("(DecalsRef    ) %08X  %08X  %08X\n", 
      m_OriginalHeader.DecalsRef.Offset, m_EditedHeader.DecalsRef.Offset,
      m_EditedHeader.DecalsRef.Offset - m_OriginalHeader.DecalsRef.Offset);
    m_pMapFile->Write(m_pDecalsRef, sizeof(DECAL)*m_Header.DecalsRef.Count);
  }

  //script crap
  if(m_Header.ScriptCrap.Count > 0)
  {
    of1 = m_Header.ScriptCrap.Offset;
    of2 = m_Header.ScriptCrap.Offset + m_Header.ScriptCrap.Count;
    TRACE("(ScriptCrap   )Offset:  %08X Offset To Next: %08X\n", of1, of2);

    m_EditedHeader.ScriptCrap.Count = m_Header.ScriptCrap.Count;
    m_EditedHeader.ScriptCrap.Offset += bias;
    bias += m_Header.ScriptCrap.Count;

    TRACE("(ScriptCrap   ) %08X  %08X  %08X\n", 
      m_OriginalHeader.ScriptCrap.Offset, m_EditedHeader.ScriptCrap.Offset,
      m_EditedHeader.ScriptCrap.Offset - m_OriginalHeader.ScriptCrap.Offset);
    m_pMapFile->Write(m_pScriptCrap, m_Header.ScriptCrap.Count);
  }

  m_pMapFile->Seek(m_ScenarioHeaderOffset, 0);
  m_pMapFile->Write(&m_EditedHeader, sizeof(m_EditedHeader));

  of1 = m_Header.StructBsp.Offset;
  of2 = m_Header.StructBsp.Offset + sizeof(SCENARIO_BSP_INFO)*m_Header.StructBsp.Count;
  TRACE("(BSP          )Offset:  %08X Offset To Next: %08X\n", of1, of2);
}

void CScenario::LoadSkybox()
{
  CString temp;
  CPcModel *pPcModel;

  if(m_Header.SkyBox.Count > 0)
  {
    m_pSkyBox = new SKYBOX[m_Header.SkyBox.Count];
    m_pMapFile->Seek(m_Header.SkyBox.Offset, 0);
    m_pMapFile->Read(m_pSkyBox, sizeof(SKYBOX)*m_Header.SkyBox.Count);

    temp = CheckForReference(m_pMapFile, m_pSkyBox[0].NameRef, m_Magic);
    gTagManager.GetTagPcModel(m_pSkyBox[0].TagId, &pPcModel);
    gRender.SetSkyboxModel(pPcModel, NULL);
  }

}

void CScenario::LoadDecals()
{
  TRACE("scenario decals:  %d\n", m_Header.Decals.Count);
  if(m_Header.Decals.Count > 0)
  {
    m_pDecals = new DECAL[m_Header.Decals.Count];
    m_pMapFile->Seek(m_Header.Decals.Offset, 0);
    m_pMapFile->Read(m_pDecals, sizeof(DECAL)*m_Header.Decals.Count);
  }

  if(m_Header.DecalsRef.Count > 0)
  {
    m_pDecalsRef = new DECAL_REF[m_Header.DecalsRef.Count];

    m_pMapFile->Seek(m_Header.DecalsRef.Offset, 0);
    m_pMapFile->Read(m_pDecalsRef, sizeof(DECAL_REF)*m_Header.DecalsRef.Count);
  }
}

void CScenario::LoadStartingEquip()
{
  if(m_Header.StartingEquip.Count > 0)
  {
    m_pStartingEquip = new STARTING_EQUIP[m_Header.StartingEquip.Count];

    m_pMapFile->Seek(m_Header.StartingEquip.Offset, 0);
    m_pMapFile->Read(m_pStartingEquip, sizeof(STARTING_EQUIP)*m_Header.StartingEquip.Count);
  }
}

void CScenario::LoadScriptCrap()
{
  if(m_Header.ScriptCrap.Count > 0)
  {
    TRACE("ScriptCrap size = %08X\n", m_Header.ScriptCrap.Count);
    m_pScriptCrap = new BYTE[m_Header.ScriptCrap.Count];

    m_pMapFile->Seek(m_Header.ScriptCrap.Offset, 0);
    m_pMapFile->Read(m_pScriptCrap, m_Header.ScriptCrap.Count);
  }
}

void CScenario::LoadControls()
{
  if(m_Header.Control.Count > 0)
  {
    m_pControls = new CONTROL[m_Header.Control.Count];

    m_pMapFile->Seek(m_Header.Control.Offset, 0);
    m_pMapFile->Read(m_pControls, sizeof(CONTROL)*m_Header.Control.Count);
  }

  if(m_Header.ControlRef.Count > 0)
  {
    m_pControlsRef = new PALLETTE[m_Header.ControlRef.Count];

    m_pMapFile->Seek(m_Header.ControlRef.Offset, 0);
    m_pMapFile->Read(m_pControlsRef, sizeof(PALLETTE)*m_Header.ControlRef.Count);

    //for(int i=0; i<m_Header.ControlRef.Count; i++)
    //{
    //  CString ref = CheckForReference(m_pMapFile, m_pControlsRef[0].NamePtr, m_Magic);
    //}
  }
}

BOOL CScenario::DeleteScenery(int index)
{
  BOOL bSuccess = FALSE;
  int count;

  if((index >= 0)&&(index < m_Header.Scenery.Count))
  {
    if(index == (m_Header.Scenery.Count-1))
    {
      m_Header.Scenery.Count--;
    }
    else
    {
      count = m_Header.Scenery.Count - index;
      memcpy(&m_pScenery[index], &m_pScenery[index+1], sizeof(SCENERY_SPAWN)*count);
      m_Header.Scenery.Count--;
    }

    bSuccess = TRUE;
  }

  return(bSuccess);
}

BOOL CScenario::DeleteVehicle(int index)
{
  BOOL bSuccess = FALSE;
  int count;

  if((index >= 0)&&(index < m_Header.Vehicle.Count))
  {
    if(index == (m_Header.Vehicle.Count-1))
    {
      m_Header.Vehicle.Count--;
    }
    else
    {
      count = m_Header.Vehicle.Count - index;
      memcpy(&m_pVehicles[index], &m_pVehicles[index+1], sizeof(VEHICLE_SPAWN)*count);
      m_Header.Vehicle.Count--;
    }

    bSuccess = TRUE;
  }

  return(bSuccess);
}

BOOL CScenario::DeleteMpEquip(int index)
{
  BOOL bSuccess = FALSE;
  int count;

  if((index >= 0)&&(index < m_Header.MpEquip.Count))
  {
    if(index == (m_Header.MpEquip.Count-1))
    {
      m_Header.MpEquip.Count--;
    }
    else
    {
      count = m_Header.MpEquip.Count - index;
      memcpy(&m_pMpEquip[index], &m_pMpEquip[index+1], sizeof(MP_EQUIP)*count);
      m_Header.MpEquip.Count--;
    }

    bSuccess = TRUE;
  }

  return(bSuccess);
}

void CScenario::AddTeleporter(float *pCoord)
{
  int sourcemax = -1;
  int destmax = -1;
  //go through and find teleporter count
  for(int i=0; i<m_Header.MultiplayerFlags.Count; i++)
  {

    if(m_pMpFlags[i].index1 == 6)//origin
    {
      if(m_pMpFlags[i].index2 > sourcemax)
        sourcemax = m_pMpFlags[i].index2;

      //TRACE("%d %s  #%d\n", i, m_MpFlagTypes[m_pMpFlags[i].index1], m_pMpFlags[i].index2);
    }

    if(m_pMpFlags[i].index1 == 7)//destination
    {
      if(m_pMpFlags[i].index2 > destmax)
        destmax = m_pMpFlags[i].index2;
      //TRACE("%d %s  #%d\n", i, m_MpFlagTypes[m_pMpFlags[i].index1], m_pMpFlags[i].index2);
    }
  }
  sourcemax++;
  destmax++;
  //TRACE("Next Source: %d Next Dest %d\n", sourcemax, destmax);

  if((m_Header.MultiplayerFlags.Count+1) < m_TagInfo[MP_FLAGS].MaxEditCount)
  {
    ZeroMemory(&m_pMpFlags[m_Header.MultiplayerFlags.Count], sizeof(MP_FLAGS));
    m_pMpFlags[m_Header.MultiplayerFlags.Count].x = pCoord[0];
    m_pMpFlags[m_Header.MultiplayerFlags.Count].y = pCoord[1];
    m_pMpFlags[m_Header.MultiplayerFlags.Count].z = pCoord[2];
    m_pMpFlags[m_Header.MultiplayerFlags.Count].yaw = 0;
    m_pMpFlags[m_Header.MultiplayerFlags.Count].index1 = 6;
    m_pMpFlags[m_Header.MultiplayerFlags.Count].index2 = sourcemax;
    m_pMpFlags[m_Header.MultiplayerFlags.Count].tag[0] = 'c';
    m_pMpFlags[m_Header.MultiplayerFlags.Count].tag[1] = 'm';
    m_pMpFlags[m_Header.MultiplayerFlags.Count].tag[2] = 't';
    m_pMpFlags[m_Header.MultiplayerFlags.Count].tag[3] = 'i';
    m_pMpFlags[m_Header.MultiplayerFlags.Count].unk2[2] = 0xFFFFFFFF;
    m_Header.MultiplayerFlags.Count++;

    ZeroMemory(&m_pMpFlags[m_Header.MultiplayerFlags.Count], sizeof(MP_FLAGS));
    m_pMpFlags[m_Header.MultiplayerFlags.Count].x = pCoord[0]+0.3;
    m_pMpFlags[m_Header.MultiplayerFlags.Count].y = pCoord[1];
    m_pMpFlags[m_Header.MultiplayerFlags.Count].z = pCoord[2];
    m_pMpFlags[m_Header.MultiplayerFlags.Count].yaw = 0;
    m_pMpFlags[m_Header.MultiplayerFlags.Count].index1 = 7;
    m_pMpFlags[m_Header.MultiplayerFlags.Count].index2 = sourcemax;
    m_pMpFlags[m_Header.MultiplayerFlags.Count].tag[0] = 'c';
    m_pMpFlags[m_Header.MultiplayerFlags.Count].tag[1] = 'm';
    m_pMpFlags[m_Header.MultiplayerFlags.Count].tag[2] = 't';
    m_pMpFlags[m_Header.MultiplayerFlags.Count].tag[3] = 'i';
    m_pMpFlags[m_Header.MultiplayerFlags.Count].unk2[2] = 0xFFFFFFFF;
    m_Header.MultiplayerFlags.Count++;
  }
}

void CScenario::LoadLightFixtures()
{
  if(m_Header.LightFixture.Count > 0)
  {
    m_pLightFixtures = new LIGHT_FIXTURE[m_Header.LightFixture.Count];
    m_pMapFile->Seek(m_Header.LightFixture.Offset, 0);
    m_pMapFile->Read(m_pLightFixtures, sizeof(LIGHT_FIXTURE)*m_Header.LightFixture.Count);
  }

  if(m_Header.LightFixtureRef.Count > 0)
  {
    m_pLightFixturesRef = new PALLETTE[m_Header.LightFixtureRef.Count];

    m_pMapFile->Seek(m_Header.LightFixtureRef.Offset, 0);
    m_pMapFile->Read(m_pLightFixturesRef, sizeof(PALLETTE)*m_Header.LightFixtureRef.Count);
  }
}

BOOL CScenario::CheckPalletteForTagExistence(UINT tag_type, UINT tag_id)
{
  BOOL bTagExists = FALSE;
  int i;

  switch(tag_type)
  {
  case TAG_SCENERY:
    for(i=0; i<m_Header.SceneryRef.Count; i++)
    {
      if(tag_id == m_pSceneryRef[i].ObjectTagId)
      {
        bTagExists = TRUE;
        break;
      }
    }
    break;

  case TAG_VEHICLE:
    for(i=0; i<m_Header.VehicleRef.Count; i++)
    {
      if(tag_id == m_pVehicleRef[i].ObjectTagId)
      {
        bTagExists = TRUE;
        break;
      }
    }
    break;

  default:
    break;
  }

  return(bTagExists);
}

void CScenario::AddVehicleType(UINT tag_id, UINT RawNameOffset)
{
  if(m_Header.VehicleRef.Count < m_MaxPalletteCount[VEHICLE_REF])
  {
    ZeroMemory(&m_pVehicleRef[m_Header.VehicleRef.Count], sizeof(PALLETTE));
    m_pVehicleRef[m_Header.VehicleRef.Count].tag[0] = 'i';
    m_pVehicleRef[m_Header.VehicleRef.Count].tag[1] = 'h';
    m_pVehicleRef[m_Header.VehicleRef.Count].tag[2] = 'e';
    m_pVehicleRef[m_Header.VehicleRef.Count].tag[3] = 'v';
    m_pVehicleRef[m_Header.VehicleRef.Count].NamePtr = RawNameOffset;
    m_pVehicleRef[m_Header.VehicleRef.Count].unk1 = 0;
    m_pVehicleRef[m_Header.VehicleRef.Count].ObjectTagId = tag_id;
    m_pVehicleRef[m_Header.VehicleRef.Count].unk[0] = 0;
    m_pVehicleRef[m_Header.VehicleRef.Count].unk[1] = 0;
    m_pVehicleRef[m_Header.VehicleRef.Count].unk[2] = 0;
    m_pVehicleRef[m_Header.VehicleRef.Count].unk[3] = 0;
    m_pVehicleRef[m_Header.VehicleRef.Count].unk[4] = 0;
    m_pVehicleRef[m_Header.VehicleRef.Count].unk[5] = 0;
    m_pVehicleRef[m_Header.VehicleRef.Count].unk[6] = 0;
    m_pVehicleRef[m_Header.VehicleRef.Count].unk[7] = 0;

    gTagManager.GetTagPcModel(tag_id,
                              &m_TagInfo[VEHICLES].PcModelPointers[m_Header.VehicleRef.Count]);

    gTagManager.GetTagXboxModel(tag_id,
                              &m_TagInfo[VEHICLES].XboxModelPointers[m_Header.VehicleRef.Count]);

    m_Header.VehicleRef.Count++;
  }
}

void CScenario::AddSceneryType(UINT tag_id, UINT RawNameOffset)
{
  if(m_Header.SceneryRef.Count < m_MaxPalletteCount[SCENERY_REF])
  {
    ZeroMemory(&m_pSceneryRef[m_Header.SceneryRef.Count], sizeof(PALLETTE));
    m_pSceneryRef[m_Header.SceneryRef.Count].tag[0] = 'n';
    m_pSceneryRef[m_Header.SceneryRef.Count].tag[1] = 'e';
    m_pSceneryRef[m_Header.SceneryRef.Count].tag[2] = 'c';
    m_pSceneryRef[m_Header.SceneryRef.Count].tag[3] = 's';
    m_pSceneryRef[m_Header.SceneryRef.Count].NamePtr = RawNameOffset;
    m_pSceneryRef[m_Header.SceneryRef.Count].unk1 = 0;
    m_pSceneryRef[m_Header.SceneryRef.Count].ObjectTagId = tag_id;
    m_pSceneryRef[m_Header.SceneryRef.Count].unk[0] = 0;
    m_pSceneryRef[m_Header.SceneryRef.Count].unk[1] = 0;
    m_pSceneryRef[m_Header.SceneryRef.Count].unk[2] = 0;
    m_pSceneryRef[m_Header.SceneryRef.Count].unk[3] = 0;
    m_pSceneryRef[m_Header.SceneryRef.Count].unk[4] = 0;
    m_pSceneryRef[m_Header.SceneryRef.Count].unk[5] = 0;
    m_pSceneryRef[m_Header.SceneryRef.Count].unk[6] = 0;
    m_pSceneryRef[m_Header.SceneryRef.Count].unk[7] = 0;

    gTagManager.GetTagPcModel(tag_id,
                              &m_TagInfo[SCENERY].PcModelPointers[m_Header.SceneryRef.Count]);

    gTagManager.GetTagXboxModel(tag_id,
                              &m_TagInfo[SCENERY].XboxModelPointers[m_Header.SceneryRef.Count]);

    m_Header.SceneryRef.Count++;
  }
}
