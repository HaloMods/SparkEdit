// Analyzer.cpp: implementation of the CAnalyzer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SparkEdit.h"
#include "Analyzer.h"
#include "math.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAnalyzer::CAnalyzer()
{
  m_OffsetMin = 0;
  m_OffsetMax = 0;
  m_Magic = 0;
}

CAnalyzer::~CAnalyzer()
{

}

void CAnalyzer::SearchMetaForValidOffsets(CFile *pMapFile, INDEX_ITEM *pItem, int index, CString tag, CString Name)
{
  UINT *pHeader = NULL;
  INDEX_ITEM *pNext = NULL;
  int metasize = 0;
  int i = 0;
  int offset_guess = 0;
  int section_start, section_end;

  pNext = pItem + 1;
  metasize = (pNext->offset - pItem->offset)/4;
  
  if(metasize <= 0)
  {
    printf("'%s' ERROR:  metasize was negative\n", tag, pItem->offset, metasize*4);
  }
  else
  {
    pHeader = new UINT[metasize];
    
    pMapFile->Seek(pItem->offset, 0);
    pMapFile->Read(pHeader, metasize*4);
    
    section_start = 0x0000000;
    section_end =  0x20000000;
    printf("\n'%s' metastart = %08X (size = %08X) %s\n", tag, pItem->offset, metasize*4, Name);
    for(i=0; i<metasize; i++)
    {
      if((pHeader[i] >= m_OffsetMin)&&(pHeader[i] < m_OffsetMax))
      {
        if(tag == "antr")
        {
          //printf("Not Processing antr\n");
        }
        else if(tag == "sbsp")
        {
          //printf("Not Processing BSP\n");
        }
        else if(tag == "snd!")
        {
          //printf("Not Processing BSP\n");
        }
        else if((pHeader[i] != 0x80000000)&&(CheckForAscii(pHeader[i])==FALSE))
        {
          offset_guess = pHeader[i] - m_Magic;
          
          if((offset_guess > section_start)&&(offset_guess <= section_end))
          {         
            /* We found a likely file offset */ 
            printf("'%s' %08X (%08X)", tag, offset_guess, pHeader[i]);
            
            if((offset_guess > pItem->offset)&&(offset_guess < (pItem->offset + metasize*4)))
              printf(" metadata\n");
            else
              printf(" raw (%s)\n", GetRawSectionText(pMapFile, offset_guess, tag));
          }
        }
      }
    }

    delete [] pHeader;
  }
}

void CAnalyzer::SetOffsetMinMax(int min, int max, int magic)
{
  m_OffsetMin = min;
  m_OffsetMax = max;
  m_Magic = magic;
}

CString CAnalyzer::GetRawSectionText(CFile *pMapFile, int offset, CString tag)
{
  DWORD current_loc;
  char buffer[128];
  CString str;

  current_loc = pMapFile->GetPosition();
  pMapFile->Seek(offset, 0);
  pMapFile->Read(buffer, 128);

  if(tag == "wphi")
    TRACE("hmm.\n");

  if(CheckForAscii(*((int*)&buffer)) == TRUE)
  {
    buffer[127] = 0;
    str = buffer;
  }

  pMapFile->Seek(current_loc, 0);

  if((str == "")&&(tag == "wphi"))
    TRACE("No string found.\n");

  return(str);
}

void CAnalyzer::SearchForFloats(CFile *pMapFile)
{
  DWORD FileLen;
  BYTE *pBuf = NULL;
  DWORD LoopCount, i, buf_size;
  DWORD ofs0_count, ofs1_count, ofs2_count;

  buf_size = 0x1000;

  pBuf = new BYTE[buf_size];

  FileLen = pMapFile->GetLength();
  LoopCount = FileLen/buf_size;

  for(i=0; i<LoopCount; i++)
  {
    /* read in next block of data */ 
    pMapFile->Seek(i*buf_size, 0);
    pMapFile->Read(pBuf, buf_size);

    /* perform scan for floats between 0.001 and 10000 */ 
    ofs0_count = SearchBufferForTextureCoords(pBuf, buf_size, 0);
    ofs1_count = SearchBufferForTextureCoords(pBuf, buf_size, 1);
    ofs2_count = SearchBufferForTextureCoords(pBuf, buf_size, 2);

    //ofs0_count = SearchBufferForFloatVectors(pBuf, buf_size, 0);
    //ofs1_count = SearchBufferForFloatVectors(pBuf, buf_size, 1);
    //ofs2_count = SearchBufferForFloatVectors(pBuf, buf_size, 2);

    /* report the results for each 'byte offset' */ 
    printf("Offset: %08X   %8d  %8d  %8d\n", i*buf_size, 
           ofs0_count, ofs1_count, ofs2_count);
  }

  delete pBuf;
}

