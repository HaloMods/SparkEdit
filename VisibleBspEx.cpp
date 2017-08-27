// VisibleBspEx.cpp: implementation of the CVisibleBspEx class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sparkedit.h"
#include "VisibleBspEx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVisibleBspEx::CVisibleBspEx()
{
  m_pReflexive_V = NULL;
  m_pReflexive_LMV = NULL;
  m_NewBspGlobals.new_bsp_magic = 0;
  
  m_pNewShaders = NULL;
}

CVisibleBspEx::~CVisibleBspEx()
{

}

void CVisibleBspEx::Cleanup()
{
  if(m_pReflexive_V)
  {
    delete m_pReflexive_V;
    m_pReflexive_V = NULL;
  }

  if(m_pReflexive_LMV)
  {
    delete m_pReflexive_V;
    m_pReflexive_LMV = NULL;
  }

  if(m_pNewShaders)
  {
    delete m_pNewShaders;
    m_pNewShaders = NULL;
  }
}

UINT CVisibleBspEx::WriteVertexDataBlocks(CFile *pMapFile)
{
  int i, i_comp, v_cnt;
  UINT V_accum;
  UINT bytes_written;


  //determine vertex data blocks
  m_pReflexive_V = new REFLEXIVE[m_SubMeshCount];
  m_pReflexive_LMV = new REFLEXIVE[m_SubMeshCount];

  memset(m_pReflexive_V, 0xCA, sizeof(REFLEXIVE)*m_SubMeshCount);
  memset(m_pReflexive_LMV, 0xCA, sizeof(REFLEXIVE)*m_SubMeshCount);

  //Set accumulators to point immediately after header/reflexive data
  V_accum = 0x800 + sizeof(BSP_SECTION_HEADER) + 2*sizeof(REFLEXIVE)*m_SubMeshCount + 8;
  m_SectionHeader.Xbox_Vert_ReflexiveStart = 0x800 + sizeof(BSP_SECTION_HEADER);
  m_SectionHeader.Xbox_LightmapVert_ReflexiveStart = m_SectionHeader.Xbox_Vert_ReflexiveStart + 
                                        m_SubMeshCount*sizeof(REFLEXIVE);

  pMapFile->Seek(V_accum, 0);
  //loop through submeshes to do the following:
  //  -calculate lightmap vert reflexive
  //  -calculate vert reflexive
  //  -update offsets and other vars within the headers
  for(i=0; i<m_SubMeshCount; i++)
  {
    i_comp = m_SubMeshCount - i - 1;
    v_cnt = m_pMesh[i].header.VertexCount1;

    // Write out vertex data, update accumulator
    m_pReflexive_V[i_comp].Offset = V_accum + m_NewBspGlobals.new_bsp_magic;
    pMapFile->Write(m_pMesh[i].pCompVert, sizeof(COMPRESSED_BSP_VERT)*v_cnt);
    V_accum += sizeof(COMPRESSED_BSP_VERT)*v_cnt;
    
    // Write out lightmap vertex data, update accumulator
    if(m_pMesh[i].LightmapIndex == -1)
    {
      m_pReflexive_LMV[i_comp].Offset = V_accum + m_NewBspGlobals.new_bsp_magic;
    }
    else
    {
      m_pReflexive_LMV[i_comp].Offset = V_accum + m_NewBspGlobals.new_bsp_magic;
      pMapFile->Write(m_pMesh[i].pCompLightmapVert, sizeof(COMPRESSED_LIGHTMAP_VERT)*v_cnt);    
      V_accum += sizeof(COMPRESSED_LIGHTMAP_VERT)*v_cnt;
    }

    //Update submesh headers
    m_pMesh[i].header.LightmapVert_Reflexive = m_SectionHeader.Xbox_LightmapVert_ReflexiveStart + sizeof(REFLEXIVE)*i_comp; //magic? 
    m_pMesh[i].header.Vert_Reflexive = m_SectionHeader.Xbox_Vert_ReflexiveStart + sizeof(REFLEXIVE)*i_comp; //magic? 

    //Write out Index Data?
  }

  pMapFile->Seek(0x800 + sizeof(BSP_SECTION_HEADER), 0);
  pMapFile->Write(m_pReflexive_V, sizeof(REFLEXIVE)*m_SubMeshCount);
  pMapFile->Write(m_pReflexive_LMV, sizeof(REFLEXIVE)*m_SubMeshCount);

  // fill in the section header, located at 0x800
  
  m_SectionHeader.BspHeaderOffset = V_accum + m_NewBspGlobals.new_bsp_magic;
  m_SectionHeader.Xbox_Vert_ReflexiveStart += m_NewBspGlobals.new_bsp_magic;
  m_SectionHeader.Xbox_Vert_ReflexiveCount = m_SubMeshCount;
  m_SectionHeader.Xbox_LightmapVert_ReflexiveCount = m_SubMeshCount;
  m_SectionHeader.Xbox_LightmapVert_ReflexiveStart += m_NewBspGlobals.new_bsp_magic;
  m_SectionHeader.tag[0] = 'p';
  m_SectionHeader.tag[1] = 's';
  m_SectionHeader.tag[2] = 'b';
  m_SectionHeader.tag[3] = 's';

  pMapFile->Seek(0x800, 0);
  pMapFile->Write(&m_SectionHeader, sizeof(BSP_SECTION_HEADER));

  bytes_written = m_SectionHeader.BspHeaderOffset - 0x800 - m_NewBspGlobals.new_bsp_magic;

  return(bytes_written);
}

