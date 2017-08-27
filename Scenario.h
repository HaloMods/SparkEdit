#if !defined(AFX_SCENARIO_H__BF1557BF_5748_4EAB_AE14_BCD4DCD4F33F__INCLUDED_)
#define AFX_SCENARIO_H__BF1557BF_5748_4EAB_AE14_BCD4DCD4F33F__INCLUDED_

#include "HaloStructDefs.h"
#include "VisibleBsp.h"
#include "Util.h"
#include "PcModel.h"
#include "XboxModel.h"


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct STRUCT_SCNR_GROUP
{
  int SelectedItemIndex; // the item that is currently selected, -1 if none
  int SelectedRefIndex;  //?
  void *pDataStruct;     //pointer to the start of the data struct array
  BOOL GroupActive;      //says if the group can be selected or not
  int GroupFilePointer;  // points to the start of the original data in the file
  int MaxEditCount;
  CPcModel **PcModelPointers;
  CXboxModel **XboxModelPointers;
}SCNR_GROUP;

#define PALLETTE_TYPES_MAX  2

typedef enum ENUM_PALLETTE_TYPES
{
  VEHICLE_REF,
  SCENERY_REF
}PALLETTE_TYPES;

class CScenario : public CUtil
{
public:
	void AddSceneryType(UINT tag_id, UINT RawNameOffset);
	void AddVehicleType(UINT tag_id, UINT RawNameOffset);
	BOOL CheckPalletteForTagExistence(UINT tag_type, UINT tag_id);
	void LoadLightFixtures(void);
	void AddTeleporter(float *pCoord);
	BOOL DeleteMpEquip(int index);
	BOOL DeleteVehicle(int index);
	BOOL DeleteScenery(int index);
	void LoadSkybox(void);
	void SaveMpChanges(void);
	int GetEditCount(SELECTION_MODE sel_mode);
	int GetMaxEditCount(SELECTION_MODE sel_mode, UINT count);
  BOOL DuplicateMachine(int index);
  BOOL DuplicateMpEquip(int index);
  BOOL DuplicateVehicles(int index);
	BOOL DuplicateScenery(int index);
	void MapSlackSpace(void);
	void SetObjectType(SELECTION_MODE sel_mode, int object_index, int new_type);
  CString GetPalletteString(SELECTION_MODE sel_mode, int index);
	int GetPalletteCount(SELECTION_MODE sel_mode);
	void GetTriggerVolume(int index, BOUNDING_BOX *pBox);
	CString GetModelDescription(SELECTION_MODE sel_mode, int index);
	void GetTagModel(SELECTION_MODE sel_mode, int index, CPcModel **pPcModel, CXboxModel **pXboxModel);
	void LoadBspInfo(UINT version);
	void UpdateTools(SELECTION_MODE sel_mode, int index);
	void SaveChanges(void);
	BOOL GetTagCoord(SELECTION_MODE sel_mode, int index, float *pCoord);
	int GetTagCount(SELECTION_MODE sel_mode);
	void EditSelectedTag(SELECTION_MODE sel_mode, int index, float *move_vector);
	void Cleanup();
	void LoadScenario(CFile *pMapFile, int magic, UINT ScnrOffset, UINT version, BOOL bMultiplayer);
	CScenario();
	virtual ~CScenario();

protected:
	void LoadControls(void);
	void LoadScriptCrap(void);
	void LoadStartingEquip(void);
	void LoadDecals(void);
	void LoadAI(void);
	void LoadSoundScenery(void);
	void LoadMachines(void);
	void LoadEquipment(void);
	void LoadBipeds(void);
	void LoadTriggerVolumes();
	void LoadObjectNames(void);
	void LoadMovePositions(void);
	void LoadDeviceGroups(void);
	void LoadStartingProfiles(void);
	void LoadBspTriggers(void);
	void LoadPlayerSpawns(void);
	void LoadMultiplayer(void);
	void TranslateTagCoord(float *move_vector, float *pData, BOOL bFullRot);
	void LoadWeapons(void);
	void LoadScenery();
	void LoadVehicles(void);
  SCNR_HEADER m_Header;
  SCNR_HEADER m_EditedHeader;
  SCNR_HEADER m_OriginalHeader;
  VEHICLE_SPAWN *m_pVehicles;
  PALLETTE *m_pVehicleRef;
  SCENERY_SPAWN *m_pScenery;
  PALLETTE *m_pSceneryRef;
  WEAPON_SPAWN *m_pWeapons;
  PLAYER_SPAWN *m_pPlayerSpawns;
  MP_FLAG *m_pMpFlags;
  MP_EQUIP *m_pMpEquip;
  BSP_TRIGGER *m_pBspTriggers;
  DEVICE_GROUP *m_pDeviceGroups;
  PLAYER_STARTING_PROFILE *m_pStartingProfile;
  MOVE_POSITIONS *m_pMovePositions;
  OBJECT_NAMES *m_pObjectNames;
  TRIGGER_VOLUMES *m_pTriggerVolumes;
  BIPED *m_pBiped;
  PALLETTE *m_pBipedRef;
  EQUIP *m_pEquip;
  PALLETTE *m_pEquipRef;
  PALLETTE *m_pWeaponRef;
  MACHINE *m_pMachines;
  PALLETTE *m_pMachinesRef;
  SOUND_SCENERY *m_pSounds;
  PALLETTE *m_pSoundRef;
  ACTOR_VARIANT_REF *m_pActorVarRef;
  AI_ANIMATION_REF *m_pAiAnimationRef;
  SKYBOX *m_pSkyBox;
  DECAL *m_pDecals;
  DECAL_REF *m_pDecalsRef;
  STARTING_EQUIP *m_pStartingEquip;

  AI_SCRIPT_REF *m_pAiScriptRef;
  AI_RECORDING_REF *m_pAiRecordingRef;
  SCRIPT_TRIGGER *m_pScriptTriggers;
  GLOBALS *m_pCutScenes;
  REFERENCES *m_pReferences;
  CONTROL *m_pControls;
  PALLETTE *m_pControlsRef;
  LIGHT_FIXTURE *m_pLightFixtures;
  PALLETTE *m_pLightFixturesRef;

  UINT m_ScenarioHeaderOffset;
  int m_Magic;
  UINT m_Version;
  BOOL m_bMultiplayer;
  CFile *m_pMapFile;
  CStringArray m_MpFlagTypes;
  CStringArray m_SoundNames;
  BYTE *m_pScriptCrap;

  SCNR_GROUP m_TagInfo[SELECTION_MODE_MAX]; //set to the number of supported groups
  UINT m_MaxPalletteCount[PALLETTE_TYPES_MAX];
};

#endif // !defined(AFX_SCENARIO_H__BF1557BF_5748_4EAB_AE14_BCD4DCD4F33F__INCLUDED_)
