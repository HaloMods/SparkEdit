// VisibleBsp.cpp: implementation of the CVisibleBsp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sparkedit.h"
#include "VisibleBsp.h"
#include "OutputPane.h"
#include "ShaderManager.h"
#include "TagManager.h"
#include "HaloMapFile.h"
#include "BspManager.h"

extern COutputPane *g_pOutput;
extern CShaderManager gShaderManager;
extern CTagManager gTagManager;
extern CHaloMapFile HaloMap;
extern CBspManager gBspManager;


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*-------------------------------------------------------------------
 * Name: CVisibleBsp()
 * Description:
 *   
 *-----------------------------------------------------------------*/
CVisibleBsp::CVisibleBsp()
{
  m_pMapFile = NULL;
  m_BspMagic = 0;
  m_Magic = 0;
  m_SubMeshCount = 0;
  m_ActiveBsp = 0;
  m_pMesh = 0;
  m_pWeather = NULL;
  m_pClusters = NULL;
  m_pLightmaps = NULL;
  m_pShaders = NULL;
  m_pNodes = NULL;
  m_pLeaves = NULL;
  m_pLeafSurfaces = NULL;
  m_pTriIndices = NULL;
  m_pClusterData = NULL;
  m_pPathfindingSurface = NULL;
  m_pBackgroundSounds = NULL;
  m_pSoundEnv = NULL;
  m_pPASData = NULL;
  m_pDetailObjects = NULL;
  m_pRuntimeDecals = NULL;


  m_Centroid[0] = 0;
  m_Centroid[1] = 0;
  m_Centroid[2] = 0;
  m_CentroidCount = 0;
  m_TriTotal = 0;

  m_MapBox.min[0] = 40000;
  m_MapBox.min[1] = 40000;
  m_MapBox.min[2] = 40000;
  m_MapBox.max[0] = -40000;
  m_MapBox.max[1] = -40000;
  m_MapBox.max[2] = -40000;
}

/*-------------------------------------------------------------------
 * Name: ~CVisibleBsp()
 * Description:
 *   
 *-----------------------------------------------------------------*/
CVisibleBsp::~CVisibleBsp()
{
  Cleanup();
}

