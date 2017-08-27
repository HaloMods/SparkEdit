// ModelManager.cpp: implementation of the CModelManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sparkedit.h"
#include "ModelManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include "HaloMapFile.h"
extern CHaloMapFile HaloMap;

CModelManager::CModelManager()
{
  m_XboxModelCount = 0;
  m_PcModelCount = 0;
  m_pMapFile = 0;

  ZeroMemory(m_TagIdList, sizeof(m_TagIdList));
}

CModelManager::~CModelManager()
{

}

void CModelManager::Cleanup()
{
  for(int i=0; i<m_PcModelCount; i++)
  {
    m_PcModels[i].Cleanup();
  }

  for(i=0; i<m_PcModelCount; i++)
  {
    m_XboxModels[i].Cleanup();
  }

  m_pMapFile = 0;
  m_PcModelCount = 0;
  m_XboxModelCount = 0;
  ZeroMemory(m_TagIdList, sizeof(m_TagIdList));
  m_Version = 0;
}

void CModelManager::Initialize(CFile *pMapFile, UINT magic, UINT vert_offset, UINT indices_offset, UINT version)
{
  m_pMapFile = pMapFile;
  m_Magic = magic;
  m_PcModelVertOffset = vert_offset;
  m_PcModelIndOffset = indices_offset;
  m_Version = version;
}

CPcModel* CModelManager::AddPcModel(UINT tag_id, INDEX_ITEM *pIndexItem)
{
  CString name;
  INDEX_ITEM *pNext;
  int size;
  CPcModel *pModel = NULL;

  if(m_PcModelCount < 500)
  {
    name = CheckForReference(m_pMapFile, pIndexItem->stringoffset, 0);
    m_PcModels[m_PcModelCount].m_TagId = tag_id;

    m_PcModels[m_PcModelCount].Initialize(m_pMapFile, 
      m_Magic, 
      m_PcModelVertOffset,
      m_PcModelIndOffset,
      name);
    
    pNext = pIndexItem + 1;
    size = pNext->offset - pIndexItem->offset;

    m_PcModels[m_PcModelCount].LoadModel(pIndexItem->offset, size);
    pModel = &m_PcModels[m_PcModelCount];

    m_TagIdList[m_PcModelCount] = pIndexItem->tag_id;
    m_PcModelCount++;
  }

  return(pModel);
}

CXboxModel* CModelManager::AddXboxModel(UINT tag_id, INDEX_ITEM *pIndexItem)
{
  CString name;
  INDEX_ITEM *pNext;
  int size;
  CXboxModel *pModel = NULL;

  if(m_XboxModelCount < 500)
  {
    name = CheckForReference(m_pMapFile, pIndexItem->stringoffset, 0);
    m_XboxModels[m_XboxModelCount].m_NamePtr = pIndexItem->stringoffset;

    m_XboxModels[m_XboxModelCount].Initialize(m_pMapFile, m_Magic, name);
    
    pNext = pIndexItem + 1;
    size = pNext->offset - pIndexItem->offset;
    
    m_XboxModels[m_XboxModelCount].LoadModel(pIndexItem->offset, size);
    pModel = &m_XboxModels[m_XboxModelCount];
    m_XboxModelCount++;
  }

  return(pModel);
}

void CModelManager::FindMatchingPcModel(UINT TopLevelObjTag, CPcModel **ppPcModel)
{
}

void CModelManager::FindMatchingXboxModel(UINT TopLevelObjTag, CXboxModel **ppXboxModel)
{
}

void CModelManager::CompileModelTextures()
{
  int i;

  for(i=0; i<m_PcModelCount; i++)
  {
    m_PcModels[i].CompileShader();
  }

  for(i=0; i<m_XboxModelCount; i++)
  {
    m_XboxModels[i].CompileShader();
  }
}

UINT CModelManager::GetModelCount()
{
  UINT model_count = 0;

  switch (m_Version)
  {
	  case 0x5:
		  model_count = m_XboxModelCount;
		  break;
	  case 0x7:
		  model_count = m_PcModelCount;
		  break;
	  case 0x261:
		  model_count = m_PcModelCount;
		  break;
  }	
  return(model_count);
}

void CModelManager::GetModel(int index, CPcModel **ppPcModel, CXboxModel **ppXboxModel)
{
  if(index < m_XboxModelCount)
    *ppXboxModel = &m_XboxModels[index];

  if(index < m_PcModelCount)
    *ppPcModel = &m_PcModels[index];
}

CString CModelManager::GetModelName(int index)
{
  CString str;

  switch (m_Version)
  {
	  case 0x5:
	      if(index < m_XboxModelCount)
		      str = m_XboxModels[index].m_ModelName;
		  break;
	  case 0x7:
		  if(index < m_PcModelCount)
			str = m_PcModels[index].m_ModelName;
		  break;
	  case 0x261:
	      if(index < m_PcModelCount)
		      str = m_PcModels[index].m_ModelName;
		  break;
  }		

  return(str);
}
