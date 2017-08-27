// ShaderManager.cpp: implementation of the CShaderManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ShaderManager.h"
#include "ProfileSettings.h"
#include "TagManager.h"
#include "glext.h"

extern COutputPane *g_pOutput;
extern CProfileSettings gProfile;
extern CTagManager gTagManager;

//typedef void (APIENTRY * PFNGLACTIVETEXTUREARBPROC) (GLenum target);
#define GL_GLEXT_PROTOTYPES

PFNGLACTIVETEXTUREARBPROC pglActiveTextureARB;
PFNGLMULTITEXCOORD2FVARBPROC pglMultiTexCoord2fvARB;
PFNGLMULTITEXCOORD2FARBPROC pglMultiTexCoord2fARB;
PFNGLCLIENTACTIVETEXTUREARBPROC pglClientActiveTextureARB;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShaderManager::CShaderManager()
{
  m_pMapFile = 0;
  m_Magic = 0;
  m_BitmapCount = 0;
  m_MapVersion = 0;
  m_pGlTextureNames = NULL;

  m_LightmapSetCount = 0;
  ZeroMemory(&m_LightmapCount, sizeof(m_LightmapCount));
  ZeroMemory(m_pGlLightmapNames, sizeof(m_pGlLightmapNames));
  int i = sizeof(m_pGlLightmapNames);
  ZeroMemory(&m_BitmapList, sizeof(m_BitmapList));
}

CShaderManager::~CShaderManager()
{
  Cleanup();
}


/*-------------------------------------------------------------------
 * Name: UpdateBitmapList()
 * Description:
 *   Builds a list of the current map's bitmaps.  This list contains
 *   the header information necessary for loading the bitmaps 
 *   and constructing the shaders.
 *-----------------------------------------------------------------*/
