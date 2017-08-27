/*-------------------------------------------------------------------
 *  Original Author: Grenadiac
 *
 *  Contributing Authors:
 *
 *  File Description: 
 *    
 *
 *-----------------------------------------------------------------*/
#include "stdafx.h"
#include "SparkEdit.h"
#include "HaloMapFile.h"
#include "ShaderManager.h"
#include "Perspective.h"
#include "OutputPane.h"
#include "SelectionManager.h"
#include "Scenario.h"
#include "ToolView.h"
#include "BspManager.h"
#include "ModelManager.h"
#include "ItemColl.h"
#include "TagManager.h"
#include "WeaponHud.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CShaderManager gShaderManager;
extern CPerspective *g_pRenderWnd;
extern COutputPane *g_pOutput;
extern CSelectionManager gSelection;
extern CScenario gScenario;
extern CToolView *g_pTools;
extern CBspManager gBspManager;
extern CModelManager gModelManager;
extern CTagManager gTagManager;

/*-------------------------------------------------------------------
 * Name: CHaloMapFile()
 * Description:
 *   Constructor - initialize class vars here.
 *-----------------------------------------------------------------*/
CHaloMapFile::CHaloMapFile()
{
  m_pIndexItem = NULL;
  m_Magic = 0;

  _chdir("output");
}

/*-------------------------------------------------------------------
 * Name: ~CHaloMapFile()
 * Description:
 *   Delete allocated memory here.
 *-----------------------------------------------------------------*/
CHaloMapFile::~CHaloMapFile()
{
  CloseMapFile();
}

/*-------------------------------------------------------------------
 * Name: ParseCmdLine()
 * Description:
 *   Parse the command line for options and filenames.
 *-----------------------------------------------------------------*/
BOOL CHaloMapFile::ParseCmdLine(CString cmdline)
{
  return(FALSE);
}

/*-------------------------------------------------------------------
 * Name: ReadFileIndexing()
 * Description:
 *   Reads the file headers to determine data offsets.
 *-----------------------------------------------------------------*/
void CHaloMapFile::ReadFileIndexing()
{
  UINT junk;
  CString str;

  /* Read the main header */ 
  m_InputFile.Seek(0,0);
  m_InputFile.Read(&m_MapfileHdr, sizeof(m_MapfileHdr));

  /* Read the Index Header */ 
  m_InputFile.Seek(m_MapfileHdr.TagIndexOffset, 0);
  m_InputFile.Read(&m_IndexHdr, sizeof(m_IndexHdr));

  //m_InputFile.Seek(m_MapfileHdr.TagIndexOffset, 0);
  //m_InputFile.Write(&m_IndexHdr, sizeof(m_IndexHdr));

  /* Account for "magic" */ 
  if(m_MapfileHdr.Version == 5)
  {
    m_Magic = m_IndexHdr.index_magic - (m_MapfileHdr.TagIndexOffset + 36);	
  	m_IndexHdr.ModelRawDataOffset -= m_Magic;
	  m_IndexHdr.indices_offset -= m_Magic;
  }
  else if((m_MapfileHdr.Version == 7)||(m_MapfileHdr.Version == 609))
  {
    m_Magic = m_IndexHdr.index_magic - (m_MapfileHdr.TagIndexOffset + 40);	
  }


  /* Initialize analyzer */ 
  int min, max;

  min = m_Magic;
  max = m_InputFile.GetLength() + m_Magic;
  m_Analyzer.SetOffsetMinMax(min, max, m_Magic);


  /* Read Index Items */ 
  m_pIndexItem = new INDEX_ITEM[m_IndexHdr.tagcount];
  ASSERT(m_pIndexItem);
  
  if((m_MapfileHdr.Version == 7)||(m_MapfileHdr.Version == 609))  //move file pointer up a little (hack)
    m_InputFile.Read(&junk, 4);

  m_InputFile.Read(m_pIndexItem, sizeof(INDEX_ITEM)*m_IndexHdr.tagcount);
  m_IndexStr.SetSize(m_IndexHdr.tagcount);

  m_Analyzer.Initialize(&m_InputFile, m_Magic);
  m_Analyzer.SetIndexInfo(m_IndexHdr.tagcount, m_pIndexItem[0].tag_id);

  char temp[128];
  UINT tag_tmp = 0;
  UINT new_tag = 0;
  int k;

  for(int i=0; i<m_IndexHdr.tagcount; i++)
  {
    /* Convert offsets to account for magic */ 
	if (m_pIndexItem[i].BitmapType == 0)
	{
    m_pIndexItem[i].offset -= m_Magic;
	}
		if(m_pIndexItem[i].offset < 0)	/* for some reason, bsps is handled differently (offset=0) */ 
			m_pIndexItem[i].offset = 0;
		m_pIndexItem[i].stringoffset -= m_Magic;

    /* 'byte-flip' the tags */ 
    for(k=0;k<3;k++)
    {
      tag_tmp = m_pIndexItem[i].tagclass[k];
      new_tag = tag_tmp << 24;
      new_tag |= (tag_tmp & 0xFF000000) >> 24;
      new_tag |= (tag_tmp & 0x00FF0000) >> 8;
      new_tag |= (tag_tmp & 0x0000FF00) << 8;
      m_pIndexItem[i].tagclass[k] = new_tag;
    }
    /* read 'filename's */ 
    m_InputFile.Seek(m_pIndexItem[i].stringoffset,0);
    m_InputFile.Read(temp, 128);
    m_IndexStr[i] = temp;
    m_Analyzer.AddTagString(temp);

    //printf("Tag %5d  %s\n", i, temp);
  }
}


