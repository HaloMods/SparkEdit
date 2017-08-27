// TagManager.cpp: implementation of the CTagManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sparkedit.h"
#include "TagManager.h"
#include "HaloMapFile.h"
#include "BspManager.h"

extern CHaloMapFile HaloMap;
extern CBspManager gBspManager;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//-------------------------------------------------------------------
//  Name: CTagManager()
//  Author(s): Grenadiac
//
//  Description:
//    Constructor - initialize class vars here.
//-------------------------------------------------------------------
CTagManager::CTagManager()
{
  m_pTagLookup = NULL;
  m_pMapFile = NULL;
  m_Magic = 0;
  m_TagCount = 0;
  m_BaseTag = 0;
  m_BitmapCount = 0;
}

//-------------------------------------------------------------------
//  Name: ~CTagManager()
//  Author(s): Grenadiac
//
//  Description:
//    Cleanup allocations, etc.
//-------------------------------------------------------------------
CTagManager::~CTagManager()
{
  Cleanup();
}

//-------------------------------------------------------------------
//  Name: Cleanup()
//  Author(s): Grenadiac
//
//  Description:
//    Public member function that can cleanup class resources before
//    the next map is loaded.
//-------------------------------------------------------------------
void CTagManager::Cleanup()
{
  if(m_pTagLookup)
    delete [] m_pTagLookup;
  m_pTagLookup = NULL;

  m_Magic = 0;
  m_TagCount = 0;
  m_BaseTag = 0;
  m_TagNames.RemoveAll();
  m_pMapFile = NULL;
  m_BitmapCount = 0;
}

//-------------------------------------------------------------------
//  Name: Initialize()
//  Author(s): Grenadiac
//
//  Description:
//    Init class vars, allocate tag resources.
//-------------------------------------------------------------------
void CTagManager::Initialize(CFile *pMapFile, UINT magic, UINT tag_count, UINT base_tag)
{
  m_TagCount = tag_count;
  m_BaseTag = base_tag;
  m_Magic = magic;
  m_pMapFile = pMapFile;

  m_pTagLookup = new TAG_LOOKUP[m_TagCount];
  ZeroMemory(m_pTagLookup, sizeof(TAG_LOOKUP)*m_TagCount);
}

//-------------------------------------------------------------------
//  Name: CheckForTag()
//  Author(s): Grenadiac
//
//  Description:
//    Used to verify that a tag_id is valid.  Used in tag search
//    functionality.
//-------------------------------------------------------------------
int CTagManager::CheckForTag(UINT val)
{
  
  int Tag = -1;
  /*
  USHORT val_l, val_h;

  val -= m_BaseTag;

  val_l = val & 0xFFFF;
  val_h = (val & 0xFFFF0000) >> 16;

  if((val_l == val_h)&&(val_l < m_TagCount))
  {
    Tag = val_l;
  }
  else if((val_l == (val_h-2))&&(val_l < m_TagCount))
  {
    Tag = val_l;
  }
  else if((val_l == (val_h-3))&&(val_l < m_TagCount))
  {
    Tag = val_l;
  }
  else if((val_l == (val_h-4))&&(val_l < m_TagCount))
  {
    Tag = val_l;
  }
*/

  if(val >= m_BaseTag)
  {
    for(int i=0; i<m_TagCount; i++)
    {
      if(m_pTagLookup[i].tag_id == val)
      {
        Tag = i;
        break;
      }
    }
  }
/*
      else if((m_pTagLookup[i].tag_id + 0x00010000) == val)
      {
        Tag = i;//(m_pTagLookup[i].tag_id - m_BaseTag) & 0xFFFF;
        break;
      }
      else if((m_pTagLookup[i].tag_id + 0x00020000) == val)
      {
        Tag = i;//(m_pTagLookup[i].tag_id - m_BaseTag) & 0xFFFF;
        break;
      }
      else if((m_pTagLookup[i].tag_id + 0x00030000) == val)
      {
        Tag = i;//(m_pTagLookup[i].tag_id - m_BaseTag) & 0xFFFF;
        break;
      }
      else if((m_pTagLookup[i].tag_id + 0x00040000) == val)
      {
        Tag = i;//(m_pTagLookup[i].tag_id - m_BaseTag) & 0xFFFF;
        break;
      }
*/
  return(Tag);
}

//-------------------------------------------------------------------
//  Name: GetTagPcModel()
//  Author(s): Grenadiac
//
//  Description:
//    Return a matching PC model pointer. If none are found, return NULL.
//-------------------------------------------------------------------
void CTagManager::GetTagPcModel(UINT tag_id, CPcModel **ppPcModel)
{
  int tag_index = CheckForTag(tag_id);
  *ppPcModel = NULL;

  if(tag_index >= 0)
  {
    *ppPcModel = m_pTagLookup[tag_index + 1].pPcModel;
  }
}