//
// This function creates separate xbox format buffers for
// each PC submesh.  It then converts the verts to xbox format.
//
void CVisibleBspEx::ConvertMeshesToXboxFormat()
{
  int i, v;
  CString str;
  UINT comp_normal, comp_binormal, comp_tangent;
  SUBMESH_INFO *pPcSubMesh;

  for(i=0; i<m_SubMeshCount; i++)
  {
    pPcSubMesh = &m_pMesh[i];
    
    // Allocate xbox vertex buffer    
    pPcSubMesh->pCompVert = new COMPRESSED_BSP_VERT[pPcSubMesh->VertCount];
    pPcSubMesh->pCompLightmapVert = new COMPRESSED_LIGHTMAP_VERT[pPcSubMesh->VertCount];

    for(v=0; v<pPcSubMesh->VertCount; v++)
    {
      //compress normal data into xbox format
      CompressVector(comp_normal, pPcSubMesh->pVert[v].normal);
      CompressVector(comp_binormal, pPcSubMesh->pVert[v].binormal);
      CompressVector(comp_tangent, pPcSubMesh->pVert[v].tangent);

      //fill in xbox vert data
      pPcSubMesh->pCompVert[v].vertex_k[0] = pPcSubMesh->pVert[v].vertex_k[0];
      pPcSubMesh->pCompVert[v].vertex_k[1] = pPcSubMesh->pVert[v].vertex_k[1];
      pPcSubMesh->pCompVert[v].vertex_k[2] = pPcSubMesh->pVert[v].vertex_k[2];
      pPcSubMesh->pCompVert[v].comp_normal = comp_normal;
      pPcSubMesh->pCompVert[v].comp_binormal = comp_binormal;
      pPcSubMesh->pCompVert[v].comp_tangent = comp_tangent;
      pPcSubMesh->pCompVert[v].uv[0] = pPcSubMesh->pVert[v].uv[0];
      pPcSubMesh->pCompVert[v].uv[1] = pPcSubMesh->pVert[v].uv[1];

      CompressVector(pPcSubMesh->pCompLightmapVert[v].comp_normal, pPcSubMesh->pLightmapVert[v].normal);
      CompressFloatToShort(pPcSubMesh->pLightmapVert[v].uv[0], pPcSubMesh->pCompLightmapVert[v].comp_uv[0]);
      CompressFloatToShort(pPcSubMesh->pLightmapVert[v].uv[1], pPcSubMesh->pCompLightmapVert[v].comp_uv[1]);

      /*
      DecompressIntToVector(comp_normal, junk);

      TRACE("Output: (%f %f %f) %08X %08X %08X (%f %f)\n",
        pPcSubMesh->pVert[v].vertex_k[0],
        pPcSubMesh->pVert[v].vertex_k[1],
        pPcSubMesh->pVert[v].vertex_k[2],
        comp_normal,
        comp_binormal,
        comp_tangent,
        pPcSubMesh->pVert[v].uv[0],
        pPcSubMesh->pVert[v].uv[1]);
        */
    }    
  }
}

void CVisibleBspEx::ConvertBsp()
{
  CFile file;
  UINT current_offset, bytes_written, bsp_header_offset;

  //update the extern cache file
  RefreshTestFile();
  SetNewConversionGlobals();

  memcpy(&m_NewBspHeader, &m_BspHeader, sizeof(BSP_HEADER));

  file.Open("E:\\mods\\halo\\xbox_conversion\\sidewinder\\cache005.map", CFile::modeReadWrite);
//  file.Open("E:\\mods\\halo\\xbox_conversion\\hangemhigh\\cache003.map", CFile::modeReadWrite);

  //TODO: fix lightmap uv compression
  ConvertMeshesToXboxFormat();
  UpdateShaderReferences();

  current_offset = 0x800;

  //Write out first mapfile section
  bytes_written = WriteVertexDataBlocks(&file);
  current_offset += bytes_written;

  //skip bsp header, we will write it out later
  bsp_header_offset = current_offset;
  current_offset += sizeof(BSP_HEADER);

  //Write out Shader Block
  m_NewBspHeader.Shaders.Offset = current_offset;
  bytes_written = WriteShaderBlock(&file, current_offset, m_NewBspGlobals.new_bsp_magic);
  current_offset += bytes_written;


  //Write out Collision BSP
  m_NewBspHeader.CollBspHeader.Offset = current_offset;
  bytes_written = m_CollBsp.WriteCollisionBsp(&file, current_offset, m_NewBspGlobals.new_bsp_magic);
  current_offset += bytes_written;

  //Write out Nodes
  m_NewBspHeader.Nodes.Offset = current_offset;
  bytes_written = WriteNodesBlock(&file, current_offset, m_NewBspGlobals.new_bsp_magic);
  current_offset += bytes_written;

  //Write out Leaves
  m_NewBspHeader.Leaves.Offset = current_offset;
  bytes_written = WriteLeavesBlock(&file, current_offset, m_NewBspGlobals.new_bsp_magic);
  current_offset += bytes_written;

  //Write out Leaf Surfaces
  m_NewBspHeader.LeafSurfaces.Offset = current_offset;
  bytes_written = WriteLeafSurfacesBlock(&file, current_offset, m_NewBspGlobals.new_bsp_magic);
  current_offset += bytes_written;

  //Write out Tri Indices
  m_NewBspHeader.SubmeshTriIndices.Offset = current_offset;
  bytes_written = WriteTriIndicesBlock(&file, current_offset, m_NewBspGlobals.new_bsp_magic);
  current_offset += bytes_written;

  //Write out SubmeshHeader
  current_offset = m_BspHeader.SubmeshHeader.Offset;
  m_NewBspHeader.SubmeshHeader.Offset = current_offset;
  bytes_written = WriteLightmapsBlock(&file, current_offset, m_NewBspGlobals.new_bsp_magic);
  current_offset += bytes_written;

  //Write out Clusters
  m_NewBspHeader.Clusters.Offset = current_offset + m_NewBspGlobals.new_bsp_magic;
  bytes_written = WriteClustersBlock(&file, current_offset, m_NewBspGlobals.new_bsp_magic);
  current_offset += bytes_written;

  //Write out ClusterData and Cluster Portals
  m_NewBspHeader.ClusterData.Offset = current_offset + m_NewBspGlobals.new_bsp_magic;
  bytes_written = WriteClusterDataAndPortals(&file, current_offset, m_NewBspGlobals.new_bsp_magic);
  current_offset += bytes_written;

  //*Write out FogPlanes

  //*Write out FogOrWeatherPallette

  //Write out Weather
  m_NewBspHeader.Weather.Offset = current_offset + m_NewBspGlobals.new_bsp_magic;
  bytes_written = WriteWeatherBlock(&file, current_offset, m_NewBspGlobals.new_bsp_magic);
  current_offset += bytes_written;
  
  //*Write out Weather Polyhedra

  //Write out PathfindingSurface
  m_NewBspHeader.PathfindingSurface.Offset = current_offset + m_NewBspGlobals.new_bsp_magic;
  bytes_written = WritePathfindingBlock(&file, current_offset, m_NewBspGlobals.new_bsp_magic);
  current_offset += bytes_written;

  //Write out BackgroundSound (header adjusted in function)
  //Write out SoundEnvironment (header adjusted in function)
  bytes_written = WriteSoundBlocks(&file, current_offset, m_NewBspGlobals.new_bsp_magic);
  current_offset += bytes_written;


  //Write out SoundPASData
  m_NewBspHeader.SoundPASData.Offset = current_offset + m_NewBspGlobals.new_bsp_magic;
  bytes_written = WriteSoundPASBlock(&file, current_offset, m_NewBspGlobals.new_bsp_magic);
  current_offset += bytes_written; 

  //*Write out Markers

  //Write out DetailObjects]
  m_NewBspHeader.DetailObjects.Offset = current_offset + m_NewBspGlobals.new_bsp_magic;
  bytes_written = WriteDetailObjectsBlock(&file, current_offset, m_NewBspGlobals.new_bsp_magic);
  current_offset += bytes_written; 

  //Write out RuntimeDecals]
  m_NewBspHeader.RuntimeDecals.Offset = current_offset + m_NewBspGlobals.new_bsp_magic;
  bytes_written = WriteRuntimeDecalsBlock(&file, current_offset, m_NewBspGlobals.new_bsp_magic);
  current_offset += bytes_written; 

  //Write out new header (function takes care of seek)
  WriteHeader(&file, bsp_header_offset);

  //AnalyzeFileSection3(&file, 0, file.GetLength(), 0, 0);
  file.Close();
}