void CHaloMapFile::CreateObjectPath(CString path)
{

}

void CHaloMapFile::SearchForFloats()
{
  //m_Analyzer.SearchFileForNumber(&m_InputFile, 2427);
  m_Analyzer.SearchForIndexDistribution(&m_InputFile);
  //m_Analyzer.SearchForBspHeader(&m_InputFile);
  //m_Analyzer.SearchForFloats(&m_InputFile);
  //m_Analyzer.SearchForPattern(&m_InputFile);
  /*m_Analyzer.SearchFileForNumber(&m_InputFile, 2330);
  printf("\n");
  m_Analyzer.SearchFileForNumber(&m_InputFile, 2427);
  printf("\n");
  m_Analyzer.SearchFileForNumber(&m_InputFile, 2383);
  printf("\n");
  m_Analyzer.SearchFileForNumber(&m_InputFile, 2364);
  printf("\n");
  m_Analyzer.SearchFileForNumber(&m_InputFile, 2432);
  printf("\n");*/
}

void CHaloMapFile::ConvertFile()
{
  AnalyzeFileSection(&m_InputFile, 0, 0x200000, m_Magic, 0);
}

void CHaloMapFile::LoadShaders()
{
  char buf[256];
  UINT dmeta = 0;
  // init the shader manager with the map file and magic
  gShaderManager.Initialize(&m_InputFile, m_Magic, m_MapfileHdr.Version);
  gShaderManager.InitializeOpenGLExtensions();

  for(int i=0; i<m_IndexHdr.tagcount; i++)
  {
    m_InputFile.Seek(m_pIndexItem[i].stringoffset, 0);
    m_InputFile.Read(buf, 255);

    switch(m_pIndexItem[i].tagclass[0])
    {
    case TAG_BITMAP:
      //printf("%8X MetaOffset: %8X %s\n", m_pIndexItem[i].stringoffset, m_pIndexItem[i].offset, buf);

          gShaderManager.UpdateBitmapList(m_IndexStr[i], m_pIndexItem[i].stringoffset, 
                                      m_pIndexItem[i].offset, m_pIndexItem[i].tag_id,m_pIndexItem[i].BitmapType);

      break;
    }
  }
}