//-------------------------------------------------------------------
//  Name: GetTagPcModel()
//  Author(s): Grenadiac
//
//  Description:
//    Return a matching PC model pointer. If none are found, return NULL.
//-------------------------------------------------------------------
void CTagManager::GetTagXboxModel(UINT tag_id, CXboxModel **ppXboxModel)
{
  int tag_index = CheckForTag(tag_id);
  *ppXboxModel = NULL;

  if(tag_index >= 0)
  {
    *ppXboxModel = m_pTagLookup[tag_index + 1].pXboxModel;
  }
}

//-------------------------------------------------------------------
//  Name: InitTag()
//  Author(s): Grenadiac
//
//  Description:
//    Called during map loading to set up the Tag Lookup table.
//    This is called once per tag.
//-------------------------------------------------------------------
void CTagManager::InitTag(int index, INDEX_ITEM *pItem)
{
  ASSERT((index >= 0)&&(index <m_TagCount));

  char temp[128];
  INDEX_ITEM *pNext = pItem + 1;

  // set the tag class(es)
  m_pTagLookup[index].tagclass[0] = pItem->tagclass[0];
  m_pTagLookup[index].tagclass[1] = pItem->tagclass[1];
  m_pTagLookup[index].tagclass[2] = pItem->tagclass[2];
  m_pTagLookup[index].tag_id = pItem->tag_id;
  m_pTagLookup[index].meta_offset = pItem->offset;
  m_pTagLookup[index].RawNameOffset = pItem->stringoffset + m_Magic;
  if(index == (m_TagCount - 1))
  {
    m_pTagLookup[index].meta_size = 0;
  }
  else
  {
	m_pTagLookup[index].meta_size = pNext->offset - pItem->offset;
  }

  // get the tag string
  m_pMapFile->Seek(pItem->stringoffset,0);
  m_pMapFile->Read(temp, 128);
  
  strncpy(m_pTagLookup[index].name, temp, 127);
  m_TagNames.Add(temp);
}

//-------------------------------------------------------------------
//  Name: GetTagPhysics()
//  Author(s): Grenadiac
//
//  Description:
//    Future placeholder.  Gets the associated physics for vehicle tag, etc.
//-------------------------------------------------------------------
void CTagManager::GetTagPhysics(UINT tag_id)
{
  // future expansion
}

//-------------------------------------------------------------------
//  Name: CompileTagList()
//  Author(s): Grenadiac
//
//  Description:
//    Called after the tags are loaded.  Finds matching subcomponents
//    of tags, such as models.
//-------------------------------------------------------------------
void CTagManager::CompileTagList()
{
  int tag;
  UINT *pTemp = NULL;

  for(tag=0; tag<m_TagCount; tag++)
  {
    //Index the bitmaps so they match up with the Shader Manager
    if(m_pTagLookup[tag].tagclass[0] == TAG_BITMAP)
    {
      m_pTagLookup[tag].BitmapIndex = m_BitmapCount;
      m_BitmapCount++;
    }

    if((m_pTagLookup[tag].tagclass[1] == TAG_SHADER)||
       (m_pTagLookup[tag].tagclass[1] == TAG_OBJECT)||
       (m_pTagLookup[tag].tagclass[1] == TAG_ITEM))
    {
      //make sure that the meta size is valid
      if(m_pTagLookup[tag].meta_size < 100000)
      {
        pTemp = new UINT[m_pTagLookup[tag].meta_size/4];

        //analyze the meta for sub-tags
        m_pMapFile->Seek(m_pTagLookup[tag].meta_offset,0);
        m_pMapFile->Read(pTemp, m_pTagLookup[tag].meta_size);

        FindMatchingSubTags(tag, pTemp, m_pTagLookup[tag].meta_size/4);

        delete pTemp;
        pTemp = NULL;
      }
    }
  }
}