void CVisibleBspEx::WriteHeader(CFile *pMapFile, UINT offset)
{
  //patch the lightmap tag ref

  m_NewBspHeader.LightmapsTag.NamePtr = m_NewBspGlobals.new_lightmap_tag.NamePtr;
  m_NewBspHeader.LightmapsTag.TagId = m_NewBspGlobals.new_lightmap_tag.TagId;

  m_NewBspHeader.Shaders.Offset += m_NewBspGlobals.new_bsp_magic;
  m_NewBspHeader.CollBspHeader.Offset += m_NewBspGlobals.new_bsp_magic;
  m_NewBspHeader.Nodes.Offset += m_NewBspGlobals.new_bsp_magic;
  m_NewBspHeader.Leaves.Offset += m_NewBspGlobals.new_bsp_magic;
  m_NewBspHeader.LeafSurfaces.Offset += m_NewBspGlobals.new_bsp_magic;
  m_NewBspHeader.SubmeshTriIndices.Offset += m_NewBspGlobals.new_bsp_magic;
  m_NewBspHeader.SubmeshHeader.Offset += m_NewBspGlobals.new_bsp_magic;
  m_NewBspHeader.Chunk10.Offset += m_NewBspGlobals.new_bsp_magic;
  m_NewBspHeader.Chunk11.Offset += m_NewBspGlobals.new_bsp_magic;
  m_NewBspHeader.Chunk12.Offset += m_NewBspGlobals.new_bsp_magic;
  m_NewBspHeader.Chunk16a.Offset += m_NewBspGlobals.new_bsp_magic;
  m_NewBspHeader.BreakableSurfaces.Offset += m_NewBspGlobals.new_bsp_magic;
  m_NewBspHeader.FogPlanes.Offset += m_NewBspGlobals.new_bsp_magic;
  m_NewBspHeader.FogRegions.Offset += m_NewBspGlobals.new_bsp_magic;
  m_NewBspHeader.FogOrWeatherPallette.Offset += m_NewBspGlobals.new_bsp_magic;
  m_NewBspHeader.Chunk16f.Offset += m_NewBspGlobals.new_bsp_magic;
  m_NewBspHeader.Chunk16g.Offset += m_NewBspGlobals.new_bsp_magic;
  m_NewBspHeader.WeatherPolyhedra.Offset += m_NewBspGlobals.new_bsp_magic;


  //m_NewBspHeader.ClusterPortals.Offset += m_NewBspGlobals.new_bsp_magic;
  //m_NewBspHeader.Weather.Offset += m_NewBspGlobals.new_bsp_magic;
  m_NewBspHeader.Chunk19.Offset += m_NewBspGlobals.new_bsp_magic;
  m_NewBspHeader.Chunk20.Offset += m_NewBspGlobals.new_bsp_magic;
  //m_NewBspHeader.PathfindingSurface.Offset += m_NewBspGlobals.new_bsp_magic;
  m_NewBspHeader.Chunk24.Offset += m_NewBspGlobals.new_bsp_magic;
  //m_NewBspHeader.BackgroundSound.Offset += m_NewBspGlobals.new_bsp_magic;
  //m_NewBspHeader.SoundEnvironment.Offset += m_NewBspGlobals.new_bsp_magic;
  //m_NewBspHeader.SoundPASData.Offset += m_NewBspGlobals.new_bsp_magic;
  m_NewBspHeader.Markers.Offset += m_NewBspGlobals.new_bsp_magic;
  //m_NewBspHeader.DetailObjects.Offset += m_NewBspGlobals.new_bsp_magic;
  //m_NewBspHeader.RuntimeDecals.Offset += m_NewBspGlobals.new_bsp_magic;

  //null out decals since they cause headaches
  m_NewBspHeader.RuntimeDecals.Count = 0;

  pMapFile->Seek(offset, 0);
  pMapFile->Write(&m_NewBspHeader, sizeof(m_NewBspHeader));
}

