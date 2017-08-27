/*-------------------------------------------------------------------
 *  Author: Grenadiac   8/18/03
 *
 *  Contributing Authors:
 *
 *  File Description: 
 *    Based on the original files created by Pfhorslayer.
 *
 *-----------------------------------------------------------------*/
#ifndef __HALOSTRUCTDEFS_H__
#define __HALOSTRUCTDEFS_H__

#include "basic_types.h"
#include "bsp_defs.h"

#define HEADER_EMPTY_INTS   485
#define RAD_TO_DEG   57.2957795f
#define DEG_TO_RAD   0.01745329252f

typedef struct STRUCT_MAPFILE_HDR
{
  int  id;                    // 'head'
  int  Version;               // 5
  int  decomp_len;            // Actual len of decompressed data. Halo sticks garbage on the end so that the file is one of several fixed sizes (35, etc).
  int  Unknown1;
  int  TagIndexOffset;
  int  TagIndexMetaLength;
  int  Reserved1[2];          // both always 0x0
  char Name[32];
  char BuildDate[32];         // Year.Month.Day.Build - I guess they use this to make sure that a certain build will only open that build's map files, because this string is in the app too
  int  MapType;               // 0 = singleplayer, 1 = multiplayer, 2 = ui - this also determines the size of the cache file. UI = 35MB, multiplayer = 47MB, and singleplayer = 270MB
  int  Unknown4;
  int  Reserved2[HEADER_EMPTY_INTS];
  int  Footer;                // 'foot'
}MAPFILE_HDR; /* header_t */ 

typedef struct STRUCT_INDEX_HDR
{
  int   index_magic;
  int   BaseTag;
  int   unknown2;
  int   tagcount;
  int   vertex_object_count;
  UINT  ModelRawDataOffset;
  int   indices_object_count;
  UINT  indices_offset;
  int   ModelRawDataSize;
}INDEX_HDR; /* index_header_t */ 

typedef struct STRUCT_INDEX_ITEM
{
  UINT tagclass[3];
  int  tag_id;
  UINT stringoffset;
  int  offset;
  int  BitmapType; //Internal = 0 || External = 1
  int	NotUsed;
}INDEX_ITEM; /* index_item_t */ 

//Shared Bitmap file structs
typedef struct STRUCT_BITMAP_FILE_HDR
{
  UINT unk1;
  UINT NameBlockOffset;
  UINT LocatorBlockOffset;
  UINT LocatorBlockCount;
}BITMAP_FILE_HDR;

typedef struct STRUCT_BITMAP_LOCATOR_ELEMENT
{
  UINT name_offset;
  UINT size;
  UINT offset;
}BITMAP_LOCATOR_ELEMENT;

typedef enum ENUM_EDIT_MODE
{
  TRANSLATE_X = 0,
  TRANSLATE_Y,
  TRANSLATE_Z,
  ROTATE_PITCH,
  ROTATE_ROLL,
  ROTATE_YAW
}EDIT_MODE;

#define SELECTION_MODE_MAX  13
typedef enum ENUM_SELECTION_MODE
{
  NONE = 0,
  BSP_MESH_INFO,
  VEHICLES,
  SCENERY,
  WEAPONS,
  PLAYER_SPAWNS,
  MP_FLAGS,
  MP_EQUIPMENT,
  EQUIPMENT,
  MACHINES,
  BIPEDS,
  AI_SPAWNS,
  TRIGGERS
}SELECTION_MODE;

