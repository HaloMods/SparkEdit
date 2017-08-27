#ifndef __PCMODEL_H__
#define __PCMODEL_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Util.h"
#include "Model.h"

typedef struct STRUCT_PCMODEL_SUBMESH_HEADER
{
  UINT Flags;
  SHORT ShaderIndex;
  SHORT unk2[3];
  float unk4b[2];
  float  unk4c;
  float unk4d[2];
  UINT  unk5a[4];
  UINT  SubObjectCount;
  UINT  unk5b[4];
  UINT  unkCount;
  int   IndexCount;
  UINT  IndexOffset;   //*verified
  UINT  IndexOffset2;  //points to another offset
  UINT  count2;
  UINT  VertexCount;
  UINT  zero1;
  UINT  RawOffsetBehavior;
  UINT  VertexRefOffset;
  UINT  unk[7];
}PCMODEL_SUBMESH_HEADER;

typedef struct STRUCT_PCMODEL_VERT
{
  float coord[3];
  float Normal[3];
  float Binormal[3];
  float Tangent[3];
  float u;
  float v;
  float unk2[3];
}PCMODEL_VERT;

typedef struct STRUCT_PCMODEL_SUBMESH_INFO
{
  MODEL_GEOMETRY_HEADER header;
  PCMODEL_SUBMESH_HEADER *pSubmeshes;
  PCMODEL_VERT **ppVert;
  USHORT **ppIndex;
  UINT ShaderType;
  UINT ShaderIndex;
  int RenderTextureIndex;
}PCMODEL_SUBMESH_INFO;


typedef struct STRUCT_PCMODEL_MODEL
{
  char name1[64];
  REFLEXIVE chunk1;
  char name2[64];
  SHORT unk1[2];
  UINT  unk2[2];
}PCMODEL_MODEL;

typedef struct STRUCT_PCMODEL_BONE
{
  char name[32];
  SHORT NextSiblingNode;
  SHORT NextChildNode;
  SHORT ParentNode;
  SHORT unk1[1];
  float Translation[3];
  float Rotation[4];
  float DistanceFromParent;
  float unk2[3][7];
}PCMODEL_BONE;

class CPcModel : public CModel  
{
public:

  CString GetMarkerName(int index);
  int GetMarkerCount(void);
  
  void DrawMarkers(int active_index);
	void DrawNodes(void);
	void DrawModel(float *pCoord, MODEL_LOD lod);
	void LoadModel(UINT model_offset, UINT size);
	void Cleanup(void);
	void Initialize(CFile *pMapFile, UINT magic, UINT VertOffset, UINT IndexOffset, CString name);
	CPcModel();
	virtual ~CPcModel();
  UINT m_TagId;

protected:
	void DrawSubmeshLines(PCMODEL_SUBMESH_INFO *pMesh);
	void DrawTexturedSubmesh(PCMODEL_SUBMESH_INFO *pMesh);
	void LoadModels(void);
	void LoadBones(void);
	void LoadSubmodels(PCMODEL_SUBMESH_INFO *pMesh);
	void LoadMeshes(void);

  UINT m_IndexOffset;
  UINT m_VertOffset;

  PCMODEL_MODEL *m_pModels;
  PCMODEL_SUBMESH_INFO *m_pMesh;
  PCMODEL_BONE *m_pBones;

  int m_TriCount;
};

#endif
