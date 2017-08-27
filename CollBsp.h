// CollBsp1.h: interface for the CCollBsp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLLBSP1_H__AE41C2CC_D1EA_4BE0_AE25_0668C520B99D__INCLUDED_)
#define AFX_COLLBSP1_H__AE41C2CC_D1EA_4BE0_AE25_0668C520B99D__INCLUDED_

#include "bsp_defs.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCollBsp  
{
public:
	UINT WriteCollisionBsp(CFile *pFile, UINT start_offset, UINT new_magic);
	void Cleanup(void);
	void LoadCollisionBsp(UINT CollHdrOffset);
	void Initialize(CFile *pMapFile, UINT bspmagic, UINT magic);
	CCollBsp();
	virtual ~CCollBsp();
  COLLISION_BSP_HEADER m_CollHeader;
protected:
  CFile *m_pMapFile;
  UINT m_BspMagic;
  UINT m_Magic;


  COLL_NODE *m_pNodes;
  COLL_PLANE *m_pPlanes;
  COLL_LEAF *m_pLeaves;
  COLL_BSP_2D_REF *m_pBsp2D_Ref;
  COLL_BSP_2D_NODES *m_pBsp2D_Nodes;
  COLL_SURFACES *m_pSurfaces;
  COLL_EDGES *m_pEdges;
  COLL_VERTEX *m_pVertices;
};

#endif // !defined(AFX_COLLBSP1_H__AE41C2CC_D1EA_4BE0_AE25_0668C520B99D__INCLUDED_)