BOOL CHaloMapFile::OpenMapFile(CString path)
{
  CString str;
  BOOL bResult = FALSE;
  m_Magic = 0;
  int map_index = -1;

  /* Clean up the existing loaded map, if there is one */ 
  CloseMapFile();
  m_InputFile.Open(path, CFile::modeReadWrite|CFile::shareDenyWrite);

  /*
  CStdioFile patch;
  char *pStr;
  UINT temp, temp2;
  patch.Open("E:\\mods\\halo\\dev\\SparkEdit\\patchlist.txt", CFile::modeRead);
  temp2 = 0xFFFFFFFF;
  do
  {
    patch.ReadString(str);
    pStr = str.GetBuffer(20);
    sscanf(pStr, "%8X", &temp);
    str.ReleaseBuffer();

    m_InputFile.Seek(temp, 0);
    m_InputFile.Write(&temp2, 4);

  }while(str != "");

  patch.Close();
  

  str.Format("Opening file: %s\n", path);
  g_pOutput->PostText(str, LOG_BLUE);
  m_InputFilename = path;
  */
    
  ReadFileIndexing();
  //gShaderManager.LoadBitmapFile();  //loads the index for HaloCE bitmaps

  //LoadWeaponHuds();

  gTagManager.Initialize(&m_InputFile, m_Magic, m_IndexHdr.tagcount, m_pIndexItem[0].tag_id);
  gSelection.Initialize(m_MapfileHdr.Version);
  gBspManager.Initialize(&m_InputFile, m_Magic);
  LoadShaders();

  LoadScenery();
  LoadTagManager();
  LoadModels();
  gTagManager.CompileTagList();
  gTagManager.CompileReferences();
  gModelManager.CompileModelTextures();  //associate textures with models
  LoadScenario();

  //gTagManager.DumpTags();

  gShaderManager.BuildShaders();
  gShaderManager.LoadTextures();  //load textures last so we only load necessary bitmaps
  gShaderManager.CloseBitmapFile();


  gSelection.SetSelectionMode(NONE);
  g_pTools->Reset();
  g_pRenderWnd->RefreshWindow();
  bResult = TRUE;

  return(bResult);
}

void CHaloMapFile::Cleanup()
{
  if(m_pIndexItem)
    delete [] m_pIndexItem;

  if(m_InputFile.m_hFile != (HANDLE)0xffffffff)
    m_InputFile.Close();

  m_pIndexItem = NULL;
  m_Magic = 0;

}

void CHaloMapFile::OpenCompressedFile(CString path)
{
  BYTE *pInBuf = NULL;
  BYTE *pOutBuf = NULL;
  HANDLE hInFile, hOutFile, hInSection, hOutSection;
  MAPFILE_HDR *pHeader = NULL;
  DWORD InFileLen, InFileLenHigh, DecompLen;
  CString str, cachepath;
  int result = Z_MEM_ERROR;

  str.Format("Decompressing %s\n", path);
  g_pOutput->PostText(str, LOG_BLUE);
  g_pOutput->PostText("Please wait...\n", LOG_BLUE);
  g_pOutput->RedrawWindow();

  int k = sizeof(MAPFILE_HDR);
  // Use Filemapping to make the input file "look" like a normal buffer
  hInFile = CreateFile(path,
                       GENERIC_READ,
                       0,             // do not share
                       0,
                       OPEN_EXISTING,
                       FILE_ATTRIBUTE_NORMAL,
                       0);

  hInSection = CreateFileMapping(hInFile,
                                 0,
                                 PAGE_READONLY,
                                 0,   //this is not a large file
                                 0,   //map the entire file
                                 0);  //it does not need a name - no interprocess comm here :)

  pInBuf = (BYTE*)MapViewOfFile(hInSection,
                                FILE_MAP_READ,
                                0,
                                0,
                                0);

  if(pInBuf)
  {
    cachepath = m_WorkingDir + "\\cache.tmp";
    // Do the same for the output "buffer" (make an output file)
    hOutFile = CreateFile(cachepath,
                          GENERIC_READ|GENERIC_WRITE,
                          0,             // do not share
                          0,
                          CREATE_ALWAYS,
                          FILE_ATTRIBUTE_NORMAL,
                          0);

    pHeader = (MAPFILE_HDR*)pInBuf;

    hOutSection = CreateFileMapping(hOutFile,
                                    0,
                                    PAGE_READWRITE,
                                    0,   //this is not a large file
                                    pHeader->decomp_len+2048,   //the decompressed length of the output file
                                    0);  //it does not need a name - no interprocess comm here :)

    pOutBuf = (BYTE*)MapViewOfFile(hOutSection,
                                   FILE_MAP_WRITE,
                                   0,
                                   0,
                                   0);

    if(!pOutBuf)
    {
      g_pOutput->PostText("Could not expand cache.tmp.\r\nThere may not be enough hard drive space.\n", LOG_BLUE);
      AfxMessageBox("Could not expand cache.tmp.  There may not be enough hard drive space.\n");
    }
    else
    {
      CWaitCursor wait;

      InFileLen = GetFileSize(hInFile, &InFileLenHigh);
      DecompLen = pHeader->decomp_len;

      memcpy(pOutBuf, pHeader, sizeof(MAPFILE_HDR));

      result = uncompress(pOutBuf+2048,
                              &DecompLen,
                              pInBuf+2048,
                              InFileLen-2048);//InFileLen-2048);

      switch(result)
      {
      case Z_DATA_ERROR:
        g_pOutput->PostText("Failed to decompress - the file was corrupt.\n", LOG_RED);
        AfxMessageBox("Failed to decompress - the file was corrupt.");
        break;

      case Z_MEM_ERROR:
        g_pOutput->PostText("Failed to decompress - not enough memory.\n", LOG_RED);
        AfxMessageBox("Failed to decompress - not enough memory.");
        break;

      case Z_BUF_ERROR:
        g_pOutput->PostText("Failed to decompress - output buffer was not large enough.\n", LOG_RED);
        AfxMessageBox("Failed to decompress - output buffer was not large enough.");
        break;

      case Z_OK:
        g_pOutput->PostText("Decompression complete.\n", LOG_BLUE);
        AfxMessageBox("Successfully decompressed the Halo map file.");
        break;
      }
    }
    
    UnmapViewOfFile(pOutBuf);
    CloseHandle(hOutSection);
    CloseHandle(hOutFile);
  }

  UnmapViewOfFile(pInBuf);
  CloseHandle(hInSection);
  CloseHandle(hInFile);

  if(result == Z_OK)
    OpenMapFile(cachepath);
}