typedef struct STRUCT_SCNR_HEADER
{
  char unk_str1[16];
  char unk_str2[16];
  char unk_str3[16];
  REFLEXIVE SkyBox;
  int unk1;
  REFLEXIVE ChildScenarios;

	UINT unneeded1[46];
  int EditorScenarioSize;
  int unk2;
  int unk3;
  UINT pointertoindex;
	UINT unneeded2[2];
  UINT pointertoendofindex;
	UINT zero1[57];

  REFLEXIVE ObjectNames;
  REFLEXIVE Scenery;
  REFLEXIVE SceneryRef;
  REFLEXIVE Biped;
  REFLEXIVE BipedRef;
  REFLEXIVE Vehicle;
  REFLEXIVE VehicleRef;
  REFLEXIVE Equip;
  REFLEXIVE EquipRef;
  REFLEXIVE Weap;
  REFLEXIVE WeapRef;
  REFLEXIVE DeviceGroups;
  REFLEXIVE Machine;
  REFLEXIVE MachineRef;
  REFLEXIVE Control;
  REFLEXIVE ControlRef;
  REFLEXIVE LightFixture;
  REFLEXIVE LightFixtureRef;
  REFLEXIVE SoundScenery;
  REFLEXIVE SoundSceneryRef;
  REFLEXIVE Unknown1[7];
  REFLEXIVE PlayerStartingProfile;
  REFLEXIVE PlayerSpawn;
  REFLEXIVE TriggerVolumes;
  REFLEXIVE Animations;
  REFLEXIVE MultiplayerFlags;
  REFLEXIVE MpEquip;
  REFLEXIVE StartingEquip;
  REFLEXIVE BspSwitchTrigger;
  REFLEXIVE Decals;
  REFLEXIVE DecalsRef;
  REFLEXIVE DetailObjCollRef;
  REFLEXIVE Unknown3[7];
  REFLEXIVE ActorVariantRef;
  REFLEXIVE Encounters;
  //below this, structs still not confirmed
  REFLEXIVE CommandLists;
  REFLEXIVE Unknown2;
  REFLEXIVE StartingLocations;
  REFLEXIVE Platoons;
  REFLEXIVE AiConversations;
  UINT ScriptSyntaxDataSize;
  UINT Unknown4;
  REFLEXIVE ScriptCrap;        
  REFLEXIVE Commands;
  REFLEXIVE Points;
  REFLEXIVE AiAnimationRefs;
  REFLEXIVE GlobalsVerified;
  REFLEXIVE AiRecordingRefs;
  REFLEXIVE Unknown5;
  REFLEXIVE Participants;
  REFLEXIVE Lines;
  REFLEXIVE ScriptTriggers;
  REFLEXIVE VerifyCutscenes;
  REFLEXIVE VerifyCutsceneTitle;
  REFLEXIVE SourceFiles;
  REFLEXIVE CutsceneFlags;
  REFLEXIVE CutsceneCameraPoi;
  REFLEXIVE CutsceneTitles;
  REFLEXIVE Unknown6[8];
  UINT  Unknown7[2];
  REFLEXIVE StructBsp;
}SCNR_HEADER;


//
//

typedef struct STRUCT_BIPED
{
  SHORT BipedType;
  SHORT unk2;
  UINT  unk3;
  float x;
  float y;
  float z;
  float rot;
  UINT unk[24];
}BIPED;

typedef struct STRUCT_EQUIP
{
  SHORT numid;
  SHORT unk2;
  UINT  unk3;
  float x;
  float y;
  float z;
  UINT unk[5];
}EQUIP;

typedef struct STRUCT_VEHICLE_SPAWN
{
  SHORT numid;
  USHORT flag;
  UINT unknown1;
	float x;
	float y;
	float z;
	float rot;
	UINT unknown2[24];
}VEHICLE_SPAWN;

typedef struct STRUCT_PALLETTE
{
  char tag[4];
  UINT NamePtr;
  UINT unk1;
  UINT ObjectTagId;
  UINT unk[8];
}PALLETTE;

typedef struct STRUCT_WEAPON_SPAWN
{
  SHORT numid;
  USHORT flag;
  UINT   unknown1;
	float  x;
	float  y;
	float  z;
	float  rot;
	UINT   unknown2[17];
}WEAPON_SPAWN;

typedef struct STRUCT_WEAPON_REF
{
  SHORT numid;
  USHORT flag;
  UINT   unknown1;
	float  x;
	float  y;
	float  z;
	float  rot;
	UINT   unknown2[17];
}WEAPON_REF;


typedef struct STRUCT_SCENERY_SPAWN
{
  SHORT numid;
  USHORT flag;
  UINT   unknown1;
	float  x;
	float  y;
	float  z;
	float  rot;
  float  unk1[2];
	UINT   unknown2[10];
}SCENERY_SPAWN;

typedef struct STRUCT_MACHINE
{
  SHORT MachineType;
  SHORT unk2;
  UINT  unk3;
  float x;
  float y;
  float z;
  float rot;
  UINT unk[10];
}MACHINE;