//-------------------------------------------------------------------
//  Name: FindMatchingSubTags()
//  Author(s): Grenadiac
//
//  Description:
//    Searches through a buffer to find model and other subtags.
//-------------------------------------------------------------------
void CTagManager::FindMatchingSubTags(int tag_index, UINT *pSearchBuf, UINT count)
{
  int i;
  int tag_id;

  for(i=0; i<count; i++)
  {
    tag_id = CheckForTag(pSearchBuf[i]);

    //SENV shaders have a strange offset in their tag_id
    if(m_pTagLookup[tag_index].tagclass[0] == TAG_SHADER_ENVIRONMENT)
    {
      if((pSearchBuf[i] & 0xF0000000)==0xE0000000)
        int test = 0;
      //tag_id = CheckForTag(pSearchBuf[i] - 0x20000);
    }


    //see if tag is the type we are looking for
    if((tag_id >= 0)&&(tag_id < m_TagCount))
    {
      if(0)//m_pTagLookup[tag_index].tagclass[1] == TAG_SHADER)
      {
        //printf("-------------------------------------------------------------------\n");
        printf("%c%c%c%c: (%04X) %s  (%08X) (%08X) Sub-Tag (%c%c%c%c): %s\n", 
          ((char*)&m_pTagLookup[tag_index].tagclass[0])[0],
          ((char*)&m_pTagLookup[tag_index].tagclass[0])[1],
          ((char*)&m_pTagLookup[tag_index].tagclass[0])[2],
          ((char*)&m_pTagLookup[tag_index].tagclass[0])[3],
          tag_index, m_TagNames[tag_index], pSearchBuf[i], i*4+0x800,  
          ((char*)&m_pTagLookup[tag_id].tagclass[0])[0],
          ((char*)&m_pTagLookup[tag_id].tagclass[0])[1],
          ((char*)&m_pTagLookup[tag_id].tagclass[0])[2],
          ((char*)&m_pTagLookup[tag_id].tagclass[0])[3],
          m_TagNames[tag_id]);
      }

      //if(m_TagNames[tag_id].Find("levels\\b40\\b40_a", 0) >=0)
      //  printf("%08X\n", i*4+0x800);
      
      if(m_pTagLookup[tag_id].tagclass[0] == TAG_MODEL2)
      {
        m_pTagLookup[tag_index].ModelTag = pSearchBuf[i];//tag_id + m_BaseTag + (tag_id << 16);
        m_pTagLookup[tag_index].pPcModel = m_pTagLookup[tag_id].pPcModel;
      }
      
      else if(m_pTagLookup[tag_id].tagclass[0] == TAG_MODEL)
      {
        m_pTagLookup[tag_index].ModelTag = pSearchBuf[i];//tag_id + m_BaseTag + (tag_id << 16);
        m_pTagLookup[tag_index].pXboxModel = m_pTagLookup[tag_id].pXboxModel;
      }
      
      else if(m_pTagLookup[tag_id].tagclass[0] == TAG_PHYSICS)
        m_pTagLookup[tag_index].PhysicsTag = pSearchBuf[i];//tag_id + m_BaseTag + (tag_id << 16);

      else if(m_pTagLookup[tag_id].tagclass[0] == TAG_COLLISION_MODEL)
        m_pTagLookup[tag_index].CollisionModelTag = pSearchBuf[i];//tag_id + m_BaseTag + (tag_id << 16);

      else if(m_pTagLookup[tag_id].tagclass[1] == TAG_SHADER_MODEL)
        m_pTagLookup[tag_index].ShaderTag = pSearchBuf[i];//tag_id + m_BaseTag + (tag_id << 16);

      else if(m_pTagLookup[tag_id].tagclass[0] == TAG_WEAPON)
        m_pTagLookup[tag_index].WeaponTag = pSearchBuf[i];//tag_id + m_BaseTag + (tag_id << 16);

      //fill in the shaders
      if(m_pTagLookup[tag_index].tagclass[1] == TAG_SHADER)
      {
        if(m_pTagLookup[tag_id].tagclass[0] == TAG_BITMAP)
        {
          if(m_pTagLookup[tag_index].BaseTextureTag == 0)
            m_pTagLookup[tag_index].BaseTextureTag = pSearchBuf[i];//tag_id + m_BaseTag + (tag_id << 16);

          else if(m_pTagLookup[tag_index].MultiTexture[0] == 0)
            m_pTagLookup[tag_index].MultiTexture[0] = pSearchBuf[i];//tag_id + m_BaseTag + (tag_id << 16);
          
          else if(m_pTagLookup[tag_index].MultiTexture[1] == 0)
            m_pTagLookup[tag_index].MultiTexture[1] = pSearchBuf[i];//tag_id + m_BaseTag + (tag_id << 16);
          
          else if(m_pTagLookup[tag_index].MultiTexture[2] == 0)
            m_pTagLookup[tag_index].MultiTexture[2] = pSearchBuf[i];//tag_id + m_BaseTag + (tag_id << 16);
          
          else if(m_pTagLookup[tag_index].MultiTexture[3] == 0)
            m_pTagLookup[tag_index].MultiTexture[3] = pSearchBuf[i];//tag_id + m_BaseTag + (tag_id << 16);
          
          else if(m_pTagLookup[tag_index].MultiTexture[4] == 0)
            m_pTagLookup[tag_index].MultiTexture[4] = pSearchBuf[i];//tag_id + m_BaseTag + (tag_id << 16);
        }
      }
    }

  }
}


