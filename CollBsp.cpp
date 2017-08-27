// CollBsp1.cpp: implementation of the CCollBsp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sparkedit.h"
#include "CollBsp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCollBsp::CCollBsp()
{
  m_pMapFile = NULL;
  m_BspMagic = 0;
  m_Magic = 0;

  m_pNodes = NULL;
  m_pPlanes = NULL;
  m_pLeaves = NULL;
  m_pBsp2D_Ref = NULL;
  m_pBsp2D_Nodes = NULL;
  m_pSurfaces = NULL;
  m_pEdges = NULL;
  m_pVertices = NULL;
}

CCollBsp::~CCollBsp()
{

}

void CCollBsp::Initialize(CFile *pMapFile, UINT bspmagic, UINT magic)
{
  m_pMapFile = pMapFile;
  m_BspMagic = bspmagic;
  m_Magic = magic;
}

void CCollBsp::LoadCollisionBsp(UINT CollHdrOffset)
{
  m_pMapFile->Seek(CollHdrOffset, 0);
  m_pMapFile->Read(&m_CollHeader, sizeof(m_CollHeader));

  m_CollHeader.Nodes.Offset -= m_BspMagic;
  m_CollHeader.Planes.Offset -= m_BspMagic;
  m_CollHeader.Leaves.Offset -= m_BspMagic;
  m_CollHeader.Bsp2dRef.Offset -= m_BspMagic;
  m_CollHeader.Bsp2dNodes.Offset -= m_BspMagic;
  m_CollHeader.Surfaces.Offset -= m_BspMagic;
  m_CollHeader.Edges.Offset -= m_BspMagic;
  m_CollHeader.Vertices.Offset -= m_BspMagic;

  if(m_CollHeader.Nodes.Count > 0)
  {
    m_pNodes = new COLL_NODE[m_CollHeader.Nodes.Count];
    m_pMapFile->Seek(m_CollHeader.Nodes.Offset, 0);
    m_pMapFile->Read(m_pNodes, sizeof(COLL_NODE)*m_CollHeader.Nodes.Count);
  }

  if(m_CollHeader.Planes.Count > 0)
  {
    m_pPlanes = new COLL_PLANE[m_CollHeader.Planes.Count];
    m_pMapFile->Seek(m_CollHeader.Planes.Offset, 0);
    m_pMapFile->Read(m_pPlanes, sizeof(COLL_PLANE)*m_CollHeader.Planes.Count);
  }

  if(m_CollHeader.Leaves.Count > 0)
  {
    m_pLeaves = new COLL_LEAF[m_CollHeader.Leaves.Count];
    m_pMapFile->Seek(m_CollHeader.Leaves.Offset, 0);
    m_pMapFile->Read(m_pLeaves, sizeof(COLL_LEAF)*m_CollHeader.Leaves.Count);
  }

  if(m_CollHeader.Bsp2dRef.Count > 0)
  {
    m_pBsp2D_Ref = new COLL_BSP_2D_REF[m_CollHeader.Bsp2dRef.Count];
    m_pMapFile->Seek(m_CollHeader.Bsp2dRef.Offset, 0);
    m_pMapFile->Read(m_pBsp2D_Ref, sizeof(COLL_BSP_2D_REF)*m_CollHeader.Bsp2dRef.Count);
  }

  if(m_CollHeader.Bsp2dNodes.Count > 0)
  {
    m_pBsp2D_Nodes = new COLL_BSP_2D_NODES[m_CollHeader.Bsp2dNodes.Count];
    m_pMapFile->Seek(m_CollHeader.Bsp2dNodes.Offset, 0);
    m_pMapFile->Read(m_pBsp2D_Nodes, sizeof(COLL_BSP_2D_NODES)*m_CollHeader.Bsp2dNodes.Count);
  }

  if(m_CollHeader.Surfaces.Count > 0)
  {
    m_pSurfaces = new COLL_SURFACES[m_CollHeader.Surfaces.Count];
    m_pMapFile->Seek(m_CollHeader.Surfaces.Offset, 0);
    m_pMapFile->Read(m_pSurfaces, sizeof(COLL_SURFACES)*m_CollHeader.Surfaces.Count);
  }

  if(m_CollHeader.Edges.Count > 0)
  {
    m_pEdges = new COLL_EDGES[m_CollHeader.Edges.Count];
    m_pMapFile->Seek(m_CollHeader.Edges.Offset, 0);
    m_pMapFile->Read(m_pEdges, sizeof(COLL_EDGES)*m_CollHeader.Edges.Count);
  }

  if(m_CollHeader.Vertices.Count > 0)
  {
    m_pVertices = new COLL_VERTEX[m_CollHeader.Vertices.Count];
    m_pMapFile->Seek(m_CollHeader.Vertices.Offset, 0);
    m_pMapFile->Read(m_pVertices, sizeof(COLL_VERTEX)*m_CollHeader.Vertices.Count);
  }
}