void CHaloMapFile::LoadScenario()
{
  UINT data_type;
  //find scenario data
  for(int TagIndex=0; TagIndex<m_IndexHdr.tagcount; TagIndex++)
  {
    data_type = m_pIndexItem[TagIndex].tagclass[0];

    if(data_type == TAG_SCENARIO)
    {
      gScenario.LoadScenario(&m_InputFile, 
                             m_Magic, 
                             m_pIndexItem[TagIndex].offset, 
                             m_MapfileHdr.Version,
                             (m_MapfileHdr.MapType == 1));
    }
  }
}

void CHaloMapFile::SaveChanges()
{
  if(m_MapfileHdr.MapType == 1)
    gScenario.SaveMpChanges();
  else
    gScenario.SaveChanges();

  //m_Bsp.SaveBspChanges(&m_InputFile);
}

void CHaloMapFile::CloseMapFile()
{
  /* Clean up the existing loaded map, if there is one */ 
  m_Analyzer.Cleanup();
  gBspManager.Cleanup();
  gShaderManager.Cleanup();
  gTagManager.Cleanup();
  gSelection.Cleanup();
  gModelManager.Cleanup();
  gScenario.Cleanup();
  Cleanup(); //cleanup after map file opened previously (if it exists)
}

void CHaloMapFile::LoadModels()
{
  char *pChar;
  CXboxModel *pXboxModel = NULL;
  CPcModel *pPcModel = NULL;

  gModelManager.Initialize(&m_InputFile, 
                           m_Magic, 
                           m_IndexHdr.ModelRawDataOffset,
                           m_IndexHdr.ModelRawDataOffset + m_IndexHdr.indices_offset, 
                           m_MapfileHdr.Version);
                                              
  for(int i=0; i<m_IndexHdr.tagcount; i++)
  {
    pChar = (char*)&m_pIndexItem[i].tagclass[0];

    if(m_pIndexItem[i].tagclass[0] == TAG_MODEL)
    {
      pXboxModel = gModelManager.AddXboxModel(m_pIndexItem[i].tag_id, &m_pIndexItem[i]);
      gTagManager.SetXboxModelTag(m_pIndexItem[i].tag_id, pXboxModel);
    }
    else if(m_pIndexItem[i].tagclass[0] == TAG_MODEL2)
    {
      pPcModel = gModelManager.AddPcModel(m_pIndexItem[i].tag_id, &m_pIndexItem[i]);
      gTagManager.SetPcModelTag(m_pIndexItem[i].tag_id, pPcModel);
    }
  }
}