//-------------------------------------------------------------------
//  Name: SetXboxModelTag()
//  Author(s): Grenadiac
//
//  Description:
//    Called to init the model pointers during model loading.
//-------------------------------------------------------------------
void CTagManager::SetXboxModelTag(UINT tag_id, CXboxModel *pXboxModel)
{
  int tag_index;

  tag_index = CheckForTag(tag_id);

  if(tag_index >= 0)
  {
    m_pTagLookup[tag_index].pXboxModel = pXboxModel;
  }
}

//-------------------------------------------------------------------
//  Name: SetPcModelTag()
//  Author(s): Grenadiac
//
//  Description:
//    Called to init the model pointers during model loading.
//-------------------------------------------------------------------
void CTagManager::SetPcModelTag(UINT tag_id, CPcModel *pPcModel)
{
  int tag_index;

  tag_index = CheckForTag(tag_id);

  if(tag_index >= 0)
  {
    m_pTagLookup[tag_index].pPcModel = pPcModel;
  }
}

//-------------------------------------------------------------------
//  Name: DumpTags()
//  Author(s): Grenadiac
//
//  Description:
//    Called to init the model pointers during model loading.
//-------------------------------------------------------------------
void CTagManager::DumpTags()
{
  int tag;
  int count;

  for(tag=0; tag<m_TagCount; tag++)
  {

    count = 0;

    if(m_pTagLookup[tag].ModelTag)count++;
    if(m_pTagLookup[tag].CollisionModelTag)count++;
    if(m_pTagLookup[tag].ShaderTag)count++;
    if(m_pTagLookup[tag].BaseTextureTag)count++;
    if(m_pTagLookup[tag].PhysicsTag)count++;
    if(m_pTagLookup[tag].WeaponTag)count++;
    if(m_pTagLookup[tag].EquipmentTag)count++;
    if(m_pTagLookup[tag].pPcModel)count++;    
    if(m_pTagLookup[tag].pXboxModel)count++;

    if(count == 1)
      printf("     ----------------------------------------------------------------\n");
    else if(count > 1)
      printf("=====================================================================\n");

    printf("%4X  %c%c%c%c  %c%c%c%c  %c%c%c%c  %s\n", tag, 
                                   ((char*)&m_pTagLookup[tag].tagclass[0])[0],
                                   ((char*)&m_pTagLookup[tag].tagclass[0])[1],
                                   ((char*)&m_pTagLookup[tag].tagclass[0])[2],
                                   ((char*)&m_pTagLookup[tag].tagclass[0])[3],
                                   ((char*)&m_pTagLookup[tag].tagclass[1])[0],
                                   ((char*)&m_pTagLookup[tag].tagclass[1])[1],
                                   ((char*)&m_pTagLookup[tag].tagclass[1])[2],
                                   ((char*)&m_pTagLookup[tag].tagclass[1])[3],
                                   ((char*)&m_pTagLookup[tag].tagclass[2])[0],
                                   ((char*)&m_pTagLookup[tag].tagclass[2])[1],
                                   ((char*)&m_pTagLookup[tag].tagclass[2])[2],
                                   ((char*)&m_pTagLookup[tag].tagclass[2])[3],
                                   m_TagNames[tag]);

    printf("     meta_offset:     %08X\n", m_pTagLookup[tag].meta_offset);
    printf("     meta_size:       %08X\n", m_pTagLookup[tag].meta_size);

    if(m_pTagLookup[tag].ModelTag)
      printf("     ModelTag:        %08X\n", m_pTagLookup[tag].ModelTag);
    
    if(m_pTagLookup[tag].CollisionModelTag)
      printf("     CollisionTag:    %08X\n", m_pTagLookup[tag].CollisionModelTag);
    
    if(m_pTagLookup[tag].ShaderTag)
      printf("     ShaderTag:       %08X\n", m_pTagLookup[tag].ShaderTag);
    
    if(m_pTagLookup[tag].BaseTextureTag)
      printf("     BaseTextureTag:  %08X, %s\n", m_pTagLookup[tag].BaseTextureTag, 
                                                 GetTagName(m_pTagLookup[tag].BaseTextureTag));
    
    if(m_pTagLookup[tag].PhysicsTag)
      printf("     PhysicsTag:      %08X\n", m_pTagLookup[tag].PhysicsTag);
    
    if(m_pTagLookup[tag].WeaponTag)
      printf("     WeaponTag:       %08X\n", m_pTagLookup[tag].WeaponTag);
    
    if(m_pTagLookup[tag].EquipmentTag)
      printf("     EquipmentTag:    %08X\n", m_pTagLookup[tag].EquipmentTag);
    
    if(m_pTagLookup[tag].pPcModel)
      printf("     CPcModel:        %08X\n", m_pTagLookup[tag].pPcModel);
    
    if(m_pTagLookup[tag].pXboxModel)
      printf("     CXboxModel:      %08X\n", m_pTagLookup[tag].pXboxModel);

    //if(m_pTagLookup[tag].bBitmapActive)
    //  printf("     Bitmap Active\n");

    printf("\n");
  }
}