int CAnalyzer::SearchBufferForFloats(BYTE *pBuf, DWORD buf_size, DWORD byte_align)
{
  int found_count = 0;
  float *pfloat = NULL;
  int i;

  pfloat = (float*)(&pBuf[byte_align]);

  for(i=0; i<((buf_size/4)-1); i++)
  {
    if((pfloat[i] > 0.001)&&(pfloat[i] < 10000))
      found_count++;
    else if((pfloat[i] < -0.001)&&(pfloat[i] > -10000))
      found_count++;
  }

  return(found_count);
}

int CAnalyzer::SearchBufferForFloatVectors(BYTE *pBuf, DWORD buf_size, DWORD byte_align)
{
  int found_count = 0;
  float *pfloat = NULL;
  int i;
  int vector_count = 0;

  pfloat = (float*)(&pBuf[byte_align]);

  for(i=0; i<((buf_size/4)-1); i++)
  {
    if((pfloat[i] > 0.001)&&(pfloat[i] < 10000))
    {
      vector_count++;
    }
    else if((pfloat[i] < -0.001)&&(pfloat[i] > -10000))
    {
      vector_count++;
    }
    else
    {
      vector_count = 0;
    }

    if(vector_count == 3)
    {
      found_count++;
      vector_count = 0;
    }
  }

  return(found_count);
}

int CAnalyzer::SearchBufferForTextureCoords(BYTE *pBuf, DWORD buf_size, DWORD byte_align)
{
  int found_count = 0;
  float *pfloat = NULL;
  int i;
  int vector_count = 0;
  float vector[3];
  float mag = 0;

  pfloat = (float*)(&pBuf[byte_align]);

  for(i=0; i<((buf_size/4)-1); i++)
  {
    if((pfloat[i] > 0.0001)&&(pfloat[i] <= 1.00))
    {
      vector[vector_count] = pfloat[i];
      vector_count++;
    }
    else
    {
      vector[0]=0;
      vector[1]=0;
      vector[2]=0;
      vector_count = 0;
    }

    if(vector_count == 3)
    {
      mag = sqrt((vector[0]*vector[0]) + (vector[0]*vector[0]) + (vector[0]*vector[0])); 
      
      if((mag > 0.9)&&(mag < 1.1))
        found_count++;

      vector[0]=0;
      vector[1]=0;
      vector[2]=0;
      vector_count = 0;
    }
  }

  return(found_count);
}

void CAnalyzer::SearchForBspHeader(CFile *pMapFile)
{
  DWORD SearchLen;
  int vert_block_start = 0x003c0408;
  int header_field_location;
  int header_to_vert_block;
  int result;
  UINT *pBuf = NULL;
  DWORD LoopCount, i, j, buf_size, file_offset;

  buf_size = 0x10000;

  pBuf = new UINT[buf_size/4];

  SearchLen = 0x004000000;//pMapFile->GetLength();
  LoopCount = SearchLen/buf_size;

  for(i=0; i<LoopCount; i++)
  {
    /* read in next block of data */ 
    file_offset = i*buf_size;
    pMapFile->Seek(file_offset, 0);
    pMapFile->Read(pBuf, buf_size);

    for(j=0; j<(buf_size/4); j++)
    {
      header_field_location = file_offset + j*4;

      header_to_vert_block = vert_block_start - header_field_location;

      result = header_to_vert_block - pBuf[j];

      if((result > -2000)&&(result < 2000))
      {
        printf("Offset: %08X   result = %d\n", file_offset, result);
      }
    }
  }

  delete pBuf;

}

