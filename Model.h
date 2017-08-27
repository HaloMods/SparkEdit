#if !defined(AFX_MODEL_H__6D40F497_E9C9_44A8_9166_A22C4BA02E87__INCLUDED_)
#define AFX_MODEL_H__6D40F497_E9C9_44A8_9166_A22C4BA02E87__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Util.h"
#include "HaloStructDefs.h"

typedef struct STRUCT_MODEL_HEADER
{
  UINT  Zero1;
  UINT  unk1a;
  UINT  Offset1;
  UINT  Offset2;
  UINT  Offset3;
  UINT  Offset4;
  UINT  Offset5;
  SHORT SuperHighLodCutoff;
  SHORT HighLodCutoff;
  SHORT MediumLodCutoff;
  SHORT LowLodCutoff;
  SHORT SuperLowLodCutoff;
  SHORT SuperHighLodNodeCount;
  SHORT HighLodNodeCount;
  SHORT MediumLodNodeCount;
  SHORT LowLodNodeCount;
  SHORT SuperLowLodNodeCount;
  float u_scale;
  float v_scale;
  UINT  unk3[29];
  REFLEXIVE Markers;
  REFLEXIVE Nodes;
  REFLEXIVE Regions;
  REFLEXIVE Geometries;
  REFLEXIVE Shaders;
}MODEL_HEADER;

typedef struct STRUCT_SHADER_DESCRIPTION
{
  UINT tag;
  UINT NamePtr;
  UINT zero;
  UINT Shader_TagId;
  UINT unk[4];
}SHADER_DESCRIPTION;

typedef struct STRUCT_MODEL_REGION
{
  char Name[64];
  REFLEXIVE Permutations;
}MODEL_REGION;

typedef enum ENUM_MODEL_LOD
{
  SUPER_LOW = 0,
  LOW,
  MEDIUM,
  HIGH,
  SUPER_HIGH
}MODEL_LOD;

typedef struct STRUCT_MODEL_REGION_PERMUTATION
{
  char Name[32];
  UINT Flags[8];
  SHORT LOD_MeshIndex[5];
  SHORT Reserved[7];
}MODEL_REGION_PERMUTATION;


typedef struct STRUCT_MODEL_GEOMETRY_HEADER
{
  UINT unk[9];
  REFLEXIVE SubmeshHeaders;
}MODEL_GEOMETRY_HEADER;

typedef struct STRUCT_MODEL_MARKER
{
  char name1[32];
  UINT unk[5];
  REFLEXIVE chunk1;
}MODEL_MARKER;

typedef struct STRUCT_MODEL_MARKER_INST
{
  SHORT name;
  SHORT node_index;
  float translation[3];
  float rotation[4];
}MODEL_MARKER_INST;

typedef struct STRUCT_MODEL_REPORT
{
  UINT IndexOffsetStart;
  UINT IndexOffsetEnd;
  UINT VertexOffsetStart;
  UINT VertexOffsetEnd;
}MODEL_REPORT;


class CModel : public CUtil  
{
public:
	int GetMarkerCount(void);
	CString GetMarkerName(int index);
	void UpdateBoundingRadius(void);
  void CompileShader();
  void UpdateBoundingBox(float *pCoord);
  void GetBoundingBox(BOUNDING_BOX *pBox);
	void LoadBones(void);
	void DrawSolidBoundingBox(float *pColor);
	void DrawBoundingBox(float *pColor);
	void LoadModel(UINT model_offset, UINT size);
	void Cleanup(void);
	void Initialize(CFile *pMapFile, UINT magic, CString name);
	CModel();
	virtual ~CModel();
  CString GetDescription();
  float m_BoundingRadius;
  CString m_ModelName;
  MODEL_REPORT m_Report;

protected:
	void LoadRegions(void);
	void LoadMarkers(void);
	void LoadShaders(void);
  CFile *m_pMapFile;
  UINT m_Magic;
  MODEL_HEADER m_Header;

  int *m_pRenderTextureIndex;
  SHADER_DESCRIPTION *m_pShaderDesc;
  MODEL_REGION *m_pRegions;
  SHADER_INDEX *m_pShaderIndex;
  BOUNDING_BOX m_BoundingBox;
  MODEL_REGION_PERMUTATION **m_ppPermutations;
  MODEL_MARKER_INST **m_ppMarkerList;
  MODEL_MARKER *m_pMarkers;
  CStringArray m_ShaderNames;
  int m_TriCount;
};

#endif // !defined(AFX_MODEL_H__6D40F497_E9C9_44A8_9166_A22C4BA02E87__INCLUDED_)
