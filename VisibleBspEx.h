// VisibleBspEx.h: interface for the CVisibleBspEx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VISIBLEBSPEX_H__6C5D0B51_CC81_425E_857B_CF8B2A26F340__INCLUDED_)
#define AFX_VISIBLEBSPEX_H__6C5D0B51_CC81_425E_857B_CF8B2A26F340__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VisibleBsp.h"

class CVisibleBspEx : public CVisibleBsp
{
public:
	void SampleConversionGlobals(void);
	void SetNewConversionGlobals(void);
  UINT WriteDetailObjectsBlock(CFile* pFile, UINT start_offset, UINT new_magic);
  UINT WriteRuntimeDecalsBlock(CFile* pFile, UINT start_offset, UINT new_magic);
  UINT WriteSoundPASBlock(CFile* pFile, UINT start_offset, UINT new_magic);
  UINT WriteSoundBlocks(CFile* pFile, UINT start_offset, UINT new_magic);
  UINT WritePathfindingBlock(CFile* pFile, UINT start_offset, UINT new_magic);
  UINT WriteWeatherBlock(CFile* pFile, UINT start_offset, UINT new_magic);
  UINT WriteClusterDataAndPortals(CFile* pFile, UINT start_offset, UINT new_magic);
	void RefreshTestFile(void);
	void UpdateShaderReferences(void);
	UINT WriteNodesBlock(CFile* pFile, UINT start_offset, UINT new_magic);
	UINT WriteLeavesBlock(CFile* pFile, UINT start_offset, UINT new_magic);
	UINT WriteLeafSurfacesBlock(CFile* pFile, UINT start_offset, UINT new_magic);
  UINT WriteTriIndicesBlock(CFile* pFile, UINT start_offset, UINT new_magic);
  UINT WriteLightmapsBlock(CFile* pFile, UINT start_offset, UINT new_magic);
  UINT WriteClustersBlock(CFile* pFile, UINT start_offset, UINT new_magic);

  UINT WriteShaderBlock(CFile* pFile, UINT start_offset, UINT new_magic);
	void WriteHeader(CFile *pMapFile, UINT offset);
  UINT WriteVertexDataBlocks(CFile *pMapFile);
	void ConvertBsp();
	void Cleanup();
	CVisibleBspEx();
	virtual ~CVisibleBspEx();
  void ConvertMeshesToXboxFormat();

protected:
  BSP_SECTION_HEADER m_SectionHeader;
  REFLEXIVE *m_pReflexive_V;
  REFLEXIVE *m_pReflexive_LMV;
  BSP_HEADER m_NewBspHeader;

  BSP_SHADER *m_pNewShaders;

  struct STRUCT_CONVERSION_GLOBALS
  {
    TAG_REFERENCE new_lightmap_tag;
    TAG_REFERENCE new_weather_tag1;
    TAG_REFERENCE new_weather_tag2;
    TAG_REFERENCE new_shader_tag;
    short short_lightmap_index;
    UINT new_memory_base_address;
    TAG_REFERENCE new_background_sound_tag;
    TAG_REFERENCE new_env_sound_tag;
    UINT new_bsp_magic;
    UINT new_mapfile_magic;
  }m_NewBspGlobals;
};

#endif // !defined(AFX_VISIBLEBSPEX_H__6C5D0B51_CC81_425E_857B_CF8B2A26F340__INCLUDED_)