void CShaderManager::UpdateBitmapList(CString Name, UINT str_offset, UINT data_location, UINT bitmap_tag_id,int BitmapType)
{
	CEBITMAP_FILE_HDR ce_BitmapFileHeader;
	CEBITMAP_LOCATOR_ELEMENT ce_BitmapElement;
	switch (BitmapType)
	{
	case 0:
		if(m_BitmapCount < MAX_BITM_COUNT)
		{
		/* Read bmp header from metadata */ 
			m_pMapFile->Seek(data_location, 0);
			m_pMapFile->Read(&m_BitmapList[m_BitmapCount].hdr0, sizeof(bitm_header_t));

			//if(m_BitmapList[m_BitmapCount].hdr0.image_count > 12)
			//  TRACE("largecount\n");
    
			m_BitmapList[m_BitmapCount].hdr0.offset_to_first -= m_Magic;
			m_BitmapList[m_BitmapCount].hdr0.image_offset -= m_Magic;
    
			// Read 1st section
			m_pMapFile->Seek(m_BitmapList[m_BitmapCount].hdr0.offset_to_first,0);
			m_pMapFile->Read(&m_BitmapList[m_BitmapCount].hdr1, sizeof(bitm_first_t));
    
			/* Read 2nd section */ 
 
			//allocate sub-bitmap headers
			m_BitmapList[m_BitmapCount].pHeader2 = new bitm_image_t[m_BitmapList[m_BitmapCount].hdr0.image_count];

			m_pMapFile->Seek(m_BitmapList[m_BitmapCount].hdr0.image_offset,0);

			m_pMapFile->Read(m_BitmapList[m_BitmapCount].pHeader2, 
                     sizeof(bitm_image_t)*m_BitmapList[m_BitmapCount].hdr0.image_count);
        
			/* this reference will be used in the shader-bitmap search later */ 
			m_BitmapList[m_BitmapCount].BitmapNamePtr = str_offset;
    
			m_BitmapList[m_BitmapCount].BitmapTagId = bitmap_tag_id;
    
			strncpy(m_BitmapList[m_BitmapCount].bitmap_name, Name.GetBuffer(128), 128);
			Name.ReleaseBuffer();

			//TRACE("%s\n", Name);
			//
			//for(int i=0; i<m_BitmapList[m_BitmapCount].hdr0.image_count; i++)
			//{
			//  TRACE("%d\n", m_BitmapList[m_BitmapCount].pHeader2[i].size);
			//}
			//

			m_BitmapCount++;
		}

		if(m_BitmapCount >= MAX_BITM_COUNT)
		{
			g_pOutput->PostText("Bitmap list overrun...\n", LOG_RED);
		}
		break;
	case 1:
		if(m_BitmapCount < MAX_BITM_COUNT)
		{
			UINT BitmapIndex = data_location;
			m_BitmapList[m_BitmapCount].BitmapType = 1;

			m_BitmapFile.Seek(0,0);
			m_BitmapFile.Read(&ce_BitmapFileHeader,sizeof(ce_BitmapFileHeader));
			
			m_BitmapFile.Seek(ce_BitmapFileHeader.LocatorBlockOffset + sizeof(ce_BitmapElement) * BitmapIndex ,0);
			m_BitmapFile.Read(&ce_BitmapElement,sizeof(ce_BitmapElement));
			
			m_BitmapList[m_BitmapCount].BitmapNamePtr = ce_BitmapElement.name_offset + ce_BitmapFileHeader.NameBlockOffset;
			m_BitmapList[m_BitmapCount].BitmapTagId = bitmap_tag_id;
			
			m_BitmapFile.Seek(ce_BitmapElement.offset, 0);
			m_BitmapFile.Read(&m_BitmapList[m_BitmapCount].hdr0, sizeof(bitm_header_t));

			m_BitmapList[m_BitmapCount].hdr0.offset_to_first += ce_BitmapElement.offset;
			m_BitmapList[m_BitmapCount].hdr0.image_offset += ce_BitmapElement.offset;

			m_BitmapFile.Seek(m_BitmapList[m_BitmapCount].hdr0.offset_to_first,0);
			m_BitmapFile.Read(&m_BitmapList[m_BitmapCount].hdr1, sizeof(bitm_first_t));

			m_BitmapList[m_BitmapCount].pHeader2 = new bitm_image_t[m_BitmapList[m_BitmapCount].hdr0.image_count];

    		m_BitmapFile.Seek(m_BitmapList[m_BitmapCount].hdr0.image_offset,0);
			m_BitmapFile.Read(m_BitmapList[m_BitmapCount].pHeader2, 
                     sizeof(bitm_image_t)*m_BitmapList[m_BitmapCount].hdr0.image_count);
			

			strncpy(m_BitmapList[m_BitmapCount].bitmap_name, Name.GetBuffer(128), 128);
			Name.ReleaseBuffer();

			m_BitmapCount++;
			
		}
		if(m_BitmapCount >= MAX_BITM_COUNT)
		{
			g_pOutput->PostText("Bitmap list overrun...\n", LOG_RED);
		}
		break;
	}
}

void CShaderManager::Initialize(CFile *pMapFile, UINT magic, UINT version)
{
  CString bitmap_path;
  m_pMapFile = pMapFile;
  m_Magic = magic;
  m_MapVersion = version;

  m_ImageUtils.m_Version = version;

  switch (m_ImageUtils.m_Version)
  {
  case 0x7:
	  bitmap_path = gProfile.GetBitmapPath();
	  if(OpenBitmapFile(bitmap_path) == FALSE)
	  {
		AfxMessageBox("Unable to open HaloPC 'bitmaps.map' file.\r\nPlease check Tools->Options... and make sure the path is set correctly.");
	  }
	  break;
  case 0x261:
	  bitmap_path = gProfile.GetCEBitmapPath();
	  if(OpenBitmapFile(bitmap_path) == FALSE)
	  {
		AfxMessageBox("Unable to open HaloCE 'bitmaps.map' file.\r\nPlease check Tools->Options... and make sure the path is set correctly.");
	  }

	  break;
  }
}