void CAnalyzer::SearchForPattern(CFile *pMapFile)
{
  DWORD FileLen;
  BYTE *pBuf = NULL;
  DWORD LoopCount, i, j, w, buf_size;
  UINT pat1, pat2, pat3;
  UINT in1, in2, in3;
  UINT wpat1, wpat2, wpat3;
  int d1, d2, d3;
  int wd1, wd2, wd3;

  buf_size = 0x10000;

  pBuf = new BYTE[buf_size];

  FileLen = pMapFile->GetLength();
  LoopCount = FileLen/buf_size;

  pat1 = 41;
  pat2 = 33;
  pat3 = 542;

  for(i=0; i<LoopCount - 2; i++)
  {
    /* read in next block of data */ 
    pMapFile->Seek(i*buf_size, 0);
    pMapFile->Read(pBuf, buf_size);

    for(j=0; j<(buf_size-3*sizeof(UINT)); j++)
    {
      for(w=0; w<4; w++)
      {
        if(w==0)
        {
          wpat1 = pat1;
          wpat2 = pat2;
          wpat3 = pat3;
        }
        else if(w==1)
        {
          wpat1 = pat1;
          wpat2 = pat3;
          wpat3 = pat2;
        }
        else if(w==2)
        {
          wpat1 = pat2;
          wpat2 = pat1;
          wpat3 = pat3;
        }
        else if(w==3)
        {
          wpat1 = pat2;
          wpat2 = pat3;
          wpat3 = pat1;
        }
        else if(w==4)
        {
          wpat1 = pat3;
          wpat2 = pat1;
          wpat3 = pat2;
        }
        else
        {
          wpat1 = pat3;
          wpat2 = pat2;
          wpat3 = pat1;
        }

        in1 = ((UINT*)&pBuf[j])[0];
        in2 = ((UINT*)&pBuf[j])[1];
        in3 = ((UINT*)&pBuf[j])[2];

        d1 = in2 - in1;
        d2 = in3 - in2;
        d3 = in1 - in3;

        wd1 = wpat2 - wpat1;
        wd2 = wpat3 - wpat2;
        wd3 = wpat1 - wpat3;

        if((d1 == wd1)&&(d2 == wd2)&&(d3 == wd3))
          printf("match found @ %08X\n", buf_size*i + j);

        //if((in1 == wpat1)&&(in2 == wpat2)&&(in3 == wpat3))
        //  printf("match found @ %08X\n", buf_size*i + j);
      }
    }
  }

  delete pBuf;
}

void CAnalyzer::SearchFileForNumber(CFile *pMapFile, UINT num)
{
  DWORD FileLen;
  BYTE *pBuf = NULL;
  DWORD LoopCount, i, j, buf_size;
  UINT *pUint = NULL;
  UINT found_count = 0;

  buf_size = 0x100;

  pUint = new UINT[buf_size];

  FileLen = pMapFile->GetLength();
  LoopCount = FileLen/(buf_size*4);

  printf("Searching...\n");
  for(i=0; i<LoopCount; i++)
  {
    /* read in next block of data */ 
    pMapFile->Seek(i*(buf_size*4), 0);
    pMapFile->Read(pUint, buf_size*4);

    found_count = 0;
    for(j=0; j<buf_size; j++)
    {
      if((pUint[j] == 0x564))//> 0x24ef000)&&(pUint[j] < 0x24effff))
      {
        found_count++;
        printf("Found '%08X' at %08X\n", pUint[j], (i*buf_size*4 + j*4));
      }
    }
    if(found_count != 0)
      printf("%08X  %4lX\n", i*buf_size*4, found_count);
  }
  printf("End Search\n");

  delete pUint;
}

void CAnalyzer::ConvertToFloat(CFile *pMapFile, UINT start_addr, UINT stop_addr)
{
  DWORD FileLen;
  DWORD LoopCount, i, buf_size;
  float *pFloat = NULL;
  UINT start;

  buf_size = 0x10000;

  pFloat = new float[buf_size];

  FileLen = pMapFile->GetLength();

  if(stop_addr < FileLen)
    LoopCount = (stop_addr - start_addr)/(buf_size*4)+1;
  else
    LoopCount = (FileLen - start_addr)/(buf_size*4);

  LoopCount = FileLen/(buf_size*4);
  start = start_addr/(buf_size*4);

  for(i=start; i<LoopCount; i++)
  {
    /* read in next block of data */ 
    pMapFile->Seek(i*(buf_size*4), 0);
    pMapFile->Read(pFloat, buf_size*4);

    DumpBufferContents(pFloat, i*(buf_size*4), buf_size);
  }

  delete pFloat;
}