/*-------------------------------------------------------------------
 * Name: Cleanup()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CVisibleBsp::Cleanup(void)
{
  BSP_SUBCLUSTER* pSubCluster;
  BSP_CLUSTER* pClusters;
  BSP_CLUSTER_PORTAL* pClusterPortals;
  int i;

  if(m_pMesh)
  {
    for(i=0; i<m_SubMeshCount; i++)
    {
      if(m_pMesh[i].pCompVert)
        delete [] m_pMesh[i].pCompVert;

      if(m_pMesh[i].pVert)
        delete [] m_pMesh[i].pVert;
      
      if(m_pMesh[i].pIndex)
        delete [] m_pMesh[i].pIndex;
      
      if(m_pMesh[i].pTextureData)
        delete [] m_pMesh[i].pTextureData;

      if(m_pMesh[i].pLightmapVert)
        delete [] m_pMesh[i].pLightmapVert;

      if(m_pMesh[i].pCompLightmapVert)
        delete [] m_pMesh[i].pCompLightmapVert;
    }
    
    delete [] m_pMesh;
    m_pMesh = 0;
  }

  if(m_pLightmaps)
    delete [] m_pLightmaps;
  m_pLightmaps = 0;

  if(m_pWeather)
    delete [] m_pWeather;
  m_pWeather = NULL;

  if(m_pClusters)
    delete [] m_pClusters;
  m_pClusters = NULL;

  if(m_pShaders)
    delete [] m_pShaders;
  m_pShaders = NULL;
  
  if(m_pNodes)
    delete [] m_pNodes;
  m_pNodes = NULL;
  
  if(m_pLeaves)
    delete [] m_pLeaves;
  m_pLeaves = NULL;
  
  if(m_pLeafSurfaces)
    delete [] m_pLeafSurfaces;
  m_pLeafSurfaces = NULL;
  
  if(m_pTriIndices)
    delete [] m_pTriIndices;
  m_pTriIndices = NULL;

  pClusters = (BSP_CLUSTER*)m_BspHeader.Clusters.unknown;
  for(int c=0; c<m_BspHeader.Clusters.Count; c++)
  {
    if(pClusters[c].PredictedResources.unknown)
      delete [] pClusters[c].PredictedResources.unknown;
    pClusters[c].PredictedResources.unknown = NULL;

    pSubCluster = (BSP_SUBCLUSTER*)pClusters[c].SubCluster.unknown;
    for(int sc=0; sc<pClusters[c].SubCluster.Count; sc++)
    {
      if(pSubCluster[sc].SurfaceIndices.unknown)
        delete [] pSubCluster[sc].SurfaceIndices.unknown;
      pSubCluster[sc].SurfaceIndices.unknown = NULL;
    }

    if(pClusters[c].SubCluster.unknown)
      delete [] pClusters[c].SubCluster.unknown;
    pClusters[c].SubCluster.unknown = NULL;

    if(pClusters[c].Portals.unknown)
      delete [] pClusters[c].Portals.unknown;
    pClusters[c].Portals.unknown = NULL;
  }
  if(m_BspHeader.Clusters.unknown)
    delete [] m_BspHeader.Clusters.unknown;
  m_BspHeader.Clusters.unknown = NULL;

  if(m_pClusterData)
    delete [] m_pClusterData;
  m_pClusterData = NULL;

  pClusterPortals = (BSP_CLUSTER_PORTAL*)m_BspHeader.ClusterPortals.unknown;
  for(int p=0; p<m_BspHeader.ClusterPortals.Count; p++)
  {
    if(pClusterPortals[p].Vertices.unknown)
      delete [] pClusterPortals[p].Vertices.unknown;
  }

  if(m_BspHeader.ClusterPortals.unknown)
    delete [] m_BspHeader.ClusterPortals.unknown;
  m_BspHeader.ClusterPortals.unknown = NULL;

  if(m_pPathfindingSurface)
    delete [] m_pPathfindingSurface;
  m_pPathfindingSurface = NULL;

  if(m_pBackgroundSounds)
    delete [] m_pBackgroundSounds;
  m_pBackgroundSounds = NULL;

  if(m_pSoundEnv)
    delete [] m_pSoundEnv;
  m_pSoundEnv = NULL;

  if(m_pPASData)
    delete [] m_pPASData;
  m_pPASData = NULL;

  if(m_pDetailObjects)
    delete [] m_pDetailObjects;
  m_pDetailObjects = NULL;

  if(m_pRuntimeDecals)
    delete [] m_pRuntimeDecals;
  m_pRuntimeDecals = NULL;




  m_CollBsp.Cleanup();

  m_pMapFile = NULL;
  m_BspMagic = 0;
  m_Magic = 0;
  m_SubMeshCount = 0;
}

/*-------------------------------------------------------------------
 * Name: LoadVisibleBsp()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CVisibleBsp::LoadVisibleBsp(UINT BspHdrOffset, UINT version)
{
//  if(version == 5)
//    LoadReflectionBlock();
  
  // read the header
  m_pMapFile->Seek(BspHdrOffset, 0);
  m_pMapFile->Read(&m_BspHeader, sizeof(m_BspHeader));

  //UINT i = (UINT)&m_BspHeader.RuntimeDecals.Count - (UINT)&m_BspHeader;
  //m_pMapFile->Seek(BspHdrOffset, 0);
  //m_pMapFile->Write(&m_BspHeader, sizeof(m_BspHeader));

  m_BspHeader.LightmapsTag.NamePtr -= m_Magic;
  m_BspHeader.Shaders.Offset -=  m_BspMagic;
  m_BspHeader.CollBspHeader.Offset -= m_BspMagic;
  m_BspHeader.Nodes.Offset -= m_BspMagic;
  m_BspHeader.Leaves.Offset -= m_BspMagic;
  m_BspHeader.LeafSurfaces.Offset -= m_BspMagic;
  m_BspHeader.SubmeshTriIndices.Offset -= m_BspMagic;
  m_BspHeader.SubmeshHeader.Offset -= m_BspMagic;
  m_BspHeader.Chunk10.Offset -= m_BspMagic;
  m_BspHeader.Chunk11.Offset -= m_BspMagic;
  m_BspHeader.Chunk12.Offset -= m_BspMagic;
  m_BspHeader.Clusters.Offset -= m_BspMagic;
  m_BspHeader.ClusterData.Offset -= m_BspMagic;
  m_BspHeader.ClusterPortals.Offset -= m_BspMagic;
  //m_BspHeader.Chunk16a.Offset -= m_BspMagic;
  //m_BspHeader.Chunk16b.Offset -= m_BspMagic;
  //m_BspHeader.Chunk16c.Offset -= m_BspMagic;
  //m_BspHeader.Chunk16d.Offset -= m_BspMagic;
  //m_BspHeader.Chunk16e.Offset -= m_BspMagic;
  //m_BspHeader.Chunk16f.Offset -= m_BspMagic;
  //m_BspHeader.Chunk16g.Offset -= m_BspMagic;
  m_BspHeader.Weather.Offset -= m_BspMagic;
  m_BspHeader.WeatherPolyhedra.Offset -= m_BspMagic;
  m_BspHeader.Chunk19.Offset -= m_BspMagic;
  m_BspHeader.Chunk20.Offset -= m_BspMagic;
  m_BspHeader.PathfindingSurface.Offset -= m_BspMagic;
  m_BspHeader.BackgroundSound.Offset -= m_BspMagic;
  m_BspHeader.SoundEnvironment.Offset -= m_BspMagic;
  m_BspHeader.Chunk24.Offset -= m_BspMagic;
  m_BspHeader.SoundPASData.Offset -= m_BspMagic;
  m_BspHeader.Chunk26.Offset -= m_BspMagic;
  m_BspHeader.Chunk27.Offset -= m_BspMagic;
  m_BspHeader.Markers.Offset -= m_BspMagic;
  m_BspHeader.DetailObjects.Offset -= m_BspMagic;
  m_BspHeader.RuntimeDecals.Offset -= m_BspMagic;
  
  gTagManager.ActivateLightmap(m_BspHeader.LightmapsTag.TagId);

  LoadShaders();

  m_CollBsp.Initialize(m_pMapFile, m_BspMagic, m_Magic);
  m_CollBsp.LoadCollisionBsp(m_BspHeader.CollBspHeader.Offset);

  LoadLightmapBsp();

  InitBspXrefs();

  LoadTriIndices();
  LoadClusters();
  LoadWeather();

  LoadPathfindingSurface();
  LoadSounds();
  LoadDetailObjects();
  LoadRuntimeDecals();

  TRACE("bsp decals:  %d\n", m_BspHeader.RuntimeDecals.Count);

  switch(version)
  {
  case 0x5:
    LoadMaterialMeshHeaders();
    LoadXboxSubmeshes();
    break;

  case 0x7:
    LoadMaterialMeshHeaders();
    LoadPcSubmeshes();
    break;

  case 0x261:
    LoadMaterialMeshHeaders();
    LoadPcSubmeshes();
    break;

  }
}

/*-------------------------------------------------------------------
 * Name: Initialize()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CVisibleBsp::Initialize(CFile *pMapFile, UINT magic, UINT bsp_magic)
{
  m_pMapFile = pMapFile;
  m_Magic = magic;
  m_BspMagic = bsp_magic;
}

/*-------------------------------------------------------------------
 * Name: LoadPcSubmeshes()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CVisibleBsp::LoadPcSubmeshes()
{
  int i, v;
  CString str;
  SUBMESH_INFO *pPcSubMesh;

  ResetBoundingBox();

  for(i=0; i<m_SubMeshCount; i++)
  {
    pPcSubMesh = &m_pMesh[i];
    
    str.Format("Loading Submesh[%d]\n", i);
    g_pOutput->PostText(str, LOG_BLACK);

    pPcSubMesh->VertCount = pPcSubMesh->header.VertexCount1;
    pPcSubMesh->IndexCount = pPcSubMesh->header.SurfaceCount;

    /* Allocate vertex and index arrays */ 
    pPcSubMesh->pIndex = new TRI_INDICES[pPcSubMesh->IndexCount];
    pPcSubMesh->pVert = new UNCOMPRESSED_BSP_VERT[pPcSubMesh->VertCount];
    pPcSubMesh->pLightmapVert = new UNCOMPRESSED_LIGHTMAP_VERT[pPcSubMesh->VertCount];

    /* Ahoy - read in that thar data argghh */ 
    m_pMapFile->Seek(pPcSubMesh->header.PcVertexDataOffset, 0);
    m_pMapFile->Read(pPcSubMesh->pVert, pPcSubMesh->VertCount*sizeof(UNCOMPRESSED_BSP_VERT));
    m_pMapFile->Read(pPcSubMesh->pLightmapVert, pPcSubMesh->VertCount*sizeof(UNCOMPRESSED_LIGHTMAP_VERT));

    m_pMapFile->Seek(pPcSubMesh->header.SurfaceIndicesOffset, 0);
    m_pMapFile->Read(pPcSubMesh->pIndex, pPcSubMesh->IndexCount*sizeof(TRI_INDICES));

    //update tri count
    m_TriTotal += pPcSubMesh->VertCount;

    /* Update the map extents info for analysis */ 
    for(v=0; v<pPcSubMesh->VertCount; v++)
    {
      //TRACE("%d %f %f\n", v, pPcSubMesh->pLightmapData[v].uv[0], pPcSubMesh->pLightmapData[v].uv[1]);
      UpdateBoundingBox(i, pPcSubMesh->pVert[v].vertex_k, 7);
    }
    
    gTagManager.GetShaderTextures(pPcSubMesh->header.ShaderTag.TagId,
                                  pPcSubMesh->BaseTextureIndex,
                                  pPcSubMesh->DetailTexture1,
                                  pPcSubMesh->DetailTexture2);
  }
}