typedef struct STRUCT_SOUND_SCENERY
{
  SHORT SoundType;
  SHORT unk2;
  UINT  unk3;
  float x;
  float y;
  float z;
  UINT unk4[5];
}SOUND_SCENERY;

typedef struct STRUCT_PLAYER_SPAWN
{
	float  x;
	float  y;
	float  z;
	float  rot;
	float  unknown2[9];
}PLAYER_SPAWN;

typedef struct STRUCT_PROFILE_PLACEMENT
{
	char name[32];
	float health;
	float shields;
	char primarytag[4];
	int primaryrawfilename;
	char zero1[4];
	int primaryidentifier;
	short primaryclip;
	short primarytotal;
	char secondarytag[4];
	int secondaryfilename;
	char zero2[4];
	int secondaryidentifier;
	short secondaryclip;
	short secondarytotal;
	short fraggrencount;
	short plasmagrencount;
	char zero3[20];
}PROFILE_PLACEMENT;


typedef struct STRUCT_MP_FLAG
{
  float x;
  float y;
  float z;
  float yaw;
  USHORT index1;
  USHORT index2;
  char  tag[4];
  UINT  unk2[31];
}MP_FLAG;

typedef struct STRUCT_MP_EQUIP
{
  UINT unk[16];
  float x;
  float y;
  float z;
  float yaw;
  float unk1;
  UINT NamePtr;
  UINT unk2;
  UINT ObjectTagId;
  UINT unk3[12];
}MP_EQUIP;

typedef struct STRUCT_PLAYER_STARTING_PROFILE
{
  UINT unk[26];
}PLAYER_STARTING_PROFILE;

typedef struct STRUCT_DEVICE_GROUP
{
  char unk[52];
}DEVICE_GROUP;

typedef struct STRUCT_BSP_TRIGGER
{
  UINT unk[2];
}BSP_TRIGGER;

typedef struct STRUCT_MOVE_POSITIONS
{
  char unk1[32];
  char unk2[4];
  UINT Offset;
  UINT unk[5];
}MOVE_POSITIONS;

typedef struct STRUCT_OBJECT_NAMES
{
  char name[32];
  USHORT unk1;
  USHORT unk2;
}OBJECT_NAMES;

typedef struct STRUCT_TRIGGER_VOLUMES
{
  UINT unk;
  char name[32];
  float unk2[9];
  BOUNDING_BOX box;
}TRIGGER_VOLUMES;

typedef struct STRUCT_ACTOR_VARIANT_REF
{
  char tag[4];
  UINT NamePtr;
  UINT unk;
  float unk2;
}ACTOR_VARIANT_REF;

//LOST IN SPACE
typedef struct STRUCT_AI_ANIMATION_REF
{
  char name[32];
  SHORT unk1;
  SHORT unk2;
  UINT unk[14];
}AI_ANIMATION_REF;

typedef struct STRUCT_AI_SCRIPT_REF
{
  char name[32];
  UINT unk[15];
}AI_SCRIPT_REF;

typedef struct STRUCT_AI_RECORDING_REF
{
  UINT unk1[6];
  char tag[4];
  UINT NamePtr;
  UINT unk[2];
}AI_RECORDING_REF;

typedef struct STRUCT_SCRIPT_TRIGGER
{
  UINT unk1;
  char name[32];
  float x;
  float y;
  float z;
  UINT unk[11];
}SCRIPT_TRIGGER;

typedef struct STRUCT_GLOBALS
{
  UINT unk1;
  char name[32];
  UINT unk2;
  float x;
  float y;
  float z;
  float unk3[4];
  UINT unk[9];
}GLOBALS;

typedef struct STRUCT_REFERENCES
{
  UINT unk1;
  char name[32];
  UINT unk[15];
}REFERENCES;

typedef struct STRUCT_ENCOUNTER
{
  char text[16];
  UINT unk[28];
  REFLEXIVE Squads;
  REFLEXIVE Platoons;
  REFLEXIVE FiringPositions;
  REFLEXIVE PlayerStartLocations;
}ENCOUNTER;