void CAnalyzer::SearchForIndexDistribution(CFile *pMapFile)
{
  DWORD FileLen;
  BYTE *pBuf = NULL;
  DWORD LoopCount, i, j, buf_size;
  USHORT *pUShort = NULL;
  UINT found_count = 0;
  USHORT min, max;
  double mean;
  UINT zero_count;

  buf_size = 0x100;

  pUShort = new USHORT[buf_size];

  FileLen = pMapFile->GetLength();
  LoopCount = FileLen/(buf_size*2);

  printf("Begin Index Distribution Analysis...\n");
  for(i=0; i<LoopCount; i++)
  {
    /* read in next block of data */ 
    pMapFile->Seek(i*(buf_size*2), 0);
    pMapFile->Read(pUShort, buf_size*2);

    min = 0xFFFF;
    max = 0;
    mean = 0.0;
    zero_count = 0;
    for(j=0; j<buf_size; j++)
    {
      if(pUShort[j] < min)
        min = pUShort[j];

      if(pUShort[j] > max)
        max = pUShort[j];

      mean += pUShort[j];

      if(pUShort[j] == 0)
        zero_count++;
    }
    mean /= buf_size;

    if(max < 10000)
      printf("%08X  %8d %8d %8.2f  (%8d)\n", i*buf_size*2, min, max, mean, zero_count);
   // else
   //   printf("%08X  --------------------------------\n", i*buf_size*2);
  }
  printf("End Index Distribution Analysis\n");

  delete pUShort;
}

void CAnalyzer::SearchForVector(float *v3, float *pBuf, int size, UINT base_addr)
{
  float compare[3];

  for(int i=0; i<(size/4-2); i++)
  {
    compare[0] = pBuf[i];
    compare[1] = pBuf[i+1];
    compare[2] = pBuf[i+2];

    if((*((UINT*)compare) & 0x7F800000)!=0x7F800000)
    {
      if((compare[0] == v3[0])&&(compare[1] == v3[1])&&(compare[2] == v3[2]))
      {
        TRACE("Found Match at 0x%X\n", base_addr+i*4);
      }
    }
  }
}

void CAnalyzer::SetIndexInfo(UINT tag_count, UINT base_tag)
{
  m_TagCount = tag_count;
  m_BaseTag = base_tag;
}

void CAnalyzer::Cleanup()
{
  m_TagList.RemoveAll();
}

CString CAnalyzer::CheckForTag(UINT val)
{
  CString str;
  USHORT val_l, val_h;

  val -= m_BaseTag;

  val_l = val & 0xFFFF;
  val_h = (val & 0xFFFF0000) >> 16;

  if((val_l == val_h)&&(val_l < m_TagCount))
  {
    str.Format("  Tag(%d)=%s", val_l, m_TagList[val_l]);
  }

  return(str);
}

void CAnalyzer::SuperAnalyzeFileSection(UINT offset, UINT len)
{
  // break up large buffers into sections
  DWORD i, j, buf_size, read_count;
  float *pFloat = NULL;
  CString tmp, out, type, reference, text;
  UINT recast;
  int loop_size;

  buf_size = 0x10000;

  pFloat = new float[buf_size];

  read_count = 0;
  out.Format("%08X: ", offset);
  for(i=0; i<len; i++)
  {
    m_pMapFile->Seek(offset+read_count*buf_size*4, 0);
    m_pMapFile->Read(pFloat, buf_size*4); // worry about special case when past EOF?
    read_count++;

    if(read_count*buf_size < len)
      loop_size = buf_size;
    else
      loop_size = (len%buf_size);

    /* loop through the data and reformat */ 
    for(j=0; j<loop_size; j++)
    {
      if((pFloat[j] > 0.0001f)&&(pFloat[j]<10000.0f))
        tmp.Format("%10.3f ", pFloat[j]);
      else if((pFloat[j] < -0.0001f)&&(pFloat[j] > -10000.0f))
        tmp.Format("%10.3f ", pFloat[j]);
      else
        tmp.Format("  %08X ", *((UINT*)&pFloat[j]));

      out += tmp;

      recast = *((UINT*)&pFloat[j]);
      reference += CheckForReference(m_pMapFile, recast, m_Magic);
      reference += CheckForTag(recast);

      text += ConvertToText(recast);

      if((j%4)==3)
      {
        out += text;
        out += "  ";
        out += reference;
        out += "\n";
        printf(out);
        reference = "";
        text = "";
        out.Format("%08X: ", offset+((read_count-1)*buf_size + j+1)*4);
      }

      tmp = "";
    }

    i += (loop_size-1);
  }

  delete pFloat;
}

void CAnalyzer::AddTagString(CString tag_str)
{
  m_TagList.Add(tag_str);
}

void CAnalyzer::Initialize(CFile *pMapFile, UINT magic)
{
  m_pMapFile = pMapFile;
  m_Magic = magic;
}