//
//Writes out the shader block (used for texture caching efficiently)
//
UINT CVisibleBspEx::WriteShaderBlock(CFile *pFile, UINT start_offset, UINT new_magic)
{
  UINT bytes_written = 0;

  pFile->Seek(start_offset, 0);
  pFile->Write(m_pNewShaders, sizeof(BSP_SHADER)*m_BspHeader.Shaders.Count);

  bytes_written = sizeof(BSP_SHADER)*m_BspHeader.Shaders.Count;

  return(bytes_written);
}

void CVisibleBspEx::UpdateShaderReferences()
{
  m_pNewShaders = new BSP_SHADER[m_BspHeader.Shaders.Count];

  memcpy(m_pNewShaders, m_pShaders, sizeof(BSP_SHADER)*m_BspHeader.Shaders.Count);

  //Update tag_id and name ref
  for(int i=0; i<m_BspHeader.Shaders.Count; i++)
  {
    m_pNewShaders[i].ShaderTag.NamePtr = m_NewBspGlobals.new_shader_tag.NamePtr;
    m_pNewShaders[i].ShaderTag.TagId = m_NewBspGlobals.new_shader_tag.TagId;
  }
}

UINT CVisibleBspEx::WriteNodesBlock(CFile* pFile, UINT start_offset, UINT new_magic)
{
  UINT bytes_written = sizeof(BSP_NODES)*m_BspHeader.Nodes.Count;

  pFile->Seek(start_offset, 0);
  pFile->Write(m_pNodes, bytes_written);

  //check to see if we are 32-bit aligned, if not add padding
  if((bytes_written%4) != 0)
    bytes_written += 2;
  

  return(bytes_written);
}

UINT CVisibleBspEx::WriteLeavesBlock(CFile* pFile, UINT start_offset, UINT new_magic)
{
  UINT bytes_written = sizeof(BSP_LEAF)*m_BspHeader.Leaves.Count;

  pFile->Seek(start_offset, 0);
  pFile->Write(m_pLeaves, bytes_written);

  return(bytes_written);
}

UINT CVisibleBspEx::WriteLeafSurfacesBlock(CFile* pFile, UINT start_offset, UINT new_magic)
{
  UINT bytes_written = sizeof(BSP_LEAF_SURFACE)*m_BspHeader.LeafSurfaces.Count;

  pFile->Seek(start_offset, 0);
  pFile->Write(m_pLeafSurfaces, bytes_written);

  return(bytes_written);
}

UINT CVisibleBspEx::WriteTriIndicesBlock(CFile* pFile, UINT start_offset, UINT new_magic)
{
  UINT bytes_written = sizeof(TRI_INDICES)*m_BspHeader.SubmeshTriIndices.Count;

  pFile->Seek(start_offset, 0);
  pFile->Write(m_pTriIndices, bytes_written);

  //check to see if we are 32-bit aligned, if not add padding
  if((bytes_written%4) != 0)
    bytes_written += 2;  

  return(bytes_written);
}

