#ifndef __XBOXMODEL_H__
#define __XBOXMODEL_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Util.h"
#include "Model.h"


typedef struct STRUCT_XBOX_MODEL_VERTEX
{
  UINT unk[8];
}XBOX_MODEL_VERTEX;

typedef struct STRUCT_MODEL_SUBMODEL_HEADER
{
  UINT  Flags;
  SHORT ShaderIndex;
  SHORT PrevFilthyPart;//?
  SHORT NextFilthyPart;//?
  SHORT CentroidPrimaryNode;//?
  float CentroidPrimaryWeight;//?
  float CentroidSecondaryWeight;//?
  UINT  unk4c;
  UINT  unk4d[2];
  UINT  unk5a[4];
  UINT  SubObjectCount;
  UINT  unk5b[5];
  int   IndexCount;
  UINT  IndexOffset;   //*verified
  UINT  IndexOffset2;  //points to another offset
  UINT  count2;
  UINT  VertexCount;
  UINT  zero1;
  UINT  RawOffsetBehavior;
  UINT  VertexRefOffset;
}MODEL_SUBMODEL_HEADER;

typedef struct STRUCT_MODEL_COMPRESSED_VERT
{
  float coord[3];
  UINT  CompNormal;
  UINT  CompBinormal;
  UINT  CompTangent;
  SHORT u;
  SHORT v;
  SHORT unk1;
  SHORT node0_weight;
}MODEL_COMPRESSED_VERT;

typedef struct STRUCT_MODEL_UV
{
  float u;
  float v;
}MODEL_UV;

typedef struct STRUCT_MODEL_SUBMESH_INFO
{
  MODEL_GEOMETRY_HEADER header;
  MODEL_SUBMODEL_HEADER *pSubmeshes;
  MODEL_COMPRESSED_VERT **ppCmpVert;
  MODEL_UV **ppTexVert;
  USHORT **ppIndex;
}XBOX_MODEL_SUBMESH_INFO;

typedef struct STRUCT_MODEL_VERT_REFERENCE
{
  UINT unk[3];
}MODEL_VERT_REFERENCE;

class CXboxModel : public CModel
{
public:
	void DrawModel(float *pCoord, MODEL_LOD lod);
	void LoadModel(UINT model_offset, UINT size);
	void Cleanup(void);
	CXboxModel();
	virtual ~CXboxModel();
  UINT m_NamePtr;

protected:
  void LoadMeshes(void);
	void DrawTexturedSubmesh(XBOX_MODEL_SUBMESH_INFO *pMesh);
	void LoadSubmodels(XBOX_MODEL_SUBMESH_INFO *pMesh);

  XBOX_MODEL_SUBMESH_INFO *m_pMesh;

  int m_TriCount;
};

#endif // __XBOXMODEL_H__