void CHaloMapFile::LoadScenery()
{
  char *pChar;
  INDEX_ITEM *pCurrent, *pNext;
  int size;

  for(int i=0; i<m_IndexHdr.tagcount; i++)
  {
    pChar = (char*)&m_pIndexItem[i].tagclass[0];

    if(m_pIndexItem[i].tagclass[0] == TAG_SCENERY)
    {
      pCurrent = &m_pIndexItem[i];
      pNext = pCurrent + 1;
      
      size = pNext->offset - pCurrent->offset;
//      printf("=========================================================\n");
      printf("size = %08X   %s\n", size, m_IndexStr[i]);
//      printf("=========================================================\n");
      
//      m_Analyzer.SuperAnalyzeFileSection(pCurrent->offset, size);
    }
  }
}

void CHaloMapFile::LoadTagManager()
{
  g_pOutput->PostText("Compiling Tag Relationships...\n", LOG_BLUE);
  int y =0;
  for (int t = 0; t<m_IndexHdr.tagcount; t++)
  {
	  y = t;
	  if (m_pIndexItem[t].BitmapType == 1)
	  {
		  do 
		  {
			y++;
		  }while (m_pIndexItem[y].BitmapType == 1);
		  if (y == m_IndexHdr.tagcount)
		  {
			m_pIndexItem[t].offset = m_MapfileHdr.decomp_len;	
		  }
		  else
		  {
			m_pIndexItem[t].offset = m_pIndexItem[y].offset;
		  }
	  }
  }
  for(int i=0; i<m_IndexHdr.tagcount; i++)
  {
    gTagManager.InitTag(i, &m_pIndexItem[i]);
  }
}

BOOL CHaloMapFile::CompressMapFile(CString output_file)
{
  BYTE *pInBuf = NULL;
  BYTE *pOutBuf = NULL;
  HANDLE hInFile, hOutFile, hInSection, hOutSection;
  MAPFILE_HDR *pHeader = NULL;
  CString str, cachepath;
  int result = Z_MEM_ERROR;
  unsigned long out_len;
  int actual_file_size = 0;
  int desired_file_size = 0;
  int pad_bytes = 0;


  //Close open file so we can open it in memory-mapping mode
  
  out_len = m_InputFile.GetLength() - 2048;
  m_InputFile.Close();


  str.Format("Saving to compressed file %s\n", output_file);
  g_pOutput->PostText(str, LOG_BLUE);

  str.Format("Compressing %s\n", m_InputFilename);
  g_pOutput->PostText(str, LOG_BLUE);
  g_pOutput->PostText("Please wait...\n", LOG_BLUE);
  g_pOutput->RedrawWindow();

  int k = sizeof(MAPFILE_HDR);
  // Use Filemapping to make the input file "look" like a normal buffer
  hInFile = CreateFile(m_InputFilename,
                       GENERIC_READ,
                       0,             // do not share
                       0,
                       OPEN_EXISTING,
                       FILE_ATTRIBUTE_NORMAL,
                       0);

  hInSection = CreateFileMapping(hInFile,
                                 0,
                                 PAGE_READONLY,
                                 0,   //this is not a large file
                                 0,   //map the entire file
                                 0);  //it does not need a name - no interprocess comm here :)

  pInBuf = (BYTE*)MapViewOfFile(hInSection,
                                FILE_MAP_READ,
                                0,
                                0,
                                0);

  if(pInBuf)
  {
    cachepath = m_WorkingDir + "\\cache.tmp";
    // Do the same for the output "buffer" (make an output file)
    hOutFile = CreateFile(output_file,
                          GENERIC_READ|GENERIC_WRITE,
                          0,             // do not share
                          0,
                          CREATE_ALWAYS,
                          FILE_ATTRIBUTE_NORMAL,
                          0);

    pHeader = (MAPFILE_HDR*)pInBuf;

    hOutSection = CreateFileMapping(hOutFile,
                                    0,
                                    PAGE_READWRITE,
                                    0,   //this is not a large file
                                    pHeader->decomp_len+2048,   //the decompressed length of the output file
                                    0);  //it does not need a name - no interprocess comm here :)

    pOutBuf = (BYTE*)MapViewOfFile(hOutSection,
                                   FILE_MAP_WRITE,
                                   0,
                                   0,
                                   0);

    if(!pOutBuf)
    {
      g_pOutput->PostText("Could not create output file.\r\nThere may not be enough hard drive space.\n", LOG_BLUE);
      AfxMessageBox("Could not create output file.  There may not be enough hard drive space.\n");
      UnmapViewOfFile(pOutBuf);
      CloseHandle(hOutSection);
    }
    else
    {
      CWaitCursor wait;

      //write header
      memcpy(pOutBuf, &m_MapfileHdr, sizeof(m_MapfileHdr));

      //compress file data
      int ret;

      ret = compress(pOutBuf+2048,
                     &out_len,
                     pInBuf+2048,
                     pHeader->decomp_len-2048);

      TRACE("outlen = %08X\n", out_len);

      int actual_file_size = out_len + 2048;
      int desired_file_size;
      int pad_bytes;

      pad_bytes = (0x800 - (actual_file_size % 0x800));
      desired_file_size = actual_file_size + pad_bytes;

      //truncate file to desired_file_size
      UnmapViewOfFile(pOutBuf);
      CloseHandle(hOutSection);
      SetFilePointer(hOutFile, desired_file_size, 0, FILE_BEGIN);      
      SetEndOfFile(hOutFile);

		  if(ret != Z_OK)
		  {
        str.Format("Compress returned %i!\n", ret);
        g_pOutput->PostText(str, LOG_RED);
        AfxMessageBox("Compress Map Failed.");
		  }
    }
    
    CloseHandle(hOutFile);
  }

  UnmapViewOfFile(pInBuf);
  CloseHandle(hInSection);
  CloseHandle(hInFile);

  return(FALSE);
}