UINT CVisibleBspEx::WriteLightmapsBlock(CFile* pFile, UINT start_offset, UINT new_magic)
{
  BSP_LIGHTMAP *pNewLightmaps = NULL;
  MATERIAL_SUBMESH_HEADER temp_msh;
  UINT bytes_written;
  UINT mesh_index;

  //vars used to update reflexives
  UINT bsp_lightmaps_size = sizeof(BSP_LIGHTMAP)*m_BspHeader.SubmeshHeader.Count;
  UINT submesh_header_accum = start_offset + bsp_lightmaps_size;
  
  //allocate temporary data structs so we can modify the originals
  pNewLightmaps = new BSP_LIGHTMAP[m_BspHeader.SubmeshHeader.Count];
  memcpy(pNewLightmaps, m_pLightmaps, sizeof(BSP_LIGHTMAP)*m_BspHeader.SubmeshHeader.Count);

  //update BSP_LIGHTMAP's reflexives
  for(int i=0; i<m_BspHeader.SubmeshHeader.Count; i++)
  {
    pNewLightmaps[i].LightmapIndex = 0; //for testing
    pNewLightmaps[i].Material.Offset = submesh_header_accum + new_magic;
    submesh_header_accum += pNewLightmaps[i].Material.Count*sizeof(MATERIAL_SUBMESH_HEADER);
  }

  //write out bsp_lightmap block
  bytes_written = sizeof(BSP_LIGHTMAP)*m_BspHeader.SubmeshHeader.Count;
  pFile->Seek(start_offset, 0);
  pFile->Write(pNewLightmaps, bytes_written);

  UINT old_memory_base_address = m_pMesh[0].header.MemoryVertexOffset;


  //update MATERIAL_SUBMESH_HEADER reflexives
  mesh_index = 0;
  for(i=0; i<m_SubMeshCount; i++)
  {
    memcpy(&temp_msh, &m_pMesh[i], sizeof(MATERIAL_SUBMESH_HEADER));

    //TODO: update reflexives, tag_ids
    temp_msh.ShaderTag.NamePtr = m_NewBspGlobals.new_shader_tag.NamePtr;
    temp_msh.ShaderTag.TagId = m_NewBspGlobals.new_shader_tag.TagId;

    temp_msh.PcVertexDataOffset += new_magic;

    // Verified for final
    temp_msh.SurfaceIndicesOffset -= m_BspHeader.SubmeshTriIndices.Offset;
    temp_msh.SurfaceIndicesOffset /= sizeof(TRI_INDICES);
    temp_msh.CompVertBufferSize = temp_msh.VertexCount1*(sizeof(COMPRESSED_BSP_VERT)+sizeof(COMPRESSED_LIGHTMAP_VERT));
    temp_msh.VertexDataOffset = m_pReflexive_V[m_SubMeshCount - i - 1].Offset;
    temp_msh.Vert_Reflexive = m_SectionHeader.Xbox_Vert_ReflexiveStart + (m_SubMeshCount - i - 1)*sizeof(REFLEXIVE);
    temp_msh.LightmapVert_Reflexive = m_SectionHeader.Xbox_LightmapVert_ReflexiveStart + (m_SubMeshCount - i - 1)*sizeof(REFLEXIVE);
    temp_msh.UnkAlways3 = 3;
    temp_msh.SomeOffset1 = 0x10000;
    temp_msh.SomeOffset2 = 0x11000;

    //TODO: figure out how the fuck i'm going to calculate these:
    //  UINT MemoryVertexOffset;
    //  UINT MemoryLightmapOffset;
    //  Note:  i know these are related to vertex and lightmap data blocks because the delta between offsets
    //         matches the file data.  Probably pointers into a global memory buffer.
    temp_msh.MemoryVertexOffset = temp_msh.MemoryVertexOffset - old_memory_base_address + m_NewBspGlobals.new_memory_base_address;
    temp_msh.MemoryLightmapOffset = temp_msh.MemoryLightmapOffset - old_memory_base_address + m_NewBspGlobals.new_memory_base_address;


    //write data out to file
    pFile->Write(&temp_msh, sizeof(MATERIAL_SUBMESH_HEADER));
    bytes_written += sizeof(MATERIAL_SUBMESH_HEADER);
  }

  return(bytes_written);
}

UINT CVisibleBspEx::WriteClustersBlock(CFile* pFile, UINT start_offset, UINT new_magic)
{
  BSP_SUBCLUSTER* pSubCluster;
  BSP_CLUSTER* pClusters;
  BSP_CLUSTER_PORTAL_INDICES* pPortals;
  BSP_CLUSTER_SURFACE_INDICES *pSurfIndices;
  BSP_CLUSTER_PREDICTED_RESOURCE* pUR1;
  UINT bytes_written = 0;
  UINT accum = start_offset;

  //
  // update cluster section and subsection reflexives
  //

  //Clusters +
    //UR1
    //SubClusters
      //SurfIndices
    //Portals


  //update cluster reflexives
  m_BspHeader.Clusters.Offset = accum + new_magic;
  DumpReflexiveInfo(&m_BspHeader.Clusters, "Out Clusters", sizeof(BSP_CLUSTER), m_BspMagic);

  pClusters = (BSP_CLUSTER*)m_BspHeader.Clusters.unknown;
  accum += sizeof(BSP_CLUSTER)*m_BspHeader.Clusters.Count;

  //Clusters
  for(int c=0; c<m_BspHeader.Clusters.Count; c++)
  {
//    pClusters[c].BackgroundSoundIndex = -1; //uberfix
//    pClusters[c].SoundEnvIndex = -1; //uberfix
//    pClusters[c].WeatherIndex = -1; //uberfix

    pClusters[c].PredictedResources.Count = 0; //uberfix
    pClusters[c].PredictedResources.Offset = accum + new_magic;
    DumpReflexiveInfo(&(pClusters[c].PredictedResources), "Out BSP_CLUSTER_PREDICTED_RESOURCE", sizeof(BSP_CLUSTER_PREDICTED_RESOURCE), m_BspMagic);
    accum += sizeof(BSP_CLUSTER_PREDICTED_RESOURCE)*pClusters[c].PredictedResources.Count;

    pClusters[c].SubCluster.Offset = accum + new_magic;
    DumpReflexiveInfo(&(pClusters[c].SubCluster), "Out SubCluster", sizeof(BSP_SUBCLUSTER), m_BspMagic);
    accum += sizeof(BSP_SUBCLUSTER)*pClusters[c].SubCluster.Count;
    
    for(int sc=0; sc<pClusters[c].SubCluster.Count; sc++)
    {
      pSubCluster = (BSP_SUBCLUSTER*)pClusters[c].SubCluster.unknown;

      pSubCluster[sc].SurfaceIndices.Offset = accum + new_magic;
      accum += sizeof(BSP_CLUSTER_SURFACE_INDICES)*pSubCluster[sc].SurfaceIndices.Count;

      DumpReflexiveInfo(&(pSubCluster[sc].SurfaceIndices), "Out SurfIndices", sizeof(BSP_SUBCLUSTER), m_BspMagic);
    }

    pClusters[c].Portals.Offset = accum + new_magic;
    DumpReflexiveInfo(&(pClusters[c].Portals), "Out Portals", sizeof(BSP_CLUSTER_PORTAL_INDICES), m_BspMagic);
    accum += sizeof(BSP_CLUSTER_PORTAL_INDICES)*pClusters[c].Portals.Count;

    if((pClusters[c].Portals.Count % 2)==1)
      accum += 2;
  }

  //write out data

  //Write Clusters
  pClusters = (BSP_CLUSTER*)m_BspHeader.Clusters.unknown;
  pFile->Seek(m_BspHeader.Clusters.Offset - new_magic, 0);
  pFile->Write(pClusters, m_BspHeader.Clusters.Count*sizeof(BSP_CLUSTER));

  for(c=0; c<m_BspHeader.Clusters.Count; c++)
  {
    //Write BSP_CLUSTER_PREDICTED_RESOURCE
    pUR1 = (BSP_CLUSTER_PREDICTED_RESOURCE*)pClusters[c].PredictedResources.unknown;
    pFile->Seek(pClusters[c].PredictedResources.Offset - new_magic, 0);
    pFile->Write(pUR1, pClusters[c].PredictedResources.Count*sizeof(BSP_CLUSTER_PREDICTED_RESOURCE));

    //Write SubClusters
    pSubCluster = (BSP_SUBCLUSTER*)pClusters[c].SubCluster.unknown;
    pFile->Seek(pClusters[c].SubCluster.Offset - new_magic, 0);
    pFile->Write(pSubCluster, pClusters[c].SubCluster.Count*sizeof(BSP_SUBCLUSTER));

    for(int sc=0; sc<pClusters[c].SubCluster.Count; sc++)
    {
      //Write SurfIndices
      pSurfIndices = (BSP_CLUSTER_SURFACE_INDICES*)pSubCluster[sc].SurfaceIndices.unknown;
      pFile->Seek(pSubCluster[sc].SurfaceIndices.Offset - new_magic, 0);
      pFile->Write(pSurfIndices, pSubCluster[sc].SurfaceIndices.Count*sizeof(BSP_CLUSTER_SURFACE_INDICES));
    }

    //Write Portals
    pPortals = (BSP_CLUSTER_PORTAL_INDICES*)pClusters[c].Portals.unknown;
    pFile->Seek(pClusters[c].Portals.Offset - new_magic, 0);
    pFile->Write(pPortals, pClusters[c].Portals.Count*sizeof(BSP_CLUSTER_PORTAL_INDICES));
  }

  bytes_written = accum - start_offset;

  return(bytes_written);
}