/*-------------------------------------------------------------------
 * Name: BuildShaders()
 * Description:
 *   Fills in the missing information for each one of the shaders.
 *   For now, it just fills in the main bitmap header.  We will
 *   perform simple texturing, saving multitexturing effects such as 
 *   
 *-----------------------------------------------------------------*/
void CShaderManager::BuildShaders()
{
  int i;

  // allocate the texture names
  m_pGlTextureNames = new UINT[m_BitmapCount];

	glGenTextures(m_BitmapCount, m_pGlTextureNames);

  for(i=0; i<m_BitmapCount; i++)
  {
    m_BitmapList[i].GlTextureName = m_pGlTextureNames[i];
  }
}

void CShaderManager::Cleanup()
{
  int i;

  for(i=0; i<m_BitmapCount; i++)
  {
    if(m_BitmapList[i].pHeader2)
      delete m_BitmapList[i].pHeader2;

    if(m_BitmapList[i].pTextureData)
      delete [] m_BitmapList[i].pTextureData;
  }

	glDeleteTextures(m_BitmapCount, m_pGlTextureNames);
	
  for(i=0; i<m_LightmapSetCount; i++)
  {
    glDeleteTextures(m_LightmapCount[i], m_pGlLightmapNames[i]);
    m_LightmapCount[i] = 0;
        
    if(m_pGlLightmapNames[i])
      delete [] m_pGlLightmapNames[i];
    m_pGlLightmapNames[i] = NULL;
  }

  if(m_pGlTextureNames)
    delete [] m_pGlTextureNames;
  m_pGlTextureNames = NULL;
  
  m_MapVersion = 0;
  m_pMapFile = 0;
  m_Magic = 0;
  m_BitmapCount = 0;
  m_LightmapSetCount = 0;
  ZeroMemory(&m_BitmapList, sizeof(m_BitmapList));
}

BOOL CShaderManager::OpenBitmapFile(CString path)
{
  BOOL bStatus;
  bStatus = m_BitmapFile.Open(path, CFile::modeRead);

  return(bStatus);
}

void CShaderManager::CloseBitmapFile()
{
  if(m_BitmapFile.m_hFile != (HANDLE)0xffffffff)
    m_BitmapFile.Close();
}

void CShaderManager::LoadTextures()
{
  int i, j;
  CString str;
  BITMAP_STATUS texture_status;
	
  if(!((m_MapVersion == 7)&&(m_BitmapFile.m_hFile == (HANDLE)0xFFFFFFFF)))
  {
    for(i=0; i<m_BitmapCount; i++)
    {
      str.Format("Loading bitmap (tagid=%08X) %s\n", m_BitmapList[i].BitmapTagId, m_BitmapList[i].bitmap_name);
	  
      g_pOutput->PostText(str, LOG_GREY);

      texture_status = gTagManager.GetTextureStatus(m_BitmapList[i].BitmapTagId);

      if(texture_status == LIGHTMAP)
      {
        m_LightmapCount[m_LightmapSetCount] = m_BitmapList[i].hdr0.image_count;
        m_pGlLightmapNames[m_LightmapSetCount] = new UINT[m_LightmapCount[m_LightmapSetCount]];
	      glGenTextures(m_LightmapCount[m_LightmapSetCount], m_pGlLightmapNames[m_LightmapSetCount]);

        for(j=0; j<m_LightmapCount[m_LightmapSetCount]; j++)
        {
          glBindTexture(GL_TEXTURE_2D, m_pGlLightmapNames[m_LightmapSetCount][j]);
          LoadBitmap(i, j);
        }
        m_LightmapSetCount++;
      }
      else if(texture_status == BITMAP_ENABLED)
      {
        glBindTexture(GL_TEXTURE_2D, m_pGlTextureNames[i]);
        LoadBitmap(i, 0);
      }
    }
  }
}

void CShaderManager::ActivateShader(int texture_index)
{
  if(texture_index < m_BitmapCount)
    glBindTexture(GL_TEXTURE_2D, m_BitmapList[texture_index].GlTextureName);
}