/*-------------------------------------------------------------------
 * Name: LoadMaterialMeshHeaders()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CVisibleBsp::LoadMaterialMeshHeaders()
{
  int i, j, hdr_count;
  UINT offset;
  CString ref;
  char *pShaderName;
  BOOL ret;
  int mv_max, mv_min;

  mv_max = 0;
  mv_min = 0x7FFFFFFF;

  //load the submesh index
  m_pLightmaps = new BSP_LIGHTMAP[m_BspHeader.SubmeshHeader.Count];
  m_pMapFile->Seek(m_BspHeader.SubmeshHeader.Offset, 0);
  m_pMapFile->Read(m_pLightmaps, sizeof(BSP_LIGHTMAP)*m_BspHeader.SubmeshHeader.Count);

  m_SubMeshCount = 0;
  for(i=0; i<m_BspHeader.SubmeshHeader.Count; i++)
  {
    m_pLightmaps[i].Material.Offset -= m_BspMagic;
    m_SubMeshCount += m_pLightmaps[i].Material.Count; //calculate total number of submeshes for allocation

    UINT temp = m_pLightmaps[i].Material.Offset + m_pLightmaps[i].Material.Count*sizeof(MATERIAL_SUBMESH_HEADER);
    //TRACE("%3d: %08X  %08X (%d)\n", i, m_pLightmaps[i].Material.Offset, temp, m_SubMeshCount);
  }

  //load the submesh headers
  m_pMesh = new SUBMESH_INFO[m_SubMeshCount];
  ZeroMemory(m_pMesh, sizeof(SUBMESH_INFO)*m_SubMeshCount);
  hdr_count = 0;

  for(i=0; i<m_BspHeader.SubmeshHeader.Count; i++)
  {
    for(j=0; j<m_pLightmaps[i].Material.Count; j++)
    {
      offset = m_pLightmaps[i].Material.Offset + sizeof(MATERIAL_SUBMESH_HEADER)*j;
      m_pMapFile->Seek(offset, 0);
      ret = m_pMapFile->Read(&(m_pMesh[hdr_count].header), sizeof(MATERIAL_SUBMESH_HEADER));

      //subtract magic from offsets
      m_pMesh[hdr_count].header.Vert_Reflexive -= m_BspMagic;
      m_pMesh[hdr_count].header.LightmapVert_Reflexive -= m_BspMagic;
      m_pMesh[hdr_count].header.VertexDataOffset -= m_BspMagic;
      m_pMesh[hdr_count].header.PcVertexDataOffset -= m_BspMagic;
      m_pMesh[hdr_count].header.SurfaceIndicesOffset = sizeof(TRI_INDICES)*m_pMesh[hdr_count].header.SurfaceIndicesOffset + 
                                                  m_BspHeader.SubmeshTriIndices.Offset;
      
      if(m_pMesh[hdr_count].header.MemoryVertexOffset < mv_min)
        mv_min = m_pMesh[hdr_count].header.MemoryVertexOffset;
      if(m_pMesh[hdr_count].header.MemoryLightmapOffset > mv_max)
        mv_max = m_pMesh[hdr_count].header.MemoryLightmapOffset;

      //TRACE("submesh: %4d  MVO=%8X LVO=%8X\n", i, m_pMesh[hdr_count].header.MemoryVertexOffset, m_pMesh[hdr_count].header.MemoryLightmapOffset);
      if(hdr_count > 0)
      {
      }

      UINT refl;
      refl =  m_pMesh[hdr_count].header.MemoryVertexOffset - m_Magic + m_BspMagic;


      if(hdr_count > 0)
      {
        /*
        UINT dSO1 = m_pMesh[hdr_count].header.SomeOffset1 - m_pMesh[hdr_count-1].header.SomeOffset1;
        UINT dSO2 = m_pMesh[hdr_count].header.SomeOffset2 - m_pMesh[hdr_count-1].header.SomeOffset2;
        TRACE("submesh: %4d  SO1=%8X dSO1=%8X SO2=%8X dSO2=%8X\n", i, m_pMesh[hdr_count].header.SomeOffset1, dSO1, m_pMesh[hdr_count].header.SomeOffset2, dSO2);
        TRACE("MeshHdr %5d, hdr = [%3d][%3d] MVO=%8X dMVO=%8X FVO=%8X dFVO=%8X\n", i, hdr_count, m_SubMeshCount-hdr_count, 
          m_pMesh[hdr_count].header.MemoryVertexOffset,
          m_pMesh[hdr_count].header.MemoryVertexOffset - m_pMesh[hdr_count-1].header.MemoryVertexOffset,
          m_pMesh[hdr_count].header.VertexDataOffset,
          m_pMesh[hdr_count].header.VertexDataOffset - m_pMesh[hdr_count-1].header.VertexDataOffset
          );
          */

        TRACE("MeshHdr %5d, hdr = [%3d][%3d] MVO=%8X dMVO=%8X SO1=%8X dSO1=%8d SO2=%8X dSO2=%8d\n", i, hdr_count, m_SubMeshCount-hdr_count, 
          m_pMesh[hdr_count].header.MemoryVertexOffset,
          m_pMesh[hdr_count].header.MemoryVertexOffset - m_pMesh[hdr_count-1].header.MemoryVertexOffset,
          m_pMesh[hdr_count].header.SomeOffset1,
          m_pMesh[hdr_count].header.SomeOffset1 - m_pMesh[hdr_count-1].header.SomeOffset1,
          m_pMesh[hdr_count].header.SomeOffset2,
          m_pMesh[hdr_count].header.SomeOffset2 - m_pMesh[hdr_count-1].header.SomeOffset2
          );
      }

      //load the shader name string (for output window)
      ref = CheckForReference(m_pMapFile, m_pMesh[hdr_count].header.ShaderTag.NamePtr, m_Magic);
      pShaderName = ref.GetBuffer(128);
      strncpy(m_pMesh[hdr_count].shader_name, pShaderName, 128);
      ref.ReleaseBuffer();

      m_pMesh[hdr_count].LightmapIndex = m_pLightmaps[i].LightmapIndex;

      hdr_count++;
    }
  }

  MAPFILE_HDR mh;
  INDEX_HDR ih;
  HaloMap.GetHeaders(&mh, &ih);

  
  TRACE("mvmin: %8X mvmax: %8X mgc: %8X bmgc: %8X declen: %8X tio: %8X tml: %8X mrdo: %8X mrds: %8X bsps: %8X\n", mv_min, mv_max, 
                                         m_Magic,
                                         m_BspMagic,
                                         mh.decomp_len, 
                                         mh.TagIndexOffset, 
                                         mh.TagIndexMetaLength,
                                         ih.ModelRawDataOffset,
                                         ih.ModelRawDataSize,
                                         gBspManager.m_pBspInfo[0].BspSize);
}

