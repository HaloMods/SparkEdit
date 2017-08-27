// LightwaveImporter.h: interface for the CLightwaveImporter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIGHTWAVEIMPORTER_H__B79CCE6A_195B_47AD_91FE_7F684CA2CDB2__INCLUDED_)
#define AFX_LIGHTWAVEIMPORTER_H__B79CCE6A_195B_47AD_91FE_7F684CA2CDB2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VisibleBsp.h"

#define OBJ_VERT_MAX   20000//0xFFFF
#define OBJ_INDEX_MAX  20000//0xFFFF

typedef struct
{
  UINT StartPolygon;
  UINT EndPolygon;
  UINT PortalIndexList[50];
  UINT NumPortals;
  UINT PvsIndex;
  UINT BoundingBox;
}BSP_LEAF;

typedef struct
{
  UINT IsLeaf;
  UINT PlaneIndex;
  UINT FrontNode;
  int  BackNode;
  UINT BoundingBox;
}BSP_NODE;

typedef struct
{
  float PointOnPlane[3];
  float Normal[3];
}BSP_PLANE;



typedef struct
{
  float vector[3];
  UINT index;
}VERTEX_UNK;

typedef struct
{
  float vector[3];
}VERT_NORM;

typedef struct
{
  float normal[3];
  float unknown;
}NORMAL_UNK;


typedef struct
{
  float unk1[3];
  USHORT unk2[4];
}BSP1;



typedef struct STRUCT_OBJ_SUBMESH_INFO
{
  UINT     VertCount;
  COMPRESSED_BSP_VERT *pVert;
  UINT     IndexCount;
  TRI_INDICES *pIndex;
}OBJ_SUBMESH_INFO;

typedef struct STRUCT_BSP_COLL_INDEX
{
  UINT CollisionIndex;
  UINT VisibleMeshIndex;
  UINT VisibleVertexIndex;
}BSP_COLL_INDEX;

#define SUBMESH_COUNT 1000

class CLightwaveImporter  
{
public:
	void UpdateCollisionMesh(void);
	void SaveCollisionVertFormat(CString filename);
	void LoadImportedDataIntoBsp(void);
	BOOL VerifyAgainstLoadedMap(void);
	void ReadTriangleIndex(CString str, USHORT *pIndex);
	void UpdateXboxSubmeshList(void);
	void Cleanup(void);
	void ParseObjFile(void);
	CLightwaveImporter();
	virtual ~CLightwaveImporter();
  void OpenObjFile(CString path);

protected:
	BOOL DoVertsMatch(float *pV1, float *pV2);
	void ReadVertex(CString str, float *pVert);
  CStdioFile m_ObjFile;

  float  m_CurrentVerts[OBJ_VERT_MAX][3];
  USHORT m_CurrentIndices[OBJ_INDEX_MAX][3];

  UINT m_CurrentVertCount;
  UINT m_CurrentIndicesCount;
  UINT m_CurrentMeshCount;
  BOOL m_bFirstPass;

  OBJ_SUBMESH_INFO m_Mesh[SUBMESH_COUNT];
};

#endif // !defined(AFX_LIGHTWAVEIMPORTER_H__B79CCE6A_195B_47AD_91FE_7F684CA2CDB2__INCLUDED_)