void CShaderManager::LoadBitmap(UINT bitmap_index, UINT sub_bitmap)
{
  BYTE *texdata = NULL;

  //allocate buffers for read file and DXT decompression
  if(m_BitmapList[bitmap_index].pHeader2[sub_bitmap].size)
  {
    texdata = new BYTE[m_BitmapList[bitmap_index].pHeader2[sub_bitmap].size];
    m_BitmapList[bitmap_index].pTextureData = new UINT[m_BitmapList[bitmap_index].pHeader2[sub_bitmap].width * 
      m_BitmapList[bitmap_index].pHeader2[sub_bitmap].height];
    
    //if(m_MapVersion == 7)
    //{
      /* Read in raw texture data from halo map file */ 
    //  m_BitmapFile.Seek(m_BitmapList[bitmap_index].pHeader2[sub_bitmap].offset, 0);
    //  m_BitmapFile.Read(texdata, m_BitmapList[bitmap_index].pHeader2[sub_bitmap].size);
    //}
    //else if(m_MapVersion == 5)
    //{
    // m_pMapFile->Seek(m_BitmapList[bitmap_index].pHeader2[sub_bitmap].offset, 0);
    //  m_pMapFile->Read(texdata, m_BitmapList[bitmap_index].pHeader2[sub_bitmap].size);
    //}
	//else if(m_MapVersion == 0x261)
	//{
	//	m_pMapFile->Seek(m_BitmapList[bitmap_index].pHeader2[sub_bitmap].offset,0);
	//	m_pMapFile->Read(texdata, m_BitmapList[bitmap_index].pHeader2[sub_bitmap].size);
	//}
    
	switch (m_MapVersion)
	{
	case 0x7:
		m_BitmapFile.Seek(m_BitmapList[bitmap_index].pHeader2[sub_bitmap].offset, 0);
		m_BitmapFile.Read(texdata, m_BitmapList[bitmap_index].pHeader2[sub_bitmap].size);
		break;
	case 0x5:
		m_pMapFile->Seek(m_BitmapList[bitmap_index].pHeader2[sub_bitmap].offset, 0);
		m_pMapFile->Read(texdata, m_BitmapList[bitmap_index].pHeader2[sub_bitmap].size);
		break;
	case 0x261:
		switch (m_BitmapList[bitmap_index].BitmapType)
		{
			case 0:
				m_pMapFile->Seek(m_BitmapList[bitmap_index].pHeader2[sub_bitmap].offset, 0);
				m_pMapFile->Read(texdata, m_BitmapList[bitmap_index].pHeader2[sub_bitmap].size);
			break;
			case 1:
				m_BitmapFile.Seek(m_BitmapList[bitmap_index].pHeader2[sub_bitmap].offset, 0);
				m_BitmapFile.Read(texdata, m_BitmapList[bitmap_index].pHeader2[sub_bitmap].size);
			break;
		}
		break;
	};

    //decompress        
    m_ImageUtils.DecompressDXT(&(m_BitmapList[bitmap_index].pHeader2[sub_bitmap]), 
      m_BitmapList[bitmap_index].pHeader2[sub_bitmap].size,
      texdata, 
      m_BitmapList[bitmap_index].pTextureData);

    /*
    //channel detection code
    UINT swap;
    BYTE mask_data;
    int junk = m_BitmapList[bitmap_index].pHeader2[sub_bitmap].width*
               m_BitmapList[bitmap_index].pHeader2[sub_bitmap].height;
    for(int k=0; k<junk; k++)
    {
       swap = *((UINT*)&m_BitmapList[bitmap_index].pTextureData[k]);
       mask_data = (swap & 0xFF000000) >> 24;
       swap = mask_data;
       swap |= mask_data << 8;
       swap |= mask_data << 16;
       swap |= mask_data << 24;
       m_BitmapList[bitmap_index].pTextureData[k] = swap;
    }
    */
    
    // Build mipmaps for that fine blurry look.
    glTexImage2D(GL_TEXTURE_2D,
      0,
      GL_RGBA8,
      m_BitmapList[bitmap_index].pHeader2[sub_bitmap].width,
      m_BitmapList[bitmap_index].pHeader2[sub_bitmap].height,
      0,
      GL_BGRA_EXT,
      GL_UNSIGNED_BYTE,
      m_BitmapList[bitmap_index].pTextureData);
    
    delete [] m_BitmapList[bitmap_index].pTextureData;
    m_BitmapList[bitmap_index].pTextureData = NULL;
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    delete texdata;
    texdata = 0;
  }
  
}