void CCollBsp::Cleanup()
{
  if(m_pNodes)
    delete [] m_pNodes;

  if(m_pPlanes)
    delete [] m_pPlanes;

  if(m_pLeaves)
    delete [] m_pLeaves;

  if(m_pBsp2D_Ref)
    delete [] m_pBsp2D_Ref;

  if(m_pBsp2D_Nodes)
    delete [] m_pBsp2D_Nodes;

  if(m_pSurfaces)
    delete [] m_pSurfaces;

  if(m_pEdges)
    delete [] m_pEdges;

  if(m_pVertices)
    delete [] m_pVertices;

  m_pNodes = NULL;
  m_pPlanes = NULL;
  m_pLeaves = NULL;
  m_pBsp2D_Ref = NULL;
  m_pBsp2D_Nodes = NULL;
  m_pSurfaces = NULL;
  m_pEdges = NULL;
  m_pVertices = NULL;
}

UINT CCollBsp::WriteCollisionBsp(CFile *pFile, UINT start_offset, UINT new_magic)
{
  UINT bytes_written = 0;
  UINT block_size;
  COLLISION_BSP_HEADER new_hdr;

  memcpy(&new_hdr, &m_CollHeader, sizeof(COLLISION_BSP_HEADER));

  //write header
  pFile->Seek(start_offset, 0);
  pFile->Write(&new_hdr, sizeof(COLLISION_BSP_HEADER));
  bytes_written += sizeof(COLLISION_BSP_HEADER);

  //write nodes
  new_hdr.Nodes.Offset = bytes_written + start_offset + new_magic;
  block_size = sizeof(COLL_NODE)*m_CollHeader.Nodes.Count;
  pFile->Write(m_pNodes, block_size);
  bytes_written += block_size;

  //write planes
  new_hdr.Planes.Offset = bytes_written + start_offset + new_magic;
  block_size = sizeof(COLL_PLANE)*m_CollHeader.Planes.Count;
  pFile->Write(m_pPlanes, block_size);
  bytes_written += block_size;

  //write leaves
  new_hdr.Leaves.Offset = bytes_written + start_offset + new_magic;
  block_size = sizeof(COLL_LEAF)*m_CollHeader.Leaves.Count;
  pFile->Write(m_pLeaves, block_size);
  bytes_written += block_size;

  //write bsp 2d ref
  new_hdr.Bsp2dRef.Offset = bytes_written + start_offset + new_magic;
  block_size = sizeof(COLL_BSP_2D_REF)*m_CollHeader.Bsp2dRef.Count;
  pFile->Write(m_pBsp2D_Ref, block_size);
  bytes_written += block_size;

  //write bsp 2d nodes
  new_hdr.Bsp2dNodes.Offset = bytes_written + start_offset + new_magic;
  block_size = sizeof(COLL_BSP_2D_NODES)*m_CollHeader.Bsp2dNodes.Count;
  pFile->Write(m_pBsp2D_Nodes, block_size);
  bytes_written += block_size;

  //write surfaces
  new_hdr.Surfaces.Offset = bytes_written + start_offset + new_magic;
  block_size = sizeof(COLL_SURFACES)*m_CollHeader.Surfaces.Count;
  pFile->Write(m_pSurfaces, block_size);
  bytes_written += block_size;

  //write edges
  new_hdr.Edges.Offset = bytes_written + start_offset + new_magic;
  block_size = sizeof(COLL_EDGES)*m_CollHeader.Edges.Count;
  pFile->Write(m_pEdges, block_size);
  bytes_written += block_size;

  //write vertices
  new_hdr.Vertices.Offset = bytes_written + start_offset + new_magic;
  block_size = sizeof(COLL_VERTEX)*m_CollHeader.Vertices.Count;
  pFile->Write(m_pVertices, block_size);
  bytes_written += block_size;

  //update collision header with new offsets
  pFile->Seek(start_offset, 0);
  pFile->Write(&new_hdr, sizeof(COLLISION_BSP_HEADER));


  return(bytes_written);
}