/*-------------------------------------------------------------------
 * Name: LoadXboxSubmeshes()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CVisibleBsp::LoadXboxSubmeshes()
{
  int i;
  CString str;
  SUBMESH_INFO *pSubMesh;

  ResetBoundingBox();

  for(i=0; i<m_SubMeshCount; i++)
  {
    pSubMesh = &m_pMesh[i];
    
    str.Format("Loading Submesh[%d]\n", i);
    g_pOutput->PostText(str, LOG_BLACK);

    pSubMesh->VertCount = pSubMesh->header.VertexCount1;
    pSubMesh->IndexCount = pSubMesh->header.SurfaceCount;

    /* Allocate vertex and index arrays */ 
    pSubMesh->pIndex = new TRI_INDICES[pSubMesh->IndexCount];
    pSubMesh->pCompVert = new COMPRESSED_BSP_VERT[pSubMesh->VertCount];
    pSubMesh->pVert = new UNCOMPRESSED_BSP_VERT[pSubMesh->VertCount];
    pSubMesh->pCompLightmapVert = new COMPRESSED_LIGHTMAP_VERT[pSubMesh->VertCount];
    pSubMesh->pLightmapVert = new UNCOMPRESSED_LIGHTMAP_VERT[pSubMesh->VertCount];

    /* Ahoy - read in that thar data argghh */ 
    printf("%4d VDO=%8X SIO=%8X LVR=%8x\n", i, pSubMesh->header.VertexDataOffset,
      pSubMesh->header.SurfaceIndicesOffset,
      pSubMesh->header.LightmapVert_Reflexive);

    m_pMapFile->Seek(pSubMesh->header.VertexDataOffset, 0);
    m_pMapFile->Read(pSubMesh->pCompVert, pSubMesh->VertCount*sizeof(COMPRESSED_BSP_VERT));

    m_pMapFile->Seek(pSubMesh->header.SurfaceIndicesOffset, 0);
    m_pMapFile->Read(pSubMesh->pIndex, pSubMesh->IndexCount*sizeof(TRI_INDICES));

    // read in compressed unknown (lightmap coords?) data
    REFLEXIVE reflexive_ptr;
    m_pMapFile->Seek(pSubMesh->header.LightmapVert_Reflexive, 0);
    m_pMapFile->Read(&reflexive_ptr, 12);
    reflexive_ptr.Offset -= m_BspMagic;
    m_pMapFile->Seek(reflexive_ptr.Offset, 0);
    m_pMapFile->Read(pSubMesh->pCompLightmapVert, pSubMesh->VertCount*sizeof(COMPRESSED_LIGHTMAP_VERT));

    //update tri count
    m_TriTotal += pSubMesh->VertCount;
    
    // Decompress verts and update the map extents info for analysis
    for(int v=0; v<pSubMesh->VertCount; v++)
    {
      pSubMesh->pVert[v].vertex_k[0] = pSubMesh->pCompVert[v].vertex_k[0];
      pSubMesh->pVert[v].vertex_k[1] = pSubMesh->pCompVert[v].vertex_k[1];
      pSubMesh->pVert[v].vertex_k[2] = pSubMesh->pCompVert[v].vertex_k[2];

      pSubMesh->pVert[v].uv[0] = pSubMesh->pCompVert[v].uv[0];
      pSubMesh->pVert[v].uv[1] = pSubMesh->pCompVert[v].uv[1];

      DecompressIntToVector(pSubMesh->pCompVert[v].comp_normal, pSubMesh->pVert[v].normal);
      DecompressIntToVector(pSubMesh->pCompVert[v].comp_binormal, pSubMesh->pVert[v].binormal);
      DecompressIntToVector(pSubMesh->pCompVert[v].comp_tangent, pSubMesh->pVert[v].tangent);

      pSubMesh->pLightmapVert[v].uv[0] = DecompressShortToFloat(pSubMesh->pCompLightmapVert[v].comp_uv[0]);
      pSubMesh->pLightmapVert[v].uv[1] = DecompressShortToFloat(pSubMesh->pCompLightmapVert[v].comp_uv[1]);

      printf("Mesh %4d Vert %5d (%12.3f, %12.3f, %12.3f)\n", i, v, pSubMesh->pVert[v].vertex_k[0],
             pSubMesh->pVert[v].vertex_k[1],pSubMesh->pVert[v].vertex_k[2]);

      UpdateBoundingBox(v, pSubMesh->pVert[v].vertex_k, 5);
    }
    
    if(pSubMesh->pCompVert)
    {
      delete [] pSubMesh->pCompVert;
      pSubMesh->pCompVert = NULL;
    }

    if(pSubMesh->pCompLightmapVert)
    {
      delete [] pSubMesh->pCompLightmapVert;
      pSubMesh->pCompLightmapVert = NULL;
    }

    gTagManager.GetShaderTextures(pSubMesh->header.ShaderTag.TagId,
                                  pSubMesh->BaseTextureIndex,
                                  pSubMesh->DetailTexture1,
                                  pSubMesh->DetailTexture2);
  }
}


/*-------------------------------------------------------------------
 * Name: ResetBoundingBox()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CVisibleBsp::ResetBoundingBox()
{
  m_MapBox.min[0] = 40000;
  m_MapBox.min[1] = 40000;
  m_MapBox.min[2] = 40000;
  m_MapBox.max[0] = -40000;
  m_MapBox.max[1] = -40000;
  m_MapBox.max[2] = -40000;

  m_Centroid[0] = 0;
  m_Centroid[1] = 0;
  m_Centroid[2] = 0;
  m_CentroidCount = 0;
}

/*-------------------------------------------------------------------
 * Name: UpdateBoundingBox()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CVisibleBsp::UpdateBoundingBox(int mesh_index, float *pCoord, UINT version)
{
  if((mesh_index >= 0)&&(mesh_index <m_SubMeshCount))
  {
    //update total map extents
    if(pCoord[0] > m_MapBox.max[0])m_MapBox.max[0] = pCoord[0];
    if(pCoord[1] > m_MapBox.max[1])m_MapBox.max[1] = pCoord[1];
    if(pCoord[2] > m_MapBox.max[2])m_MapBox.max[2] = pCoord[2];

    if(pCoord[0] < m_MapBox.min[0])m_MapBox.min[0] = pCoord[0];
    if(pCoord[1] < m_MapBox.min[1])m_MapBox.min[1] = pCoord[1];
    if(pCoord[2] < m_MapBox.min[2])m_MapBox.min[2] = pCoord[2];

    m_Centroid[0] += pCoord[0];
    m_Centroid[1] += pCoord[1];
    m_Centroid[2] += pCoord[2];
    m_CentroidCount++;

    //update current mesh extents
    if(pCoord[0] > m_pMesh[mesh_index].Box.max[0])m_pMesh[mesh_index].Box.max[0] = pCoord[0];
    if(pCoord[1] > m_pMesh[mesh_index].Box.max[1])m_pMesh[mesh_index].Box.max[1] = pCoord[1];
    if(pCoord[2] > m_pMesh[mesh_index].Box.max[2])m_pMesh[mesh_index].Box.max[2] = pCoord[2];
    
    if(pCoord[0] < m_pMesh[mesh_index].Box.min[0])m_pMesh[mesh_index].Box.min[0] = pCoord[0];
    if(pCoord[1] < m_pMesh[mesh_index].Box.min[1])m_pMesh[mesh_index].Box.min[1] = pCoord[1];
    if(pCoord[2] < m_pMesh[mesh_index].Box.min[2])m_pMesh[mesh_index].Box.min[2] = pCoord[2];
  }
}

/*-------------------------------------------------------------------
 * Name: DumpBoundingBoxInfo()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CVisibleBsp::DumpBoundingBoxInfo(int mesh_index)
{
  CString str;

  if(mesh_index == -1)
  {
    str.Format("Map Extents:  (%.1f, %.1f, %.1f) to (%.1f, %.1f, %.1f)\n",
               m_MapBox.min[0],
               m_MapBox.min[1],
               m_MapBox.min[2],
               m_MapBox.max[0],
               m_MapBox.max[1],
               m_MapBox.max[2]);

    g_pOutput->PostText(str, LOG_BLUE);

    str.Format("Map Size:  %.1f x %.1f x %.1f\n",
               m_MapBox.max[0] - m_MapBox.min[0],
               m_MapBox.max[1] - m_MapBox.min[1],
               m_MapBox.max[2] - m_MapBox.min[2]);
    g_pOutput->PostText(str, LOG_BLUE);
  }
  else
  {
/*    str.Format("Submesh Extents:  (%.1f, %.1f, %.1f) to (%.1f, %.1f, %.1f)\n",
               m_MeshBox[mesh_index].min[0],
               m_MeshBox[mesh_index].min[1],
               m_MeshBox[mesh_index].min[2],
               m_MeshBox[mesh_index].max[0],
               m_MeshBox[mesh_index].max[1],
               m_MeshBox[mesh_index].max[2]);
    g_pOutput->PostText(str, LOG_BLUE);
    */
  }
}