UINT CVisibleBspEx::WriteClusterDataAndPortals(CFile* pFile, UINT start_offset, UINT new_magic)
{
  UINT bytes_written;
  BSP_CLUSTER_PORTAL* pClusterPortals;
  UINT accum = start_offset;

  //Write Cluster Data
  bytes_written = m_BspHeader.ClusterDataSize;
  pFile->Seek(start_offset, 0);
  pFile->Write(m_pClusterData, bytes_written);

  if((bytes_written % 2)==1)
  {
    bytes_written += 2;
    pFile->Seek(start_offset+bytes_written, 0);
  }

  accum += bytes_written;

  //Update Cluster Portal Reflexives
  m_NewBspHeader.ClusterPortals.Offset = accum + new_magic;
  accum += m_NewBspHeader.ClusterPortals.Count*sizeof(BSP_CLUSTER_PORTAL);
  int k = m_NewBspHeader.ClusterPortals.Count;
  k = sizeof(BSP_CLUSTER_PORTAL);
  k = m_NewBspHeader.ClusterPortals.Count*sizeof(BSP_CLUSTER_PORTAL);
  pClusterPortals = (BSP_CLUSTER_PORTAL*)m_BspHeader.ClusterPortals.unknown;

  for(int p=0; p<m_NewBspHeader.ClusterPortals.Count; p++)
  {
    pClusterPortals[p].Vertices.Offset = accum + new_magic;
    accum += sizeof(VERTEX)*pClusterPortals[p].Vertices.Count;
  }

  // Write out Cluster Portals
  pFile->Seek(m_NewBspHeader.ClusterPortals.Offset - new_magic, 0);
  k = m_NewBspHeader.ClusterPortals.Count*sizeof(BSP_CLUSTER_PORTAL);

  pFile->Write(pClusterPortals, m_NewBspHeader.ClusterPortals.Count*sizeof(BSP_CLUSTER_PORTAL));

  for(p=0; p<m_NewBspHeader.ClusterPortals.Count; p++)
  {
    pFile->Seek(pClusterPortals[p].Vertices.Offset - new_magic, 0);
    pFile->Write(pClusterPortals[p].Vertices.unknown, pClusterPortals[p].Vertices.Count*sizeof(VERTEX));
  }

  bytes_written = accum - start_offset;

  return(bytes_written);
}

UINT CVisibleBspEx::WriteWeatherBlock(CFile* pFile, UINT start_offset, UINT new_magic)
{
  UINT bytes_written = sizeof(BSP_WEATHER)*m_BspHeader.Weather.Count;

  //Patch tag ids for weather
  for(int i=0; i<m_BspHeader.Weather.Count; i++)
  {
    *(UINT*)&m_pWeather[i].WeatherTag.tag = *(UINT*)&m_NewBspGlobals.new_weather_tag1.tag;
    m_pWeather[i].WeatherTag.NamePtr = m_NewBspGlobals.new_weather_tag1.NamePtr;
    m_pWeather[i].WeatherTag.TagId = m_NewBspGlobals.new_weather_tag1.TagId;
    m_pWeather[i].WeatherTag.unknown = m_NewBspGlobals.new_weather_tag1.unknown;

    *(UINT*)&m_pWeather[i].WeatherTag2.tag = *(UINT*)&m_NewBspGlobals.new_weather_tag2.tag;
    m_pWeather[i].WeatherTag2.NamePtr = m_NewBspGlobals.new_weather_tag2.NamePtr;
    m_pWeather[i].WeatherTag2.TagId = m_NewBspGlobals.new_weather_tag2.TagId;
    m_pWeather[i].WeatherTag2.unknown = m_NewBspGlobals.new_weather_tag2.unknown;
  }

  pFile->Seek(start_offset, 0);
  pFile->Write(m_pWeather, bytes_written);

  return(bytes_written);
}