typedef struct STRUCT_ENC_SQUAD
{
  char name[32];
  SHORT ActorType;
  SHORT Platoon;
  SHORT InitialState;
  SHORT ReturnState;
  UINT  unk1[11];
  UINT  Ai_Attacking;
  UINT  Ai_AttackingSearch;
  UINT  Ai_AttackingGuard;
  UINT  Ai_Defending;
  UINT  Ai_DefendingSearch;
  UINT  Ai_DefendingGuard;
  UINT  Ai_Pursuing;
  UINT  unk2[3];
  SHORT NormalDiffCount;
  SHORT InsaneDiffCount;
  UINT  unk3[20];
  REFLEXIVE StartLocations;
  UINT  unk4[3];
}ENC_SQUAD;

typedef struct STRUCT_ENC_SQUAD_SPAWN
{
  float x;
  float y;
  float z;
  float yaw;
  SHORT unk2[5];
  SHORT CommandList;
}ENC_SQUAD_SPAWN;

typedef struct STRUCT_ENCOUNTER_INFO
{
  char text[16];
  ENC_SQUAD *pSquads;
  ENC_SQUAD_SPAWN **ppSquadSpawns;
  REFLEXIVE Platoons;
  REFLEXIVE FiringPositions;
  REFLEXIVE PlayerStartLocations;
}ENCOUNTER_INFO;

typedef struct STRUCT_DECAL
{
  SHORT unk1;
  SHORT unk2;
  float x;
  float y;
  float z;
}DECAL;

typedef struct STRUCT_DECAL_REF
{
  char tag[4];
  UINT NamePtr;
  UINT reserved;
  UINT TagId;
}DECAL_REF;


typedef struct STRUCT_SHADER_INDEX
{
  UINT ShaderType;
  UINT ShaderIndex;
}SHADER_INDEX;

typedef struct STRUCT_SKYBOX
{
  char tag[4];
  UINT NameRef;
  UINT unk1;
  UINT TagId;
}SKYBOX;

typedef struct STRUCT_STARTING_EQUIP
{
  UINT unk[51];
}STARTING_EQUIP;

typedef struct STRUCT_CONTROL
{
  SHORT unk1;
  SHORT unk2;
  UINT  unk3;
  float x;
  float y;
  float z;
  UINT  Tag_Id;
  UINT unk[10];
}CONTROL;

typedef struct STRUCT_LIGHT_FIXTURE
{
  UINT unk[22];
}LIGHT_FIXTURE;