//-------------------------------------------------------------------
//  Name: SetPcModelTag()
//  Author(s): Grenadiac
//
//  Description:
//    Called to init the model pointers during model loading.
//-------------------------------------------------------------------
void CTagManager::CompileReferences()
{
  int tag_index;
  int tag;
  UINT *pTemp = NULL;

  for(tag=0; tag<m_TagCount; tag++)
  {
    if(m_pTagLookup[tag].tagclass[0] == TAG_WEAPON)
    {
      tag_index = CheckForTag(m_pTagLookup[tag].WeaponTag);

      if(tag_index >= 0)
      {
        TRACE("here");
        //m_pTagLookup[tag_index].pPcModel = pPcModel;
      }
    }
  }
}

//-------------------------------------------------------------------
//  Name: GetBaseTextureIndex()
//  Author(s): Grenadiac
//
//  Description:
//    Called by Model Manager and Visible BSP to get a bitmap index
//    based on the shader.  By calling this function, the bitmap tag
//    is marked as active and the bitmap will be loaded
//-------------------------------------------------------------------
int CTagManager::GetBaseTextureIndex(UINT shader_tag_id)
{
  int shader_tag_index = -1;
  UINT texture_tag;
  int texture_index = -1;
  int tag_index = CheckForTag(shader_tag_id);

  if(tag_index >= 0)
  {
    //TRACE("Getting base texture for %s (%08X)\n", m_TagNames[tag_index], shader_tag_id);
    texture_tag = m_pTagLookup[tag_index].BaseTextureTag;
    
    shader_tag_index = CheckForTag(texture_tag);
    if(shader_tag_index >= 0)
    {
      m_pTagLookup[shader_tag_index].BitmapStatus = BITMAP_ENABLED;
      texture_index = m_pTagLookup[shader_tag_index].BitmapIndex;
      //TRACE("Activated %s\n", GetTagName(texture_tag));
    }
  }
  return(texture_index);
}

void CTagManager::GetShaderTextures(UINT shader_tag_id, 
                                    int &base_texture_index, 
                                    int &detail1_texture_index, 
                                    int &detail2_texture_index)
{
  int shader_tag_index = -1;
  int texture_index = -1;
  int tag_index = CheckForTag(shader_tag_id);

  if(tag_index >= 0)
  {
    //base texture
    shader_tag_index = CheckForTag(m_pTagLookup[tag_index].BaseTextureTag);
    if(shader_tag_index >= 0)
    {
      m_pTagLookup[shader_tag_index].BitmapStatus = BITMAP_ENABLED;
      base_texture_index = m_pTagLookup[shader_tag_index].BitmapIndex;
    }

    //detail 1
    shader_tag_index = CheckForTag(m_pTagLookup[tag_index].MultiTexture[0]);
    if(shader_tag_index >= 0)
    {
      m_pTagLookup[shader_tag_index].BitmapStatus = BITMAP_ENABLED;
      detail1_texture_index = m_pTagLookup[shader_tag_index].BitmapIndex;
    }

    // detail 2
    shader_tag_index = CheckForTag(m_pTagLookup[tag_index].MultiTexture[1]);
    if(shader_tag_index >= 0)
    {
      m_pTagLookup[shader_tag_index].BitmapStatus = BITMAP_ENABLED;
      detail2_texture_index = m_pTagLookup[shader_tag_index].BitmapIndex;
    }
  }
}

//-------------------------------------------------------------------
//  Name: GetTagName()
//  Author(s): Grenadiac
//
//  Description:
//    Returns the string associated with the tag name.
//-------------------------------------------------------------------
CString CTagManager::GetTagName(UINT tag_id)
{
  int tag_index = CheckForTag(tag_id);
  CString name;

  if((tag_index >= 0)&&(tag_index < m_TagNames.GetSize()))
  {
    name = m_TagNames[tag_index];
  }

  return(name);
}