void CHaloMapFile::ReportMapInfo(CStdioFile *pFile)
{
  CString str;

  str = "-------Mapfile Header info-------\n";
  pFile->WriteString(str);
  str.Format("Map Name:  %s\n", m_MapfileHdr.Name);
  pFile->WriteString(str);
  str.Format("Map Version:  %d\n", m_MapfileHdr.Version);
  pFile->WriteString(str);
  str.Format("Build Date:  %s\n", m_MapfileHdr.BuildDate);
  pFile->WriteString(str);
  str.Format("Decompressed Length:    0x%08X\n", m_MapfileHdr.decomp_len);
  pFile->WriteString(str);
  str.Format("Tag Index Offset:       0x%08X\n", m_MapfileHdr.TagIndexOffset);
  pFile->WriteString(str);
  str.Format("Tag Index/Meta Length:  0x%08X\n", m_MapfileHdr.TagIndexMetaLength);
  pFile->WriteString(str);

  str.Format("Mapfile Magic:          0x%08X\n\n", m_Magic);
  pFile->WriteString(str);

  str = "-------Mapfile Index info-------\n";
  pFile->WriteString(str);
  str.Format("Base Tag:               0x%08X\n", m_IndexHdr.BaseTag);
  pFile->WriteString(str);
  str.Format("Tag Count:  %d\n", m_IndexHdr.tagcount);
  pFile->WriteString(str);
  str.Format("Model Raw Data Offset:  0x%08X\n", m_IndexHdr.ModelRawDataOffset);
  pFile->WriteString(str);
  str.Format("Model Indices Offset:   0x%08X\n", m_IndexHdr.indices_offset);
  pFile->WriteString(str);
  str.Format("Model Raw Data Size:    0x%08X\n", m_IndexHdr.ModelRawDataSize);
  pFile->WriteString(str);
}

void CHaloMapFile::LoadWeaponHuds()
{
  CWeaponHud weap_hud;

  weap_hud.Initialize(&m_InputFile, m_Magic);

  for(int i=0; i<m_IndexHdr.tagcount; i++)
  {
    if(m_pIndexItem[i].tagclass[0] == TAG_WEAPON_HUD_INTERFACE)
    {
      weap_hud.LoadWeaponHud(&m_pIndexItem[i]);
    }
  }
}

void CHaloMapFile::GetHeaders(MAPFILE_HDR *pMapfileHdr, INDEX_HDR *pIndexHdr)
{
  memcpy(pMapfileHdr, &m_MapfileHdr, sizeof(MAPFILE_HDR));
  memcpy(pIndexHdr, &m_IndexHdr, sizeof(INDEX_HDR));
}