UINT CVisibleBspEx::WritePathfindingBlock(CFile* pFile, UINT start_offset, UINT new_magic)
{
  UINT bytes_written = sizeof(BYTE)*m_BspHeader.PathfindingSurface.Count;

  pFile->Seek(start_offset, 0);
  pFile->Write(m_pPathfindingSurface, bytes_written);

  return(bytes_written);
}

UINT CVisibleBspEx::WriteSoundBlocks(CFile* pFile, UINT start_offset, UINT new_magic)
{
  UINT bytes_written = 0;

  //patch tag_id for conversion

  for(int i=0; i<m_NewBspHeader.BackgroundSound.Count; i++)
  {
    //*(UINT*)&m_pBackgroundSounds[i].SoundTag.tag = *(UINT*)&m_NewBspGlobals.new_background_sound_tag.tag;
    m_pBackgroundSounds[i].SoundTag.TagId = m_NewBspGlobals.new_background_sound_tag.TagId;
    m_pBackgroundSounds[i].SoundTag.NamePtr = m_NewBspGlobals.new_background_sound_tag.NamePtr;
    m_pBackgroundSounds[i].SoundTag.unknown = m_NewBspGlobals.new_background_sound_tag.unknown;
  }

  for(i=0; i<m_NewBspHeader.SoundEnvironment.Count; i++)
  {
    //*(UINT*)&m_pSoundEnv[i].SoundTag.tag = *(UINT*)&m_NewBspGlobals.new_env_sound_tag.tag;
    m_pSoundEnv[i].SoundTag.NamePtr = m_NewBspGlobals.new_env_sound_tag.NamePtr;
    m_pSoundEnv[i].SoundTag.TagId = m_NewBspGlobals.new_env_sound_tag.TagId;
    m_pSoundEnv[i].SoundTag.unknown = m_NewBspGlobals.new_env_sound_tag.unknown;
  }

  m_NewBspHeader.BackgroundSound.Offset = start_offset + new_magic;
  pFile->Seek(start_offset, 0);
  pFile->Write(m_pBackgroundSounds, sizeof(BSP_BACKGROUND_SOUND)*m_BspHeader.BackgroundSound.Count);
  bytes_written += sizeof(BSP_BACKGROUND_SOUND)*m_BspHeader.BackgroundSound.Count;

  m_NewBspHeader.SoundEnvironment.Offset = start_offset + new_magic + bytes_written;
  pFile->Write(m_pSoundEnv, sizeof(BSP_SOUND_ENV)*m_BspHeader.SoundEnvironment.Count);
  bytes_written += sizeof(BSP_SOUND_ENV)*m_BspHeader.SoundEnvironment.Count;

  return(bytes_written);
}

UINT CVisibleBspEx::WriteSoundPASBlock(CFile* pFile, UINT start_offset, UINT new_magic)
{
  UINT bytes_written = sizeof(BYTE)*m_BspHeader.SoundPASDataSize;

  pFile->Seek(start_offset, 0);
  pFile->Write(m_pPASData, bytes_written);

  //pad 32-bit alignment
  int rem = bytes_written%4;
  bytes_written += rem;

  return(bytes_written);
}

UINT CVisibleBspEx::WriteDetailObjectsBlock(CFile* pFile, UINT start_offset, UINT new_magic)
{
  UINT bytes_written = sizeof(BSP_DETAIL_OBJECT)*m_BspHeader.DetailObjects.Count;

  //patch tag_id for conversion
  for(int i=0; i<m_NewBspHeader.SoundEnvironment.Count; i++)
  {
    //m_pDetailObjects[i].unk[0] = 0;
  }

  pFile->Seek(start_offset, 0);
  pFile->Write(m_pDetailObjects, bytes_written);

  return(bytes_written);
}

UINT CVisibleBspEx::WriteRuntimeDecalsBlock(CFile* pFile, UINT start_offset, UINT new_magic)
{
  UINT bytes_written = sizeof(BSP_RUNTIME_DECAL)*m_BspHeader.RuntimeDecals.Count;

  pFile->Seek(start_offset, 0);
  pFile->Write(m_pRuntimeDecals, bytes_written);

  return(bytes_written);
}

void CVisibleBspEx::RefreshTestFile()
{
  system("copy E:\\mods\\halo\\xbox_conversion\\pristine\\cache005.map E:\\mods\\halo\\xbox_conversion\\sidewinder\\cache005.map");
//  system("copy E:\\mods\\halo\\xbox_conversion\\pristine\\cache003.map E:\\mods\\halo\\xbox_conversion\\hangemhigh\\cache003.map");
}