BITMAP_STATUS CTagManager::GetTextureStatus(int bitmap_tagid)
{
  BITMAP_STATUS BitmapStatus = BITMAP_DISABLED;
  
  int tag_index = CheckForTag(bitmap_tagid);

  if(tag_index >= 0)
  {
    BitmapStatus = m_pTagLookup[tag_index].BitmapStatus;
	//BitmapStatus = BITMAP_ENABLED;//m_pTagLookup[tag_index].BitmapStatus;
  }

  return(BitmapStatus);
}

CString CTagManager::GetTagDescription(UINT tag_id)
{
  CString desc, temp;
  CString model_subtag_info;
  CString shader_subtag_info;

  int tag_index = CheckForTag(tag_id);

  if(tag_index >= 0)
  {
    //perform recursive tag lookup
    desc+= "------------------------------------------------------\r\n";
    temp.Format("Tag ID: %08X %c%c%c%c  %c%c%c%c  %c%c%c%c  %s\r\n", tag_id, 
                                   ((char*)&m_pTagLookup[tag_index].tagclass[0])[0],
                                   ((char*)&m_pTagLookup[tag_index].tagclass[0])[1],
                                   ((char*)&m_pTagLookup[tag_index].tagclass[0])[2],
                                   ((char*)&m_pTagLookup[tag_index].tagclass[0])[3],
                                   ((char*)&m_pTagLookup[tag_index].tagclass[1])[0],
                                   ((char*)&m_pTagLookup[tag_index].tagclass[1])[1],
                                   ((char*)&m_pTagLookup[tag_index].tagclass[1])[2],
                                   ((char*)&m_pTagLookup[tag_index].tagclass[1])[3],
                                   ((char*)&m_pTagLookup[tag_index].tagclass[2])[0],
                                   ((char*)&m_pTagLookup[tag_index].tagclass[2])[1],
                                   ((char*)&m_pTagLookup[tag_index].tagclass[2])[2],
                                   ((char*)&m_pTagLookup[tag_index].tagclass[2])[3],
                                   m_TagNames[tag_index]);
    desc += temp;
    
    temp.Format("Meta Offset: %08X\r\n", m_pTagLookup[tag_index].meta_offset);
    desc += temp;
    
    temp.Format("Meta Size: %08X\r\n", m_pTagLookup[tag_index].meta_size);
    desc += temp;

    if(m_pTagLookup[tag_index].ModelTag)
    {
      temp.Format("ModelTag: %08X  %s\r\n", 
        m_pTagLookup[tag_index].ModelTag, GetTagName(m_pTagLookup[tag_index].ModelTag));
      desc += temp;

      model_subtag_info += GetTagDescription(m_pTagLookup[tag_index].ModelTag);
    }
    
    if(m_pTagLookup[tag_index].CollisionModelTag)
    {
      temp.Format("CollisionModelTag: %08X  %s\r\n", 
        m_pTagLookup[tag_index].CollisionModelTag, GetTagName(m_pTagLookup[tag_index].CollisionModelTag));
      desc += temp;
    }
    
    if(m_pTagLookup[tag_index].ShaderTag)
    {
      temp.Format("ShaderTag: %08X  %s\r\n", 
        m_pTagLookup[tag_index].ShaderTag, GetTagName(m_pTagLookup[tag_index].ShaderTag));
      desc += temp;

      shader_subtag_info += GetTagDescription(m_pTagLookup[tag_index].ShaderTag);
    }
    
    if(m_pTagLookup[tag_index].BaseTextureTag)
    {
      temp.Format("BaseTextureTag: %08X  %s\r\n", 
        m_pTagLookup[tag_index].BaseTextureTag, GetTagName(m_pTagLookup[tag_index].BaseTextureTag));
      desc += temp;
    }
    
    if(m_pTagLookup[tag_index].MultiTexture[0])
    {
      temp.Format("MultiTextureTag: %08X  %s\r\n", 
        m_pTagLookup[tag_index].MultiTexture[0], GetTagName(m_pTagLookup[tag_index].MultiTexture[0]));
      desc += temp;
    }

    if(m_pTagLookup[tag_index].MultiTexture[1])
    {
      temp.Format("MultiTextureTag: %08X  %s\r\n", 
        m_pTagLookup[tag_index].MultiTexture[1], GetTagName(m_pTagLookup[tag_index].MultiTexture[1]));
      desc += temp;
    }

    if(m_pTagLookup[tag_index].MultiTexture[2])
    {
      temp.Format("MultiTextureTag: %08X  %s\r\n", 
        m_pTagLookup[tag_index].MultiTexture[2], GetTagName(m_pTagLookup[tag_index].MultiTexture[2]));
      desc += temp;
    }

    if(m_pTagLookup[tag_index].MultiTexture[3])
    {
      temp.Format("MultiTextureTag: %08X  %s\r\n", 
        m_pTagLookup[tag_index].MultiTexture[3], GetTagName(m_pTagLookup[tag_index].MultiTexture[3]));
      desc += temp;
    }

    if(m_pTagLookup[tag_index].MultiTexture[4])
    {
      temp.Format("MultiTextureTag: %08X  %s\r\n", 
        m_pTagLookup[tag_index].MultiTexture[4], GetTagName(m_pTagLookup[tag_index].MultiTexture[4]));
      desc += temp;
    }

    if(m_pTagLookup[tag_index].PhysicsTag)
    {
      temp.Format("PhysicsTag: %08X  %s\r\n", 
        m_pTagLookup[tag_index].PhysicsTag, GetTagName(m_pTagLookup[tag_index].PhysicsTag));
      desc += temp;
    }
    
    if(m_pTagLookup[tag_index].WeaponTag)
    {
      temp.Format("WeaponTag: %08X  %s\r\n", 
        m_pTagLookup[tag_index].WeaponTag, GetTagName(m_pTagLookup[tag_index].WeaponTag));
      desc += temp;
    }
    
    if(m_pTagLookup[tag_index].EquipmentTag)
    {
      temp.Format("EquipmentTag: %08X  %s\r\n", 
        m_pTagLookup[tag_index].EquipmentTag, GetTagName(m_pTagLookup[tag_index].EquipmentTag));
      desc += temp;
    }

    desc += model_subtag_info;
    desc += shader_subtag_info;

  }

  return(desc);
}