void CShaderManager::ActivateLightmap(int bsp_index, int lightmap_index)
{
  if(lightmap_index < 0)
  {
    //TRACE("lightmap not used\n");
  }
  else if(lightmap_index < m_LightmapCount[bsp_index])
  {
    pglActiveTextureARB(GL_TEXTURE0_ARB);
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_REPLACE);
    glBindTexture(GL_TEXTURE_2D, m_pGlLightmapNames[bsp_index][lightmap_index]);
  }
}

void CShaderManager::ActivateLightmapBlend(int texture_index, int bsp_index, int lightmap_index)
{
  int count;
  glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &count);
  if(1)//TODO:  check for number of texturing units during initialization
  {
    //enable advanced texture blending extensions
    //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);

    //set up material texture stage
    if(texture_index < m_BitmapCount)
    {
      pglActiveTextureARB(GL_TEXTURE0_ARB);
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, m_BitmapList[texture_index].GlTextureName);
    }
    
    //set up lightmap texture stage
    if((lightmap_index < m_LightmapCount[bsp_index])&&(lightmap_index >= 0))
    {
      pglActiveTextureARB(GL_TEXTURE1_ARB);
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D, m_pGlLightmapNames[bsp_index][lightmap_index]);
    }
  }
}

void CShaderManager::DeactivateLightmapBlend()
{
  pglActiveTextureARB(GL_TEXTURE0_ARB);
  glDisable(GL_TEXTURE_2D);

  pglActiveTextureARB(GL_TEXTURE1_ARB);
  glDisable(GL_TEXTURE_2D);
}

void CShaderManager::InitializeOpenGLExtensions()
{
  char* extensionsList = (char*) glGetString(GL_EXTENSIONS);

  pglActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress("glActiveTextureARB");
  pglMultiTexCoord2fvARB = (PFNGLMULTITEXCOORD2FVARBPROC)wglGetProcAddress("glMultiTexCoord2fvARB");
  pglMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC)wglGetProcAddress("glMultiTexCoord2fARB");
  pglClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC)wglGetProcAddress("glClientActiveTextureARB");
}

void CShaderManager::ActivateSingleTexture(UINT texture_index)
{
  pglActiveTextureARB(GL_TEXTURE0_ARB);
  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_REPLACE);
  glBindTexture(GL_TEXTURE_2D, m_BitmapList[texture_index].GlTextureName);
}

void CShaderManager::DeactivateSingleTexture()
{
  pglActiveTextureARB(GL_TEXTURE0_ARB);
  glDisable(GL_TEXTURE_2D);
}

void CShaderManager::LoadBitmapFile()
{
  CFile bitmaps;
  BITMAP_FILE_HDR header;
  BITMAP_LOCATOR_ELEMENT *pLocator = NULL;
  char *pNameBlock = NULL;
  UINT name_size;

  bitmaps.Open("C:\\Program Files\\Microsoft Games\\HaloCE\\maps\\bitmaps.map", CFile::modeRead);
  bitmaps.Read(&header, 16);

  name_size = header.LocatorBlockOffset - header.NameBlockOffset;

  pLocator = new BITMAP_LOCATOR_ELEMENT[header.LocatorBlockCount];
  pNameBlock = new char[name_size];

  bitmaps.Seek(header.NameBlockOffset, 0);
  bitmaps.Read(pNameBlock, name_size);
  bitmaps.Read(pLocator, sizeof(BITMAP_LOCATOR_ELEMENT)*header.LocatorBlockCount);
  bitmaps.Close();

  for(int i=0; i<header.LocatorBlockCount; i++)
  {
    printf("%8X %8X %s\n", pLocator[i].offset, pLocator[i].size, &pNameBlock[pLocator[i].name_offset]);
  }

  if(pLocator)
    delete pLocator;

  if(pNameBlock)
    delete pNameBlock;
}