#define TAG_OBJECT                0x656a626f   /* 'obje' */ 
#define TAG_UNIT                  0x2          /* 'unit' */ 
#define TAG_BIPED                 0x64706962   /* 'bipd' */ 
#define TAG_VEHICLE               0x69686576   /* 'vehi' */ 
#define TAG_ITEM                  0x6d657469   /* 'item' */ 
#define TAG_WEAPON                0x70616577   /* 'weap' */ 
#define TAG_EQUIPMENT             0x70697165   /* 'eqip' */ 
#define TAG_GARBAGE               0x62726167   /* 'garb' */ 
#define TAG_PROJECTILE            0x6A6F7270   /* 'proj' */ 
#define TAG_SCENERY               0x6E656373   /* 'scen' */ 
#define TAG_SOUND_SCENERY         0x4          /* 'ssce' */ 
#define TAG_DEVICE                0x5          /* 'devi' */ 
#define TAG_MACHINE               0x6          /* 'mach' */ 
#define TAG_CONTROL               0x7          /* 'ctrl' */ 
#define TAG_LIGHT_FIXTURE         0x8          /* 'lifi' */ 
#define TAG_PLACEHOLDER           0x9          /* 'plac' */ 
#define TAG_BITMAP                0x6d746962   /* 'bitm' */ 
#define TAG_SHADER                0x72646873   /* 'shdr' */ 
#define TAG_SOUND                 0x21646e73   /* 'snd!' */ 
#define TAG_UNICODE_STR           0x72747375   /* 'ustr' */ 
#define TAG_SCENARIO              0x726E6373   /* 'scnr' */ 
#define TAG_STRUCTURED_BSP        0x70736273   /* 'sbsp' */ 
#define TAG_MULTIPLAYER_SCENARIO  0x796c706d   /* 'mply' */ 
#define TAG_ITEM_COLLECTION       0x636d7469   /* 'itmc' */ 
#define TAG_HUD_MSG_TEXT          0x20746D68   /* 'hmt ' */ 
#define TAG_CAMERA_TRACK          0x6B617274   /* 'trak' */ 
#define TAG_MODEL                 0x65646F6D   /* 'mode' */ 
#define TAG_ACTOR                 0x72746361   /* 'actr' */ 
#define TAG_ACTOR_VARIANT         0x76746361   /* 'actv' */ 
#define TAG_ANIMATION_TRIGGER     0x72746E61   /* 'antr' */ 
#define TAG_SKY                   0x20796B73   /* 'sky ' */ 
#define TAG_LENSFLARE             0x736E656C   /* 'lens' */ 
#define TAG_GRENADE_HUD_INTERFACE 0x69687267   /* 'grhi' */ 
#define TAG_UNIT_HUD_INTERFACE    0x69686E75   /* 'unhi' */ 
#define TAG_WEAPON_HUD_INTERFACE  0x69687077   /* 'wphi' */ 
#define TAG_COLLISION_MODEL       0x6C6C6F63   /* 'coll' */ 
#define TAG_CONTRAIL              0x746E6F63   /* 'cont' */ 
#define TAG_DECAL                 0x61636564   /* 'deca' */ 
#define TAG_EFFECT                0x65666665   /* 'effe' */ 
#define TAG_PARTICLE              0x74726170   /* 'part' */ 
#define TAG_PARTICLE_SYSTEM       0x6C746370   /* 'pctl' */ 
#define TAG_WEATHER_PARTICLE      0x13         /* 'rain' */ 
#define TAG_GAME_GLOBALS          0x6774616D   /* 'matg' */ 
#define TAG_HUD_NUMBER            0x23647568   /* 'hud#' */ 
#define TAG_HUD_GLOBALS           0x67647568   /* 'hudg' */ 
#define TAG_DAMAGE                0x2174706A   /* 'jpt!' */ 
#define TAG_LIGHT                 0x6867696C   /* 'ligh' */ 
#define TAG_GLOW                  0x14         /* 'glw!' */ 
#define TAG_LIGHT_VOLUME          0x3273676D   /* 'mgs2' */ 
#define TAG_LOOPING_SOUND         0x646E736C   /* 'lsnd' */ 
#define TAG_SOUND_ENVIRONMENT     0x65646E73   /* 'snde' */ 
#define TAG_PHYSICS               0x79687070   /* 'pphy' */ 
#define TAG_POINT_PHYSICS         0x79687070   /* 'pphy' */ 
#define TAG_FOG                   0x20676F66   /* 'fog ' */ 
#define TAG_WIND                  0x646E6977   /* 'wind' */ 
#define TAG_SHADER_ENVIRONMENT    0x766E6573   /* 'senv' */ 
#define TAG_DETAIL_OBJ_COLLECTION 0x63626F64   /* 'dobc' */ 
#define TAG_FONT                  0x746E6F66   /* 'font' */ 
#define TAG_DIALOG                0x676C6475   /* 'udlg' */ 
#define TAG_UI_ITEM               0x614C6544   /* 'DeLa' */ 
#define TAG_UI_ITEM_COLLECTION    0x6C756F53   /* 'Soul' */ 
#define TAG_SHADER_MODEL          0x6F736F73   /* 'soso' */ 
#define TAG_SHADER_MODEL_LE       0x6F736F73
#define TAG_SHADER_TRANSPARENCY   0x72746F73   /* 'sotr' */ 
#define TAG_SHADER_WATER          0x74617773   /* 'swat' */ 
#define TAG_SHADER_GLASS          0x616C6773   /* 'sgla' */ 
#define TAG_SHADER_METAL          0x74656D73   /* 'smet' */ 
#define TAG_SHADER_PLASMA         0x616C7073   /* 'spla' */ 
#define TAG_ANTENNA               0x21746E61   /* 'ant!' */ 
#define TAG_STRING_LIST           0x23727473   /* 'str#' */ 
#define TAG_COLOR_GROUP           0x6F6C6F63   /* 'colo' */ 
#define TAG_LIGHTNING             0x15         /* 'elec' */ 
#define TAG_METER                 0x7274656D   /* 'metr' */ 
#define TAG_SHADER_TRANSP_VARIANT 0x69686373   /* 'schi' */ 
#define TAG_FLAG                  0x16         /* 'flag' */ 
#define TAG_MATERIAL_EFFECT       0x746F6F66   /* 'foot' */ 
#define TAG_VIRTUAL_KEYBOARD      0x17         /* 'vcky' */ 

#define TAG_MODEL2                0x32646F6D

#endif