/*-------------------------------------------------------------------
 * Name: GetMapCentroid()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CVisibleBsp::GetMapCentroid(float *cx, float *cy, float *cz)
{
  //*cx = (m_MapBox.max[0] + m_MapBox.min[0])/2.0f;
  //*cy = (m_MapBox.max[1] + m_MapBox.min[1])/2.0f;
  //*cz = (m_MapBox.max[2] + m_MapBox.min[2])/2.0f;

  if(m_CentroidCount == 0)
  {
    *cx = 0;
    *cy = 0;
    *cz = 0;
  }
  else
  {
    *cx = m_Centroid[0]/m_CentroidCount;
    *cy = m_Centroid[1]/m_CentroidCount;
    *cz = m_Centroid[2]/m_CentroidCount;
  }
}

void CVisibleBsp::ExportPcMeshToObj(CString path)
{
  CStdioFile OutFile;
  CString str;
  int i,v,f;
  float vertex[3];
  UINT face[3];

  if(!OutFile.Open(path, CFile::modeCreate|CFile::modeWrite))
  {
    AfxMessageBox("Failed to create exported mesh file.");
  }
  else
  {
    int base_count=1;
    int vert_count=1;
    for(i=0; i<m_SubMeshCount; i++)
    {
      str.Format("g Submesh_%03d\n", i);
      OutFile.WriteString(str);

      for(v=0; v<m_pMesh[i].VertCount; v++)
      {
        vertex[0] = m_pMesh[i].pVert[v].vertex_k[0];
        vertex[1] = m_pMesh[i].pVert[v].vertex_k[1];
        vertex[2] = m_pMesh[i].pVert[v].vertex_k[2];

        str.Format("v %f %f %f\n", vertex[0], vertex[1], vertex[2]);
        OutFile.WriteString(str);

        if((v%10)==0)
        {
          str.Format("#vertex %d %d (%d)\n", v, vert_count+=10,m_pMesh[i].VertCount);
          OutFile.WriteString(str);
        }
      }

      for(f=0; f<m_pMesh[i].IndexCount; f++)
      {
        face[0] = m_pMesh[i].pIndex[f].tri_ind[0]+base_count;
        face[1] = m_pMesh[i].pIndex[f].tri_ind[1]+base_count;
        face[2] = m_pMesh[i].pIndex[f].tri_ind[2]+base_count;

        str.Format("f %d %d %d\n", face[0], face[1], face[2]);
        OutFile.WriteString(str);
      }
      base_count+=m_pMesh[i].VertCount;
    }    
  }

  OutFile.Close();
}

void CVisibleBsp::ExportXboxMeshToObj(CString path)
{
  CStdioFile OutFile;
  CString str;
  int i,v,f;
  float vertex[3];
  UINT face[3];

  if(!OutFile.Open(path, CFile::modeCreate|CFile::modeWrite))
  {
    AfxMessageBox("Failed to create exported mesh file.");
  }
  else
  {
    int base_count=1;
    int vert_count=1;
    for(i=0; i<m_SubMeshCount; i++)
    {
      str.Format("g Submesh_%03d\n", i);
      OutFile.WriteString(str);

      for(v=0; v<m_pMesh[i].VertCount; v++)
      {
        vertex[0] = m_pMesh[i].pVert[v].vertex_k[0];
        vertex[1] = m_pMesh[i].pVert[v].vertex_k[1];
        vertex[2] = m_pMesh[i].pVert[v].vertex_k[2];

        str.Format("v %f %f %f\n", vertex[0], vertex[1], vertex[2]);
        OutFile.WriteString(str);

        if((v%10)==0)
        {
          str.Format("#vertex %d %d (%d)\n", v, vert_count+=10,m_pMesh[i].VertCount);
          OutFile.WriteString(str);
        }
      }

      for(f=0; f<m_pMesh[i].IndexCount; f++)
      {
        face[0] = m_pMesh[i].pIndex[f].tri_ind[0]+base_count;
        face[1] = m_pMesh[i].pIndex[f].tri_ind[1]+base_count;
        face[2] = m_pMesh[i].pIndex[f].tri_ind[2]+base_count;

        str.Format("f %d %d %d\n", face[0], face[1], face[2]);
        OutFile.WriteString(str);
      }
      base_count+=m_pMesh[i].VertCount;
    }    
  }

  OutFile.Close();
}

void CVisibleBsp::LoadWeather()
{
  if(m_BspHeader.Weather.Count > 0)
  {  
    m_pWeather = new BSP_WEATHER[m_BspHeader.Weather.Count];
    m_pMapFile->Seek(m_BspHeader.Weather.Offset, 0);
    m_pMapFile->Read(m_pWeather, sizeof(BSP_WEATHER)*m_BspHeader.Weather.Count);   
  }
}

CString CVisibleBsp::GetMeshInfo(int mesh_index)
{
  CString info = "SparkEdit Error:  Invalid Mesh";

  if(mesh_index < m_SubMeshCount)
  {
    info.Format("Triangle Count: %d\r\n", m_pMesh[mesh_index].IndexCount);
    info += gTagManager.GetTagDescription(m_pMesh[mesh_index].header.ShaderTag.NamePtr);
  }

  return(info);
}

void CVisibleBsp::LoadClusters()
{
  BSP_SUBCLUSTER* pSubCluster;
  BSP_CLUSTER* pClusters;
  BSP_CLUSTER_PORTAL_INDICES* pPortals;
  BSP_CLUSTER_SURFACE_INDICES *pSurfIndices;
  BSP_CLUSTER_PORTAL *pClusterPortals;


  //DumpReflexiveInfo(&m_BspHeader.Clusters, "Clusters", sizeof(BSP_CLUSTER), m_BspMagic);

  LoadSection(m_pMapFile, &m_BspHeader.Clusters, sizeof(BSP_CLUSTER), 0);
  pClusters = (BSP_CLUSTER*)m_BspHeader.Clusters.unknown;

  for(int c=0; c<m_BspHeader.Clusters.Count; c++)
  {
    //DumpReflexiveInfo(&(pClusters[c].UR1), "BSP_CLUSTER_PREDICTED_RESOURCE", sizeof(BSP_CLUSTER_PREDICTED_RESOURCE), m_BspMagic);

    LoadSection(m_pMapFile, &(pClusters[c].PredictedResources), sizeof(BSP_CLUSTER_PREDICTED_RESOURCE), m_BspMagic);

    BSP_CLUSTER_PREDICTED_RESOURCE* pUR1 = (BSP_CLUSTER_PREDICTED_RESOURCE*)pClusters[c].PredictedResources.unknown;

    //DumpReflexiveInfo(&(pClusters[c].SubCluster), "SubCluster", sizeof(BSP_SUBCLUSTER), m_BspMagic);

    LoadSection(m_pMapFile, &(pClusters[c].SubCluster), sizeof(BSP_SUBCLUSTER), m_BspMagic);
    pSubCluster = (BSP_SUBCLUSTER*)pClusters[c].SubCluster.unknown;

    for(int sc=0; sc<pClusters[c].SubCluster.Count; sc++)
    {
      //DumpReflexiveInfo(&(pSubCluster[sc].SurfaceIndices), "SurfIndices", sizeof(BSP_CLUSTER_SURFACE_INDICES), m_BspMagic);
  
      LoadSection(m_pMapFile, &(pSubCluster[sc].SurfaceIndices), sizeof(BSP_CLUSTER_SURFACE_INDICES), m_BspMagic);
      pSurfIndices = (BSP_CLUSTER_SURFACE_INDICES*)pSubCluster[sc].SurfaceIndices.unknown;
    }

    //DumpReflexiveInfo(&(pClusters[c].Portals), "Portals", sizeof(BSP_CLUSTER_PORTAL_INDICES), m_BspMagic);

    LoadSection(m_pMapFile, &(pClusters[c].Portals), sizeof(BSP_CLUSTER_PORTAL_INDICES), m_BspMagic);
    pPortals = (BSP_CLUSTER_PORTAL_INDICES*)pClusters[c].Portals.unknown;
  }

  // Read in Cluster Data
  m_pClusterData = new BYTE[m_BspHeader.ClusterDataSize];
  m_pMapFile->Seek(m_BspHeader.ClusterData.Offset, 0);
  m_pMapFile->Read(m_pClusterData, m_BspHeader.ClusterDataSize); 

  // Read in Cluster Portals
  DumpReflexiveInfo(&m_BspHeader.ClusterPortals, "ClusterPortals", sizeof(BSP_CLUSTER_PORTAL), m_BspMagic);
  LoadSection(m_pMapFile, &m_BspHeader.ClusterPortals, sizeof(BSP_CLUSTER_PORTAL), 0);
  pClusterPortals = (BSP_CLUSTER_PORTAL*)m_BspHeader.ClusterPortals.unknown;

  for(int p=0; p<m_BspHeader.ClusterPortals.Count; p++)
  {
    DumpReflexiveInfo(&pClusterPortals[p].Vertices, "PortalVerts", sizeof(VERTEX), m_BspMagic);
    LoadSection(m_pMapFile, &pClusterPortals[p].Vertices, sizeof(VERTEX), m_BspMagic);
  }
}

void CVisibleBsp::InitBspXrefs()
{
  ZeroMemory(m_BspXrefs, sizeof(m_BspXrefs));

  strncpy(m_BspXrefs[0].name, "Shaders", 127);
  m_BspXrefs[0].offset = m_BspHeader.Shaders.Offset;
  m_BspXrefs[0].count = m_BspHeader.Shaders.Count;

  strncpy(m_BspXrefs[1].name, "CollBspHeader", 127);
  m_BspXrefs[1].offset = m_BspHeader.CollBspHeader.Offset;
  m_BspXrefs[1].count = m_BspHeader.CollBspHeader.Count;

  strncpy(m_BspXrefs[2].name, "Nodes", 127);
  m_BspXrefs[2].offset = m_BspHeader.Nodes.Offset;
  m_BspXrefs[2].count = m_BspHeader.Nodes.Count;

  strncpy(m_BspXrefs[3].name, "Leaves", 127);
  m_BspXrefs[3].offset = m_BspHeader.Leaves.Offset;
  m_BspXrefs[3].count = m_BspHeader.Leaves.Count;

  strncpy(m_BspXrefs[4].name, "LeafSurfaces", 127);
  m_BspXrefs[4].offset = m_BspHeader.LeafSurfaces.Offset;
  m_BspXrefs[4].count = m_BspHeader.LeafSurfaces.Count;

  strncpy(m_BspXrefs[5].name, "SubmeshTriIndices", 127);
  m_BspXrefs[5].offset = m_BspHeader.SubmeshTriIndices.Offset;
  m_BspXrefs[5].count = m_BspHeader.SubmeshTriIndices.Count;

  strncpy(m_BspXrefs[6].name, "SubmeshHeader", 127);
  m_BspXrefs[6].offset = m_BspHeader.SubmeshHeader.Offset;
  m_BspXrefs[6].count = m_BspHeader.SubmeshHeader.Count;

  strncpy(m_BspXrefs[7].name, "Chunk10", 127);
  m_BspXrefs[7].offset = m_BspHeader.Chunk10.Offset;
  m_BspXrefs[7].count = m_BspHeader.Chunk10.Count;

  strncpy(m_BspXrefs[8].name, "Chunk11", 127);
  m_BspXrefs[8].offset = m_BspHeader.Chunk11.Offset;
  m_BspXrefs[8].count = m_BspHeader.Chunk11.Count;

  strncpy(m_BspXrefs[9].name, "Chunk12", 127);
  m_BspXrefs[9].offset = m_BspHeader.Chunk12.Offset;
  m_BspXrefs[9].count = m_BspHeader.Chunk12.Count;

  strncpy(m_BspXrefs[10].name, "Clusters", 127);
  m_BspXrefs[10].offset = m_BspHeader.Clusters.Offset;
  m_BspXrefs[10].count = m_BspHeader.Clusters.Count;

  strncpy(m_BspXrefs[11].name, "ClusterData", 127);
  m_BspXrefs[11].offset = m_BspHeader.ClusterData.Offset;
  m_BspXrefs[11].count = m_BspHeader.ClusterData.Count;

  strncpy(m_BspXrefs[12].name, "ClusterPortals", 127);
  m_BspXrefs[12].offset = m_BspHeader.ClusterPortals.Offset;
  m_BspXrefs[12].count = m_BspHeader.ClusterPortals.Count;

  strncpy(m_BspXrefs[13].name, "Chunk16a", 127);
  m_BspXrefs[13].offset = m_BspHeader.Chunk16a.Offset;
  m_BspXrefs[13].count = m_BspHeader.Chunk16a.Count;

  strncpy(m_BspXrefs[14].name, "BreakableSurfaces", 127);
  m_BspXrefs[14].offset = m_BspHeader.BreakableSurfaces.Offset;
  m_BspXrefs[14].count = m_BspHeader.BreakableSurfaces.Count;

  strncpy(m_BspXrefs[15].name, "FogPlanes", 127);
  m_BspXrefs[15].offset = m_BspHeader.FogPlanes.Offset;
  m_BspXrefs[15].count = m_BspHeader.FogPlanes.Count;

  strncpy(m_BspXrefs[16].name, "FogRegions", 127);
  m_BspXrefs[16].offset = m_BspHeader.FogRegions.Offset;
  m_BspXrefs[16].count = m_BspHeader.FogRegions.Count;

  strncpy(m_BspXrefs[17].name, "FogOrWeatherPallette", 127);
  m_BspXrefs[17].offset = m_BspHeader.FogOrWeatherPallette.Offset;
  m_BspXrefs[17].count = m_BspHeader.FogOrWeatherPallette.Count;

  strncpy(m_BspXrefs[18].name, "Chunk16f", 127);
  m_BspXrefs[18].offset = m_BspHeader.Chunk16f.Offset;
  m_BspXrefs[18].count = m_BspHeader.Chunk16f.Count;

  strncpy(m_BspXrefs[19].name, "Chunk16g", 127);
  m_BspXrefs[19].offset = m_BspHeader.Chunk16g.Offset;
  m_BspXrefs[19].count = m_BspHeader.Chunk16g.Count;

  strncpy(m_BspXrefs[20].name, "Weather", 127);
  m_BspXrefs[20].offset = m_BspHeader.Weather.Offset;
  m_BspXrefs[20].count = m_BspHeader.Weather.Count;

  strncpy(m_BspXrefs[21].name, "WeatherPolyhedra", 127);
  m_BspXrefs[21].offset = m_BspHeader.WeatherPolyhedra.Offset;
  m_BspXrefs[21].count = m_BspHeader.WeatherPolyhedra.Count;

  strncpy(m_BspXrefs[22].name, "Chunk19", 127);
  m_BspXrefs[22].offset = m_BspHeader.Chunk19.Offset;
  m_BspXrefs[22].count = m_BspHeader.Chunk19.Count;

  strncpy(m_BspXrefs[23].name, "Chunk20", 127);
  m_BspXrefs[23].offset = m_BspHeader.Chunk20.Offset;
  m_BspXrefs[23].count = m_BspHeader.Chunk20.Count;

  strncpy(m_BspXrefs[24].name, "PathfindingSurface", 127);
  m_BspXrefs[24].offset = m_BspHeader.PathfindingSurface.Offset;
  m_BspXrefs[24].count = m_BspHeader.PathfindingSurface.Count;

  strncpy(m_BspXrefs[25].name, "Chunk24", 127);
  m_BspXrefs[25].offset = m_BspHeader.Chunk24.Offset;
  m_BspXrefs[25].count = m_BspHeader.Chunk24.Count;

  strncpy(m_BspXrefs[26].name, "BackgroundSound", 127);
  m_BspXrefs[26].offset = m_BspHeader.BackgroundSound.Offset;
  m_BspXrefs[26].count = m_BspHeader.BackgroundSound.Count;

  strncpy(m_BspXrefs[27].name, "SoundEnvironment", 127);
  m_BspXrefs[27].offset = m_BspHeader.SoundEnvironment.Offset;
  m_BspXrefs[27].count = m_BspHeader.SoundEnvironment.Count;

  strncpy(m_BspXrefs[28].name, "SoundPASData", 127);
  m_BspXrefs[28].offset = m_BspHeader.SoundPASData.Offset;
  m_BspXrefs[28].count = m_BspHeader.SoundPASData.Count;

  strncpy(m_BspXrefs[29].name, "Chunk26", 127);
  m_BspXrefs[29].offset = m_BspHeader.Chunk26.Offset;
  m_BspXrefs[29].count = m_BspHeader.Chunk26.Count;

  strncpy(m_BspXrefs[30].name, "Chunk27", 127);
  m_BspXrefs[30].offset = m_BspHeader.Chunk27.Offset;
  m_BspXrefs[30].count = m_BspHeader.Chunk27.Count;

  strncpy(m_BspXrefs[31].name, "Markers", 127);
  m_BspXrefs[31].offset = m_BspHeader.Markers.Offset;
  m_BspXrefs[31].count = m_BspHeader.Markers.Count;

  strncpy(m_BspXrefs[32].name, "DetailObjects", 127);
  m_BspXrefs[32].offset = m_BspHeader.DetailObjects.Offset;
  m_BspXrefs[32].count = m_BspHeader.DetailObjects.Count;

  strncpy(m_BspXrefs[33].name, "RuntimeDecals", 127);
  m_BspXrefs[33].offset = m_BspHeader.RuntimeDecals.Offset;
  m_BspXrefs[33].count = m_BspHeader.RuntimeDecals.Count;

  strncpy(m_BspXrefs[34].name, "Coll Nodes", 127);
  m_BspXrefs[34].offset = m_CollBsp.m_CollHeader.Nodes.Offset;
  m_BspXrefs[34].count = m_CollBsp.m_CollHeader.Nodes.Count;

  strncpy(m_BspXrefs[35].name, "Coll Planes", 127);
  m_BspXrefs[35].offset = m_CollBsp.m_CollHeader.Planes.Offset;
  m_BspXrefs[35].count = m_CollBsp.m_CollHeader.Planes.Count;

  strncpy(m_BspXrefs[36].name, "Coll Leaves", 127);
  m_BspXrefs[36].offset = m_CollBsp.m_CollHeader.Leaves.Offset;
  m_BspXrefs[36].count = m_CollBsp.m_CollHeader.Leaves.Count;

  strncpy(m_BspXrefs[37].name, "Coll Bsp2dRef", 127);
  m_BspXrefs[37].offset = m_CollBsp.m_CollHeader.Bsp2dRef.Offset;
  m_BspXrefs[37].count = m_CollBsp.m_CollHeader.Bsp2dRef.Count;

  strncpy(m_BspXrefs[38].name, "Coll Bsp2dNodes", 127);
  m_BspXrefs[38].offset = m_CollBsp.m_CollHeader.Bsp2dNodes.Offset;
  m_BspXrefs[38].count = m_CollBsp.m_CollHeader.Bsp2dNodes.Count;

  strncpy(m_BspXrefs[39].name, "Coll Surfaces", 127);
  m_BspXrefs[39].offset = m_CollBsp.m_CollHeader.Surfaces.Offset;
  m_BspXrefs[39].count = m_CollBsp.m_CollHeader.Surfaces.Count;

  strncpy(m_BspXrefs[40].name, "Coll Edges", 127);
  m_BspXrefs[40].offset = m_CollBsp.m_CollHeader.Edges.Offset;
  m_BspXrefs[40].count = m_CollBsp.m_CollHeader.Edges.Count;

  strncpy(m_BspXrefs[41].name, "Coll Vertices", 127);
  m_BspXrefs[41].offset = m_CollBsp.m_CollHeader.Vertices.Offset;
  m_BspXrefs[41].count = m_CollBsp.m_CollHeader.Vertices.Count;
}


void CVisibleBsp::LoadShaders()
{
  if(m_BspHeader.Shaders.Count > 0)
  {  
    m_pShaders = new BSP_SHADER[m_BspHeader.Shaders.Count];
    m_pMapFile->Seek(m_BspHeader.Shaders.Offset, 0);
    m_pMapFile->Read(m_pShaders, sizeof(BSP_SHADER)*m_BspHeader.Shaders.Count);   
  }
}

void CVisibleBsp::LoadLightmapBsp()
{
  if(m_BspHeader.Nodes.Count > 0)
  {  
    m_pNodes = new BSP_NODES[m_BspHeader.Nodes.Count];
    m_pMapFile->Seek(m_BspHeader.Nodes.Offset, 0);
    m_pMapFile->Read(m_pNodes, sizeof(BSP_NODES)*m_BspHeader.Nodes.Count);   
  }

  if(m_BspHeader.Leaves.Count > 0)
  {  
    m_pLeaves = new BSP_LEAF[m_BspHeader.Leaves.Count];
    m_pMapFile->Seek(m_BspHeader.Leaves.Offset, 0);
    m_pMapFile->Read(m_pLeaves, sizeof(BSP_LEAF)*m_BspHeader.Leaves.Count);   
  }

  if(m_BspHeader.LeafSurfaces.Count > 0)
  {  
    m_pLeafSurfaces = new BSP_LEAF_SURFACE[m_BspHeader.LeafSurfaces.Count];
    m_pMapFile->Seek(m_BspHeader.LeafSurfaces.Offset, 0);
    m_pMapFile->Read(m_pLeafSurfaces, sizeof(BSP_LEAF_SURFACE)*m_BspHeader.LeafSurfaces.Count);   
  }
}

void CVisibleBsp::LoadTriIndices()
{
  if(m_BspHeader.SubmeshTriIndices.Count > 0)
  {  
    m_pTriIndices = new TRI_INDICES[m_BspHeader.SubmeshTriIndices.Count];
    m_pMapFile->Seek(m_BspHeader.SubmeshTriIndices.Offset, 0);
    m_pMapFile->Read(m_pTriIndices, sizeof(TRI_INDICES)*m_BspHeader.SubmeshTriIndices.Count);   
  }
}

BOOL CVisibleBsp::LoadSection(CFile *pMapFile, REFLEXIVE *pReflexive, int reflexive_element_size, UINT magic)
{
  BOOL bStatus = FALSE;

  if(pReflexive->Count == 0)
  {
    bStatus = TRUE;
  }
  else
  {
    pReflexive->unknown = new BYTE[reflexive_element_size*pReflexive->Count];
    
    if(pReflexive->unknown)
    {
      pReflexive->Offset -= magic;
      m_pMapFile->Seek(pReflexive->Offset, 0);
      bStatus = m_pMapFile->Read(pReflexive->unknown, reflexive_element_size*pReflexive->Count);
    }
  }

  return(bStatus);
}

void CVisibleBsp::LoadReflectionBlock()
{
  BSP_SECTION_HEADER SectionHdr;
  REFLEXIVE* pMeshes;
  REFLEXIVE* pLightmaps;
  UINT temp;

  m_pMapFile->Seek(0x800, 0);
  m_pMapFile->Read(&SectionHdr, sizeof(BSP_SECTION_HEADER));

  SectionHdr.Xbox_Vert_ReflexiveStart -= m_BspMagic;
  SectionHdr.Xbox_LightmapVert_ReflexiveStart -= m_BspMagic;

  pMeshes = new REFLEXIVE[SectionHdr.Xbox_Vert_ReflexiveCount];
  pLightmaps = new REFLEXIVE[SectionHdr.Xbox_LightmapVert_ReflexiveCount];

  m_pMapFile->Seek(SectionHdr.Xbox_Vert_ReflexiveStart, 0);
  m_pMapFile->Read(pMeshes, sizeof(REFLEXIVE)*SectionHdr.Xbox_Vert_ReflexiveCount);

  m_pMapFile->Seek(SectionHdr.Xbox_LightmapVert_ReflexiveStart, 0);
  m_pMapFile->Read(pLightmaps, sizeof(REFLEXIVE)*SectionHdr.Xbox_LightmapVert_ReflexiveCount);

  TRACE("Vert Reflexive Block\n");
  for(int i=0; i<SectionHdr.Xbox_Vert_ReflexiveCount; i++)
  {
    if(i < SectionHdr.Xbox_Vert_ReflexiveCount-1)
      temp = pMeshes[i].Offset - pMeshes[i+1].Offset;

    TRACE("%5d %8X %8X\n", i, pMeshes[i].Offset - m_BspMagic, temp);
  }

  TRACE("Lightmap Reflexive Block\n");
  for(i=0; i<SectionHdr.Xbox_LightmapVert_ReflexiveCount; i++)
  {
    if(i < SectionHdr.Xbox_LightmapVert_ReflexiveCount-1)
      temp = pLightmaps[i].Offset - pLightmaps[i+1].Offset;

    TRACE("%5d %8X %8X\n", i, pLightmaps[i].Offset - m_BspMagic, temp);
  }
}

void CVisibleBsp::DrawClusterPortals()
{
  VERTEX* pPortalVerts;
  BSP_CLUSTER_PORTAL *pClusterPortals;

  pClusterPortals = (BSP_CLUSTER_PORTAL*)m_BspHeader.ClusterPortals.unknown;

  glColor4f(0,0,0,0);
  glLineWidth(10.0);

  for(int p=0; p<m_BspHeader.ClusterPortals.Count; p++)
  {
    pPortalVerts = (VERTEX*)pClusterPortals[p].Vertices.unknown;

    glBegin(GL_LINES);
    for(int v=0; v<pClusterPortals[p].Vertices.Count; v++)
    {
      glVertex3fv((float*)&pPortalVerts[v]);
    }
    glVertex3fv((float*)&pPortalVerts[pClusterPortals[p].Vertices.Count-1]);

    glEnd();
  }

  glEnable(GL_BLEND);
  glColor4f(0.5,0.5,0,0.3f);

  for(p=0; p<m_BspHeader.ClusterPortals.Count; p++)
  {
    pPortalVerts = (VERTEX*)pClusterPortals[p].Vertices.unknown;

    glBegin(GL_POLYGON);
    for(int v=0; v<pClusterPortals[p].Vertices.Count; v++)
    {
      glVertex3fv((float*)&pPortalVerts[v]);
    }
    glEnd();
  }
  glDisable(GL_BLEND);
}

void CVisibleBsp::LoadPathfindingSurface()
{
  if(m_BspHeader.PathfindingSurface.Count > 0)
  {
    m_pPathfindingSurface = new BYTE[m_BspHeader.PathfindingSurface.Count];
    m_pMapFile->Seek(m_BspHeader.PathfindingSurface.Offset, 0);
    m_pMapFile->Read(m_pPathfindingSurface, sizeof(BYTE)*m_BspHeader.PathfindingSurface.Count);   
  }
}

void CVisibleBsp::LoadSounds()
{
  if(m_BspHeader.BackgroundSound.Count > 0)
  {
    m_pBackgroundSounds = new BSP_BACKGROUND_SOUND[m_BspHeader.BackgroundSound.Count];
    m_pMapFile->Seek(m_BspHeader.BackgroundSound.Offset, 0);
    m_pMapFile->Read(m_pBackgroundSounds, sizeof(BSP_BACKGROUND_SOUND)*m_BspHeader.BackgroundSound.Count);   
  }

  if(m_BspHeader.SoundEnvironment.Count > 0)
  {
    m_pSoundEnv = new BSP_SOUND_ENV[m_BspHeader.SoundEnvironment.Count];
    m_pMapFile->Seek(m_BspHeader.SoundEnvironment.Offset, 0);
    m_pMapFile->Read(m_pSoundEnv, sizeof(BSP_SOUND_ENV)*m_BspHeader.SoundEnvironment.Count);   
  }

  if(m_BspHeader.SoundPASDataSize > 0)
  {
    m_pPASData = new BYTE[m_BspHeader.SoundPASDataSize];
    m_pMapFile->Seek(m_BspHeader.SoundPASData.Offset, 0);
    m_pMapFile->Read(m_pPASData, sizeof(BYTE)*m_BspHeader.SoundPASDataSize);   
  }
}

void CVisibleBsp::LoadDetailObjects()
{
  if(m_BspHeader.DetailObjects.Count > 0)
  {
    m_pDetailObjects = new BSP_DETAIL_OBJECT[m_BspHeader.DetailObjects.Count];
    m_pMapFile->Seek(m_BspHeader.DetailObjects.Offset, 0);
    m_pMapFile->Read(m_pDetailObjects, sizeof(BSP_DETAIL_OBJECT)*m_BspHeader.DetailObjects.Count);   
  }
}

void CVisibleBsp::LoadRuntimeDecals()
{
  if(m_BspHeader.RuntimeDecals.Count > 0)
  {
    m_pRuntimeDecals = new BSP_RUNTIME_DECAL[m_BspHeader.RuntimeDecals.Count];
    m_pMapFile->Seek(m_BspHeader.RuntimeDecals.Offset, 0);
    m_pMapFile->Read(m_pRuntimeDecals, sizeof(BSP_RUNTIME_DECAL)*m_BspHeader.RuntimeDecals.Count);   
  }
}