void CTagManager::ActivateLightmap(UINT lightmap_tag)
{
  int tag_index = CheckForTag(lightmap_tag);

  if(tag_index >= 0)
  {
    //define new defs for multi-bitmap resources
    m_pTagLookup[tag_index].BitmapStatus = LIGHTMAP;
    CString temp = m_TagNames[tag_index];
  }
}

UINT CTagManager::GetHighLevelTagCount(UINT tag_type)
{
  UINT count=0;
  int i;
  
  for(i=0; i<m_TagCount; i++)
  {
    if(m_pTagLookup[i].tagclass[0] == tag_type)
      count++;
  }

  return(count);
}

void CTagManager::GetHighLevelTagInfo(UINT tag_type, int index, UINT *pTagId, UINT *pRawNameOffset, CString *pStr)
{
  int count=0;

  for(int i=0; i<m_TagCount; i++)
  {
    if(m_pTagLookup[i].tagclass[0] == tag_type)
    {      
      if(count == index)
        break;

      count++;
    }
  }

  if(count == index)
  {
    *pTagId = m_pTagLookup[i].tag_id;
    *pStr =  m_pTagLookup[i].name;
    *pRawNameOffset = m_pTagLookup[i].RawNameOffset;
  }
}

void CTagManager::ExportTagInfo(CString filename)
{
  CStdioFile outfile;
  CString str;

  outfile.Open(filename, CFile::modeCreate|CFile::modeReadWrite);
  int tag;
  int count;

  str.Format("=====================================================================\n");
  outfile.WriteString(str);

  HaloMap.ReportMapInfo(&outfile);

  gBspManager.ReportBspInfo(&outfile);
  str.Format("=====================================================================\n");
  outfile.WriteString(str);

  outfile.WriteString("\n##################### TAG INDEX DUMP ################################\n\n");

  for(tag=0; tag<m_TagCount; tag++)
  {
    count = 0;

    if(m_pTagLookup[tag].ModelTag)count++;
    if(m_pTagLookup[tag].CollisionModelTag)count++;
    if(m_pTagLookup[tag].ShaderTag)count++;
    if(m_pTagLookup[tag].BaseTextureTag)count++;
    if(m_pTagLookup[tag].PhysicsTag)count++;
    if(m_pTagLookup[tag].WeaponTag)count++;
    if(m_pTagLookup[tag].EquipmentTag)count++;
    if(m_pTagLookup[tag].pPcModel)count++;    
    if(m_pTagLookup[tag].pXboxModel)count++;

    if(count == 1)
    {
      str.Format("     ----------------------------------------------------------------\n");
      outfile.WriteString(str);
    }
    else if(count > 1)
    {
      str.Format("=====================================================================\n");
      outfile.WriteString(str);
    }

    str.Format("%4X %c%c%c%c  %c%c%c%c  %c%c%c%c %08X  %s\n", tag, 
                                   ((char*)&m_pTagLookup[tag].tagclass[0])[0],
                                   ((char*)&m_pTagLookup[tag].tagclass[0])[1],
                                   ((char*)&m_pTagLookup[tag].tagclass[0])[2],
                                   ((char*)&m_pTagLookup[tag].tagclass[0])[3],
                                   ((char*)&m_pTagLookup[tag].tagclass[1])[0],
                                   ((char*)&m_pTagLookup[tag].tagclass[1])[1],
                                   ((char*)&m_pTagLookup[tag].tagclass[1])[2],
                                   ((char*)&m_pTagLookup[tag].tagclass[1])[3],
                                   ((char*)&m_pTagLookup[tag].tagclass[2])[0],
                                   ((char*)&m_pTagLookup[tag].tagclass[2])[1],
                                   ((char*)&m_pTagLookup[tag].tagclass[2])[2],
                                   ((char*)&m_pTagLookup[tag].tagclass[2])[3],
                                   m_pTagLookup[tag].tag_id,
                                   m_TagNames[tag]);
    outfile.WriteString(str);

    str.Format("     meta_offset:     %08X\n", m_pTagLookup[tag].meta_offset);
    outfile.WriteString(str);
    str.Format("     meta_size:       %08X\n", m_pTagLookup[tag].meta_size);
    outfile.WriteString(str);

    if(m_pTagLookup[tag].pPcModel)
    {
      str.Format("     IndicesStart:    %08X\n", m_pTagLookup[tag].pPcModel->m_Report.IndexOffsetStart);
      outfile.WriteString(str);
      str.Format("     IndicesEnd:      %08X\n", m_pTagLookup[tag].pPcModel->m_Report.IndexOffsetEnd);
      outfile.WriteString(str);
      str.Format("     VerticesStart:   %08X\n", m_pTagLookup[tag].pPcModel->m_Report.VertexOffsetStart);
      outfile.WriteString(str);
      str.Format("     VerticesEnd:     %08X\n", m_pTagLookup[tag].pPcModel->m_Report.VertexOffsetEnd);
      outfile.WriteString(str);
    }
    
    if(m_pTagLookup[tag].pXboxModel)
    {
      str.Format("     IndicesStart:    %08X\n", m_pTagLookup[tag].pXboxModel->m_Report.IndexOffsetStart);
      outfile.WriteString(str);
      str.Format("     IndicesEnd:      %08X\n", m_pTagLookup[tag].pXboxModel->m_Report.IndexOffsetEnd);
      outfile.WriteString(str);
      str.Format("     VerticesStart:   %08X\n", m_pTagLookup[tag].pXboxModel->m_Report.VertexOffsetStart);
      outfile.WriteString(str);
      str.Format("     VerticesEnd:     %08X\n", m_pTagLookup[tag].pXboxModel->m_Report.VertexOffsetEnd);
      outfile.WriteString(str);
    }
    
    if(m_pTagLookup[tag].ModelTag)
    {
      str.Format("     ModelTag:        %08X  %s\n", m_pTagLookup[tag].ModelTag, 
                                                 GetTagName(m_pTagLookup[tag].ModelTag));
      outfile.WriteString(str);
    }
    
    if(m_pTagLookup[tag].CollisionModelTag)
    {
      str.Format("     CollisionTag:    %08X  %s\n", m_pTagLookup[tag].CollisionModelTag,
                                                 GetTagName(m_pTagLookup[tag].CollisionModelTag));
      outfile.WriteString(str);
    }
    
    if(m_pTagLookup[tag].ShaderTag)
    {
      str.Format("     ShaderTag:       %08X  %s\n", m_pTagLookup[tag].ShaderTag,
                                                 GetTagName(m_pTagLookup[tag].ShaderTag));
      outfile.WriteString(str);
    }
    
    if(m_pTagLookup[tag].BaseTextureTag)
    {
      str.Format("     BaseTextureTag:  %08X  %s\n", m_pTagLookup[tag].BaseTextureTag, 
                                                 GetTagName(m_pTagLookup[tag].BaseTextureTag));
      outfile.WriteString(str);
    }
    
    if(m_pTagLookup[tag].PhysicsTag)
    {
      str.Format("     PhysicsTag:      %08X\n", m_pTagLookup[tag].PhysicsTag,
                                                 GetTagName(m_pTagLookup[tag].PhysicsTag));
      outfile.WriteString(str);
    }
    
    if(m_pTagLookup[tag].WeaponTag)
    {
      str.Format("     WeaponTag:       %08X\n", m_pTagLookup[tag].WeaponTag,
                                                 GetTagName(m_pTagLookup[tag].WeaponTag));
      outfile.WriteString(str);
    }
    
    if(m_pTagLookup[tag].EquipmentTag)
    {
      str.Format("     EquipmentTag:    %08X\n", m_pTagLookup[tag].EquipmentTag,
                                                 GetTagName(m_pTagLookup[tag].EquipmentTag));
      outfile.WriteString(str);
    }

    str.Format("\n");
    outfile.WriteString(str);
  }

  outfile.Close();
}