void CVisibleBspEx::SetNewConversionGlobals()
{
  //m_NewBspGlobals.new_lightmap_tag.tag;
  m_NewBspGlobals.new_lightmap_tag.NamePtr;
  m_NewBspGlobals.new_lightmap_tag.TagId = 0xe7e90673;
  m_NewBspGlobals.new_lightmap_tag.unknown = 0;

  *(UINT*)&m_NewBspGlobals.new_weather_tag1.tag = 0x6e696172; //rain
  m_NewBspGlobals.new_weather_tag1.NamePtr = 0;
  m_NewBspGlobals.new_weather_tag1.TagId = -1;
  m_NewBspGlobals.new_weather_tag1.unknown = 0;

  *(UINT*)&m_NewBspGlobals.new_weather_tag2.tag = 0x1a259770; //wind
  m_NewBspGlobals.new_weather_tag2.NamePtr = 0x803c3a8d;
  m_NewBspGlobals.new_weather_tag2.TagId = 0xe803068d;
  m_NewBspGlobals.new_weather_tag2.unknown = 0;

  //m_NewBspGlobals.new_shader_tag.tag;
  m_NewBspGlobals.new_shader_tag.TagId = 0xe7ea0674;
  m_NewBspGlobals.new_shader_tag.NamePtr = 0x803c366c;
  m_NewBspGlobals.new_shader_tag.unknown = 0;
  
  //m_NewBspGlobals.new_background_sound_tag.tag = ;
  m_NewBspGlobals.new_background_sound_tag.NamePtr = 0x803c3ab0;
  m_NewBspGlobals.new_background_sound_tag.TagId = 0xe804068e;
  m_NewBspGlobals.new_background_sound_tag.unknown = 0;

  //m_NewBspGlobals.new_env_sound_tag.tag = ;  //snde
  m_NewBspGlobals.new_env_sound_tag.NamePtr = 0x803c3b05;
  m_NewBspGlobals.new_env_sound_tag.TagId = 0xe8060690;
  m_NewBspGlobals.new_env_sound_tag.unknown = 0;

  m_NewBspGlobals.new_bsp_magic = 0x81915800;
  m_NewBspGlobals.new_mapfile_magic = 0x81915800;
  m_NewBspGlobals.short_lightmap_index = 0;
  m_NewBspGlobals.new_memory_base_address = 0x022711e0;

  CString infilename = m_WorkingDir + "\\import_conv_globals.dat";
  CFile infile;

  if(infile.Open(infilename, CFile::modeRead))
  {
    infile.Read(&m_NewBspGlobals, sizeof(m_NewBspGlobals));
    infile.Close();
  }
  else
  {
    AfxMessageBox("Could not detect import_conv_globals.dat");
  }
}

void CVisibleBspEx::SampleConversionGlobals()
{
  *(UINT*)&m_NewBspGlobals.new_lightmap_tag.tag = *(UINT*)&m_BspHeader.LightmapsTag.tag;
  m_NewBspGlobals.new_lightmap_tag.NamePtr = m_BspHeader.LightmapsTag.NamePtr;
  m_NewBspGlobals.new_lightmap_tag.TagId = m_BspHeader.LightmapsTag.TagId;
  m_NewBspGlobals.new_lightmap_tag.unknown = m_BspHeader.LightmapsTag.unknown;

  if(m_pWeather)
  {
    *(UINT*)&m_NewBspGlobals.new_weather_tag1.tag = *(UINT*)&m_pWeather[0].WeatherTag.tag;
    m_NewBspGlobals.new_weather_tag1.NamePtr = m_pWeather[0].WeatherTag.NamePtr;
    m_NewBspGlobals.new_weather_tag1.TagId = m_pWeather[0].WeatherTag.TagId;
    m_NewBspGlobals.new_weather_tag1.unknown = m_pWeather[0].WeatherTag.unknown;
    
    *(UINT*)&m_NewBspGlobals.new_weather_tag2.tag = *(UINT*)&m_pWeather[0].WeatherTag2.tag;
    m_NewBspGlobals.new_weather_tag2.NamePtr = m_pWeather[0].WeatherTag2.NamePtr;
    m_NewBspGlobals.new_weather_tag2.TagId = m_pWeather[0].WeatherTag2.TagId;
    m_NewBspGlobals.new_weather_tag2.unknown = m_pWeather[0].WeatherTag2.unknown;
  }

  if(m_pShaders)
  {
    *(UINT*)&m_NewBspGlobals.new_shader_tag.tag = *(UINT*)&m_pShaders[0].ShaderTag.tag;
    m_NewBspGlobals.new_shader_tag.NamePtr = m_pShaders[0].ShaderTag.NamePtr;
    m_NewBspGlobals.new_shader_tag.TagId = m_pShaders[0].ShaderTag.TagId;
    m_NewBspGlobals.new_shader_tag.unknown = m_pShaders[0].ShaderTag.unknown;
  }
  
  if(m_pBackgroundSounds)
  {
    *(UINT*)&m_NewBspGlobals.new_background_sound_tag.tag = *(UINT*)&m_pBackgroundSounds[0].SoundTag.tag;
    m_NewBspGlobals.new_background_sound_tag.NamePtr = m_pBackgroundSounds[0].SoundTag.NamePtr;
    m_NewBspGlobals.new_background_sound_tag.TagId = m_pBackgroundSounds[0].SoundTag.TagId;
    m_NewBspGlobals.new_background_sound_tag.unknown = m_pBackgroundSounds[0].SoundTag.unknown;
  }

  if(m_pSoundEnv)
  {
    *(UINT*)&m_NewBspGlobals.new_env_sound_tag.tag = *(UINT*)&m_pSoundEnv[0].SoundTag.tag;
    m_NewBspGlobals.new_env_sound_tag.NamePtr = m_pSoundEnv[0].SoundTag.NamePtr;
    m_NewBspGlobals.new_env_sound_tag.TagId = m_pSoundEnv[0].SoundTag.TagId;
    m_NewBspGlobals.new_env_sound_tag.unknown = m_pSoundEnv[0].SoundTag.unknown;
  }

  m_NewBspGlobals.new_bsp_magic = m_BspMagic;
  m_NewBspGlobals.new_mapfile_magic = m_Magic;
  m_NewBspGlobals.short_lightmap_index = 0;
  m_NewBspGlobals.new_memory_base_address = m_pMesh[0].header.MemoryVertexOffset;

  CString outfilename = m_WorkingDir + "\\export_conv_globals.dat";
  CFile outfile;

  outfile.Open(outfilename, CFile::modeCreate|CFile::modeWrite);
  outfile.Write(&m_NewBspGlobals, sizeof(m_NewBspGlobals));
  outfile.Close();
}
