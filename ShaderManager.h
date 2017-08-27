// ShaderManager.h: interface for the CShaderManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHADERMANAGER_H__A568CA2B_BB49_4672_93A8_BCCDA788E561__INCLUDED_)
#define AFX_SHADERMANAGER_H__A568CA2B_BB49_4672_93A8_BCCDA788E561__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Util.h"
#include "HaloBmp2.h"
#include "OutputPane.h"


#define MAX_SHADER_COUNT 100
#define MAX_BITM_COUNT 1000


typedef struct
{
   UINT unk1;
   UINT NameBlockOffset;
   UINT LocatorBlockOffset;
   UINT LocatorBlockCount;
}CEBITMAP_FILE_HDR;

typedef struct
{
   UINT name_offset;
   UINT size;   
   UINT offset;     
}CEBITMAP_LOCATOR_ELEMENT;

typedef struct
{
  UINT unknown1[0x22];
  UINT BitmapTag;
  UINT BitmapNamePtr;
  UINT unknown2[100];
}SHADER_ENV_DEF;

typedef struct
{
  bitm_header_t hdr0;
	bitm_first_t	hdr1;
  bitm_image_t *pHeader2;
  UINT BitmapNamePtr;
  UINT GlTextureName;
  UINT *pTextureData;
  BOOL bBitmapActive;
  UINT BitmapTagId;
  int BitmapType;
  char bitmap_name[128];
}BITMAP_INFO;

typedef struct
{
  SHADER_ENV_DEF def;
  char shader_name[128];
  char bitmap_name[128];
  UINT GlTextureName;
  UINT *pTextureData;
  UINT ShaderNamePtr;
}SHADER_ENV_INFO;


typedef struct
{
  UINT unknown1[19];
  UINT BitmapTag;
  UINT BitmapNamePtr;
  UINT unknown2[19];
}SHADER_MET_DEF;

typedef struct
{
  SHADER_MET_DEF def;
  char shader_name[128];
  char bitmap_name[128];
  BITMAP_INFO bitmap_info;
  UINT GlTextureName;
  UINT *pTextureData;
  UINT ShaderNamePtr;
}SHADER_MET_INFO;

typedef struct
{
  UINT unk[41];
  char tag1[4];
  UINT NamePtr;
  UINT unk1;
  UINT TagId;
  UINT unk2[2];
  char tag2[4];
  UINT unknown_end[100];
}SHADER_SOSO_DEF;

typedef struct STRUCT_SHADER_SOSO_INFO
{
  SHADER_SOSO_DEF def;
  char shader_name[128];
  char bitmap_name[128];
  BITMAP_INFO bitmap_info;
  UINT GlTextureName;
  UINT *pTextureData;
  UINT ShaderNamePtr;
}SHADER_SOSO_INFO;


class CShaderManager : public CUtil
{
public:
	void ActivateDetailBlend(int base_texture, int detail_tex1, int detail_tex2, int bsp_index);
	void LoadBitmapFile(void);
	void DeactivateSingleTexture(void);
	void ActivateSingleTexture(UINT texture_index);
	void InitializeOpenGLExtensions(void);
	void DeactivateLightmapBlend(void);
	void ActivateLightmapBlend(int texture_index, int bsp_index, int lightmap_index);
	void ActivateLightmap(int bsp_index, int lightmap_index);
	void ActivateShader(int texture_index);
	void LoadTextures(void);
	void CloseBitmapFile(void);
	BOOL OpenBitmapFile(CString path);
	void Cleanup(void);
	void BuildShaders(void);
	void Initialize(CFile *pMapFile, UINT magic, UINT version);
	void UpdateBitmapList(CString Name, UINT str_offset, UINT data_location, UINT bitmap_tag_id,int BitmapType);
	CShaderManager();
	virtual ~CShaderManager();

protected:
	void LoadBitmap(UINT bitmap_index, UINT sub_bitmap);
	UINT m_MapVersion;
  CFile *m_pMapFile;
  CFile m_BitmapFile;
  CHaloBmp m_ImageUtils;
  UINT m_Magic;
  
  UINT *m_pGlTextureNames;
  UINT *m_pGlLightmapNames[50];
  BITMAP_INFO m_BitmapList[MAX_BITM_COUNT];
  UINT m_BitmapCount;
  UINT m_LightmapCount[50];
  UINT m_LightmapSetCount;
};

#endif // !defined(AFX_SHADERMANAGER_H__A568CA2B_BB49_4672_93A8_BCCDA788E561__INCLUDED_)
