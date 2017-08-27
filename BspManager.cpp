// BspManager.cpp: implementation of the CBspManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sparkedit.h"
#include "BspManager.h"
#include "TagManager.h"

extern CTagManager gTagManager;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*-------------------------------------------------------------------
 * Name: CBspManager()
 * Description:
 *   
 *-----------------------------------------------------------------*/
CBspManager::CBspManager()
{
  m_pMapFile = NULL;
  m_Magic = 0;
  m_pBspInfo = NULL;
  m_pBsp = NULL;
  m_ActiveBsp = 0;
  m_BspCount = 0;
  m_Version = 0;
}

/*-------------------------------------------------------------------
 * Name: ~CBspManager()
 * Description:
 *   
 *-----------------------------------------------------------------*/
CBspManager::~CBspManager()
{
  Cleanup();
}

/*-------------------------------------------------------------------
 * Name: Initialize()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CBspManager::Initialize(CFile *pMapFile, UINT magic)
{
  m_pMapFile = pMapFile;
  m_Magic = magic;
}

/*-------------------------------------------------------------------
 * Name: ActivateBsp()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CBspManager::ActivateBsp(int bsp_index)
{
  ASSERT(bsp_index < m_BspCount);
  m_ActiveBsp = bsp_index;
}

/*-------------------------------------------------------------------
 * Name: LoadVisibleBspInfo()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CBspManager::LoadVisibleBspInfo(REFLEXIVE BspChunk, UINT version)
{
  CString temp;
  UINT hdr;
  int i = 0;
  m_Version = version;

  if(BspChunk.Count > 0)
  {
    m_pBspInfo = new SCENARIO_BSP_INFO[BspChunk.Count];
    m_pBsp = new CVisibleBspEx[BspChunk.Count];
    m_pMapFile->Seek(BspChunk.Offset, 0);
    m_pMapFile->Read(m_pBspInfo, sizeof(SCENARIO_BSP_INFO)*BspChunk.Count);

    m_BspCount = BspChunk.Count;

    for(int i=0; i<BspChunk.Count; i++)
    {
      m_pBspInfo[i].Magic -= m_pBspInfo[i].BspStart;
      m_pBsp[i].Initialize(m_pMapFile, m_Magic, m_pBspInfo[i].Magic);

      temp = CheckForReference(m_pMapFile, m_pBspInfo[i].BspTag.NamePtr, m_Magic);
      m_BspNames.Add(temp);
      m_pMapFile->Seek(m_pBspInfo[i].BspStart, 0);
      m_pMapFile->Read(&hdr, sizeof(hdr));

      hdr -= m_pBspInfo[i].Magic;
      m_pBsp[i].LoadVisibleBsp(hdr, version);
      m_pBsp[i].SampleConversionGlobals();
    }

//    m_pBsp[0].ConvertBsp();
  }

/*
  UINT *pBuf = NULL;
  int bsp_index = 0;
  UINT BufSize = m_pBspInfo[bsp_index].BspSize/4;

  //AnalyzeFileSection(m_pMapFile, m_pBspInfo[0].BspStart, m_pBspInfo[0].BspSize, m_Magic);
  pBuf = new UINT[BufSize];
  m_pMapFile->Seek(m_pBspInfo[bsp_index].BspStart,0);
  m_pMapFile->Read(pBuf, BufSize*4);

  printf("\nAnalyzing BSP %d-------------------------------\n", bsp_index);
  gTagManager.FindMatchingSubTags(0, pBuf, BufSize);
  */
}

void CBspManager::Cleanup()
{
  for(int i=0; i<m_BspCount; i++)
  {
    m_pBsp[i].Cleanup();
  }

  if(m_pBspInfo)
    delete [] m_pBspInfo;
  m_pBspInfo = NULL;

  if(m_pBsp)
    delete [] m_pBsp;
  m_pBsp = NULL;

  m_BspNames.RemoveAll();
  m_ActiveBsp = 0;
  m_BspCount = 0;
  m_Version = 0;
}

void CBspManager::LoadBspSections()
{
  int i;

  for(i=0; i<m_BspCount; i++)
  {
  }
}

void CBspManager::GetActiveBspCentroid(float *x, float *y, float *z)
{
  *x = 0;
  *y = 0;
  *z = 0;

  if(m_pBsp)
    m_pBsp[m_ActiveBsp].GetMapCentroid(x, y, z);
}

SUBMESH_INFO* CBspManager::GetActiveBspSubmesh(UINT mesh_index)
{
  SUBMESH_INFO *pMesh = NULL;

  pMesh = &(m_pBsp[m_ActiveBsp].m_pMesh[mesh_index]);

  return(pMesh);
}

UINT CBspManager::GetActiveBspSubmeshCount()
{
  UINT mesh_count;

  if(m_pBsp)
    mesh_count = m_pBsp[m_ActiveBsp].m_SubMeshCount;
  else
    mesh_count = 0;

  return(mesh_count);
}

void CBspManager::ExportActiveBspToObj(CString path)
{
  if(m_pBsp)
  {
    if(m_Version == 5)
    {
      m_pBsp[m_ActiveBsp].ExportXboxMeshToObj(path);
    }
    else if(m_Version == 7)
    {
      m_pBsp[m_ActiveBsp].ExportPcMeshToObj(path);
    }
  }
}

CString CBspManager::GetActiveBspName()
{
  return(m_BspNames[m_ActiveBsp]);
}

CString CBspManager::GetMeshInfo(int mesh_index)
{
  CString info;

  if(m_pBsp)
    info = m_pBsp[m_ActiveBsp].GetMeshInfo(mesh_index);
  else
    info = "No BSP loaded";
  
  return(info);
}

void CBspManager::ReportBspInfo(CStdioFile *pFile)
{
  CString str;

  str = "\n-------BSP info-------\n";
  pFile->WriteString(str);

  for(int i=0; i<m_BspCount; i++)
  {
    str.Format("BSP %d Name:  %s\n", i, m_BspNames[i]);
    pFile->WriteString(str);

    str.Format("BSP %d Offset:          0x%08X\n", i, m_pBspInfo[i].BspStart);
    pFile->WriteString(str);

    str.Format("BSP %d Size:            0x%08X\n", i, m_pBspInfo[i].BspSize);
    pFile->WriteString(str);

    str.Format("BSP %d Magic:           0x%08X\n\n", i, m_pBspInfo[i].Magic);
    pFile->WriteString(str);
  }
}