void CShaderManager::ActivateDetailBlend(int base_texture, int detail_tex1, int detail_tex2, int bsp_index)
{
  int count;
  glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &count);
  if(1)//TODO:  check for number of texturing units during initialization
  {
    //enable advanced texture blending extensions

    // REPLACE                 Arg0
    // MODULATE                Arg0 * Arg1
    // ADD                     Arg0 + Arg1
	  // ADD_SIGNED_EXT          Arg0 + Arg1 - 0.5
    // INTERPOLATE_EXT         Arg0 * (Arg2) + Arg1 * (1-Arg2)
    //
    //	PRIMARY_COLOR_EXT       primary color of incoming fragment
    //  TEXTURE                 texture color of corresponding texture unit
    //  CONSTANT_EXT            texture environment constant color
    //  PREVIOUS_EXT            result of previous texture environment; on
		//                          texture unit 0, this maps to PRIMARY_COLOR_EXT



    //set up material texture stage
    if(base_texture < m_BitmapCount)
    {
      pglActiveTextureARB(GL_TEXTURE0_ARB);
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, m_BitmapList[base_texture].GlTextureName);
      glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_REPLACE);
    }
    
    //set up detail 1
    if(0)//detail_tex1 < m_BitmapCount)
    {
      pglActiveTextureARB(GL_TEXTURE0_ARB);
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, m_BitmapList[detail_tex1].GlTextureName);      
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
      glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_INTERPOLATE_EXT);

      glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT, GL_TEXTURE);
      glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_EXT,GL_SRC_COLOR);
      
      glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_PREVIOUS_EXT);
      glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_EXT, GL_SRC_COLOR);
      
      glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_ALPHA_EXT, GL_TEXTURE2_ARB);
      glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_ALPHA_EXT, GL_ONE);

      glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB_EXT, GL_TEXTURE2_ARB);
      glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB_EXT, GL_ONE_MINUS_SRC_ALPHA);
      

      glMatrixMode(GL_TEXTURE);
 
			// Reset the current matrix and apply our chosen scale value
			glLoadIdentity();
			glScalef(100.0f, 100.0f, 1);
		  glMatrixMode(GL_MODELVIEW);

    }

    //set up detail 2
    if(0)//detail_tex2 < m_BitmapCount)
    {
      pglActiveTextureARB(GL_TEXTURE1_ARB);
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, m_BitmapList[detail_tex2].GlTextureName);
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
      glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_RGB_SCALE_EXT);
      glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB_EXT,GL_PREVIOUS_EXT);
      glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB_EXT,GL_SRC_COLOR);
      glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB_EXT,GL_TEXTURE);
      glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_RGB_EXT,GL_SRC_COLOR);

      glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_ALPHA_EXT, GL_TEXTURE2_ARB);
      glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_ALPHA_EXT, GL_ONE);
      glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB_EXT, GL_TEXTURE2_ARB);
      glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB_EXT, GL_ONE_MINUS_SRC_ALPHA);

      glMatrixMode(GL_TEXTURE);
 
			// Reset the current matrix and apply our chosen scale value
			glLoadIdentity();
			glScalef(100.0f, 100.0f, 1);
		  glMatrixMode(GL_MODELVIEW);
    }

    //set up blending function
//    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_INTERPOLATE_EXT);
      
//    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_ALPHA_EXT, GL_TEXTURE0_ARB);
//    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT, GL_TEXTURE1_ARB);
//    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_TEXTURE2_ARB);
//    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_ALPHA_EXT, GL_SRC_ALPHA);
//    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_EXT, GL_SRC_COLOR);
//    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_EXT, GL_SRC_COLOR);

  }
}
