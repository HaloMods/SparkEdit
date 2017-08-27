/*-------------------------------------------------------------------
 *  Original Author: Grenadiac
 *
 *  Contributing Authors:
 *
 *  File Description: 
 *    Utility class written to avoid rewriting analysis code over
 *    and over.  Meant to be parent class of Bitmaps, sounds, models,
 *    etc.
 *
 *-----------------------------------------------------------------*/
#include "stdafx.h"
#include "SparkEdit.h"
#include "Util.h"
#include "direct.h"
#include "Perspective.h"
#include "TagManager.h"
#include "Bspmanager.h"

extern CTagManager gTagManager;
extern CBspManager gBspManager;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CPerspective *g_pRenderWnd;

/*-------------------------------------------------------------------
 * Name: CUtil()
 * Description:
 *   Init util vars here.
 *-----------------------------------------------------------------*/
CUtil::CUtil()
{
  char tmp[257];

  m_ObjCount = 0;
  m_ObjStart = 0xFFFFFFFF;
  m_ObjEnd = 0;
  
  /* set working directory as output path for now */ 
  memset(tmp, 0, 257);
  _getcwd(tmp, 256);  
  m_WorkingDir = tmp;

  memset(tmp, 0, 257);
  _chdir("output");
  _getcwd(tmp, 256);
  m_OutputPathRoot = tmp;

  float test = 1.0;
  SHORT c_test;

  CompressFloatToShort(test, c_test);
  test = DecompressShortToFloat(c_test);
}

/*-------------------------------------------------------------------
 * Name: ~CUtil()
 * Description:
 *   
 *-----------------------------------------------------------------*/
CUtil::~CUtil()
{
}

/*-------------------------------------------------------------------
 * Name: DumpBlockInfo()
 * Description:
 *   Outputs object(s) overall start and stop positions in the file
 *-----------------------------------------------------------------*/
void CUtil::DumpBlockInfo(void)
{
  printf("'%s' (%04d)  %08X %08X\n", m_ObjTag, m_ObjCount, m_ObjStart, m_ObjEnd);
}

/*-------------------------------------------------------------------
 * Name: UpdateBlockInfo()
 * Description:
 *   Determines the first and last address where objects of this
 *   type occupy the original map file (to figure out map format).
 *-----------------------------------------------------------------*/
void CUtil::UpdateBlockInfo(UINT offset, UINT size)
{
  if(offset < m_ObjStart)
    m_ObjStart = offset;

  if((offset+size) > m_ObjEnd)
    m_ObjEnd = offset + size;
}

/*-------------------------------------------------------------------
 * Name: DumpInfo()
 * Description:
 *   Utility function used to output object size and location info.
 *-----------------------------------------------------------------*/
void CUtil::DumpInfo(UINT offset, UINT size, CString Name)
{
  printf("'%s' %04d %08X  %08X   %s\n", m_ObjTag, m_ObjCount++, offset, size, Name);

  UpdateBlockInfo(offset, size);
}

/*-------------------------------------------------------------------
 * Name: ReadHeader()
 * Description:
 *   A prototype that should be overridden in the child class.
 *   Determines object size and location in the raw data section.
 *-----------------------------------------------------------------*/
void CUtil::ReadHeader(CFile *pMapFile, INDEX_ITEM *item, int magic, CString Name)
{
}

CString CUtil::CreateItemDirectory(CString Name)
{
  CString path, temp;
  int i = 0;
  int j = 0;

  path = m_OutputPathRoot;

  while(i >= 0)
  {
    i = Name.Find("\\", j);
    
    if(i < 0)
      temp = Name.Right(Name.GetLength() - j);
    else
      temp = Name.Mid(j, (i-j));

    path += ("\\" + temp);
    _mkdir(path);
    
    j = i + 1;
  }

  return(path);
}

void CUtil::ExtractFilename(CString Input, CString *pPath, CString *pFilename)
{
  int i;

  i = Input.ReverseFind('\\');
  *pFilename = Input.Right(Input.GetLength() - i - 1);
  *pPath = Input.Left(i);
}

void CUtil::DumpBufferContents(float *pFloat, UINT buffer_base, UINT float_count)
{
  DWORD j;
  UINT found_count = 0;
  CString tmp, out, type;
  char text[256];
  BOOL bFirstPass = TRUE;

  out.Format("%08X: ", buffer_base);
    for(j=0; j<float_count; j++)
    {
      if((pFloat[j] > 0.0001f)&&(pFloat[j]<10000.0f))
        tmp.Format("%10.3f ", pFloat[j]);
      else if((pFloat[j] < -0.0001f)&&(pFloat[j] > -10000.0f))
        tmp.Format("%10.3f ", pFloat[j]);
      else
        tmp.Format("  %08X ", *((UINT*)&pFloat[j]));

      memcpy(&text[(j%4)*4], &pFloat[j], 4);

      out += tmp;

      if((j%4)==3)
      {
        type = GetAddrRange(buffer_base + j*4);
        tmp.Format("  %s\n", type);
        out += tmp;
        printf(out);
        out.Format("%08X: ", (buffer_base +(j+1)*4));
        ZeroMemory(text, 256);
      }

      tmp = "";
    }

    //
    
    //
}

CString CUtil::GetAddrRange(UINT addr)
{
  CString type;

  /* put in data checks */ 
  if((addr >= 0x0)&&(addr < 0x800))
    type = "header";

  else if(addr < 0x37840)
    type = "unclassified";

  else if(addr < 0x390bc)
    type = "struct1: f[3]u[3]f[3]";

  else if(addr < 0x396e0)
    type = "binary (lightmap?)";

  else if(addr < 0x3f000)
    type = "struct1: f[3]u[3]f[3]";

  else if(addr < 0x40660)
    type = "binary (lightmap?)";

  else if(addr < 0x472a0)
    type = "struct1: f[3]u[3]f[3]";

  else if(addr < 0x48dc0)
    type = "binary (lightmap?)";
  
  else if(addr < 0x4c100)
    type = "struct1: f[3]u[3]f[3]";
  
  else if(addr < 0x4cde0)
    type = "binary (lightmap?)";
  
  else if(addr < 0x52b20)
    type = "struct1: f[3]u[3]f[3]";
  
  else if(addr < 0x54280)
    type = "binary (lightmap?)";
  
  else if(addr < 0x57000)
    type = "struct1: f[3]u[3]f[3]";
  
  else if(addr < 0x57b60)
    type = "binary (lightmap?)";
  
  else if(addr < 0x57fe0)
    type = "struct1: f[3]u[3]f[3]";
  
  else if(addr < 0x5ae80)
    type = "more of the same";

  else if(addr < 0x5b2c0)
    type = "bsp header?";

  else if(addr < 0x63cf0)
    type = "flag crap";

  else if(addr < 0x68030)
    type = "normals?";

  else if(addr < 0x70ffc)
    type = "indexing1";

  else if(addr < 0x79310)
    type = "struct2: f[3]us[4]";

  else if(addr < 0xa11e0)
    type = "indexing2 with flags";

  else if(addr < 0xaade0)
    type = "vertex data";

  else if(addr < 0xb3420)
    type = "FF00 stuff";

  else if(addr < 0xc4860)
    type = "indexing3";

  else if(addr < 0xcbe74)
    type = "indexing4 (16-bit)";

  else if(addr < 0xcc324)
    type = "(1,2,3) 16-bit";

  else if(addr < 0xd12d0)
    type = "large struct";

  else if(addr < 0xd20e0)
    type = "struct4 (f[3]f[3]i,addr,i)";

  else if(addr < 0xd72b0)
    type = "INTERESTING INDEX";

  else if(addr < 0xd8018)
    type = "zeros, floats, flags";


  //else if((addr >= 0x0)&&(addr < 0x800))
  //  type = "vertex";

  else if((addr >= 0xd8800)&&(addr < 0x1652100))
    type = "bitmaps";

  else if((addr >= 0x273bd9c)&&(addr < 0x29eff84))
    type = "sound";

  else if((addr >= 0x026025A4)&&(addr < 0x266432C))
    type = "scenario";

  
  return(type);
}

BOOL CUtil::CheckForAscii(int val)
{
  BYTE tmp;
  BOOL bIsText = TRUE;

  for(int i=0; i<4; i++)
  {
    tmp = ((BYTE*)&val)[i];

    if((tmp < 'a')||(tmp > 'z'))
    {
      if(tmp != '\\')
        bIsText = FALSE;
    }
  }

  return(bIsText);
}

void CUtil::AnalyzeFileSection(CFile *pMapFile, UINT offset, UINT len, UINT magic, UINT xref_magic)
{
  // break up large buffers into sections
  DWORD i, j, buf_size, read_count, buffer_base;
  float *pFloat = NULL;
  CString tmp, out, type, reference, text;
  UINT recast;
  int loop_size;

  OFFSET_XREF *pXrefBuffer = 0;
  UINT XrefCount;

  buf_size = 0x10000;

  pFloat = new float[buf_size];

  read_count = 0;
  out.Format("%08X: ", offset);
  i=0;
  while(i<len)
  {
    pMapFile->Seek(offset+read_count*buf_size, 0);
    pMapFile->Read(pFloat, buf_size*4); // worry about special case when past EOF?
    read_count++;

    if(read_count*buf_size < len)
      loop_size = buf_size/4;
    else
      loop_size = (len%buf_size)/4 + 4;

    buffer_base = offset+(read_count-1)*buf_size*4;

    TRACE("%08X / %08X (%08X) (%08X)\n", i, len, loop_size, buffer_base);


    AppendXrefs((UINT*)pFloat,
                   loop_size,
                   buffer_base,
                   offset,
                   offset+len,
                   &pXrefBuffer,
                   &XrefCount,
                   xref_magic);
                   
    /* loop through the data and reformat */ 
    for(j=0; j<loop_size; j++)
    {
      BYTE *pTemp = (BYTE*)&pFloat[j];
      BYTE test[4];
      float LE;

      test[0] = pTemp[3];
      test[1] = pTemp[2];
      test[2] = pTemp[1];
      test[3] = pTemp[0];

      LE = *((float*)test);

      if((pFloat[j] > 0.00001f)&&(pFloat[j]<100000.0f))
        tmp.Format("%10.3f ", pFloat[j]);
      else if((pFloat[j] < -0.00001f)&&(pFloat[j] > -100000.0f))
        tmp.Format("%10.3f ", pFloat[j]);
      else if((LE < -0.00001f)&&(LE > -100000.0f))
        tmp.Format("~%9.3f ", LE);
      else if((LE < -0.00001f)&&(LE > -100000.0f))
        tmp.Format("~%9.3f ", LE);
      else
        tmp.Format("  %08X ", *((UINT*)&pFloat[j]));

      out += tmp;

      recast = *((UINT*)&pFloat[j]);
      reference += CheckForReference(pMapFile, recast, magic);

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
        out.Format("%08X: ", offset+i+4);//offset+((read_count-1)*buf_size + j+1)*4);
      }

      tmp = "";
      i+=4;
    }

    //i += (loop_size-1);
  }

  delete pFloat;
}

void CUtil::AnalyzeFileSection3(CFile *pMapFile, UINT offset, UINT len, UINT magic, UINT xref_magic)
{
  // break up large buffers into sections
  DWORD j, k, buf_size;
  float *pFloat = NULL;
  CString tmp, out, type, reference, text, xref_string, bsp_string;
  UINT recast;

  OFFSET_XREF *pXrefBuffer = 0;
  UINT XrefCount = 0;

  buf_size = len/4 + 1;

  pFloat = new float[buf_size];

  out.Format("%08X: ", offset);

  pMapFile->Seek(offset, 0);
  pMapFile->Read(pFloat, len);

  AppendXrefs((UINT*)pFloat,
              buf_size,
              offset,
              0,
              pMapFile->GetLength(),
              &pXrefBuffer,
              &XrefCount,
              xref_magic);

  //DUMP BSP INFO

  for(k=0; k<BSP_XREF_COUNT; k++)
  {
    printf("[BSP: %08X | size= %8X | count =%8d | %s]\n",
      gBspManager.m_pBsp[0].m_BspXrefs[k].offset,
      gBspManager.m_pBsp[0].m_BspXrefs[k+1].offset - gBspManager.m_pBsp[0].m_BspXrefs[k].offset,
      gBspManager.m_pBsp[0].m_BspXrefs[k].count,
      gBspManager.m_pBsp[0].m_BspXrefs[k].name);
  }


  /* loop through the data and reformat */ 
  for(j=0; j<buf_size; j++)
  {
    BYTE *pTemp = (BYTE*)&pFloat[j];
    BYTE test[4];
    float LE;
    
    test[0] = pTemp[3];
    test[1] = pTemp[2];
    test[2] = pTemp[1];
    test[3] = pTemp[0];
    
    LE = *((float*)test);

    for(k=0; k<XrefCount; k++)
    {
      if((offset+j*4) == pXrefBuffer[k].dest)
      {
        tmp.Format("XREF: (%X->%X) ", pXrefBuffer[k].source, pXrefBuffer[k].dest);
        xref_string += tmp;
      }
      else if((*((UINT*)&pFloat[j])-xref_magic) == pXrefBuffer[k].source)
      {
        tmp.Format("XREF to: %X ", pXrefBuffer[k].dest);
        xref_string += tmp;
      }
    }

    for(k=0; k<BSP_XREF_COUNT; k++)
    {
      if(gBspManager.m_pBsp[0].m_BspXrefs[k].offset == (offset+j*4))
        bsp_string.Format("[BSP: %X (%d) %s] ", 
                   (offset+j*4),
                   gBspManager.m_pBsp[0].m_BspXrefs[k].count,
                   gBspManager.m_pBsp[0].m_BspXrefs[k].name);
    }
    

    if((pFloat[j] > 0.00001f)&&(pFloat[j]<100000.0f))
      tmp.Format("%10.3f ", pFloat[j]);
    else if((pFloat[j] < -0.00001f)&&(pFloat[j] > -100000.0f))
      tmp.Format("%10.3f ", pFloat[j]);
    else if((LE < -0.00001f)&&(LE > -100000.0f))
      tmp.Format("~%9.3f ", LE);
    else if((LE < -0.00001f)&&(LE > -100000.0f))
      tmp.Format("~%9.3f ", LE);
    else
      tmp.Format("  %08X ", *((UINT*)&pFloat[j]));
    
    out += tmp;
    
    recast = *((UINT*)&pFloat[j]);
    reference += CheckForReference(pMapFile, recast, magic);
    
    text += ConvertToText(recast);
    
    if((j%4)==3)
    {
      out += text;
      out += "  ";
      out += xref_string;
      out += bsp_string;
      out += reference;
      out += "\n";
      printf(out);
      reference = "";
      xref_string = "";
      bsp_string = "";
      text = "";
      out.Format("%08X: ", offset+j*4+4);
    }
    
    tmp = "";
  }
  
  if(pFloat)
    delete pFloat;

  if(pXrefBuffer)
    delete pXrefBuffer;
}

CString CUtil::CheckForReference(CFile *pMapFile, UINT val, UINT magic)
{
  UINT offset_guess;
  UINT section_start = 0;
  UINT section_end = pMapFile->GetLength();
  CString str;
  char buffer[128];

  if((val != 0x80000000)&&(CheckForAscii(val)==FALSE))
  {
    offset_guess = val - magic;
    
    if((offset_guess > section_start)&&(offset_guess <= section_end))
    {         
      /* We found a likely file offset */ 
      pMapFile->Seek(offset_guess, 0);
      pMapFile->Read(buffer, 128);

      if(CheckForAscii(*((int*)&buffer)) == TRUE)
      {
        buffer[127] = 0;
        str.Format("%s", buffer);
      }
    }
  }

  return(str);
}

CString CUtil::ConvertToText(UINT val)
{
  char buf[5];
  ZeroMemory(buf,5);
  CString str;
  char tmp;

  for(int i=0; i<4; i++)
  {
    tmp = ((BYTE*)&val)[i];

    if((tmp >= 'a')&&(tmp <= 'z'))//||(tmp != '\\'))
      buf[i] = tmp;
    else if((tmp >= 'A')&&(tmp <= 'Z'))
      buf[i] = tmp;
    else if(tmp == '\\')
      buf[i] = tmp;
    else
      buf[i] = '.';
  }

  str = buf;

  return(str);
}

//converts 16 bit int into a float
float CUtil::DecompressShortToFloat(SHORT comp_vector)
{
  int temp;

  temp = comp_vector + comp_vector;

  float decomp_val;

  decomp_val = (((float)temp) + 1)/65535.0;

  return(decomp_val);
}

//converts 32 bit int into 3 float vector (-1.0 to 1.0)
void CUtil::DecompressIntToVector(UINT comp_vector, float *pVector)
{
  int cx, cy, cz;

  cx = comp_vector << 21;
  cy = (comp_vector >> 11) << 21;
  cz = (comp_vector >> 22) << 22;
 
  pVector[0] = (((float)cx/1048576.0) + 1.0)/2047.0;
  pVector[1] = (((float)cy/1048576.0) + 1.0)/2047.0;
  pVector[2] = (((float)cz/1048576.0) + 1.0)/2047.0;

  float mag = sqrt(pVector[0]*pVector[0] + pVector[1]*pVector[1] + pVector[2]*pVector[2]);
  //TRACE("%08X  %f %f %f (%f)\n", comp_vector, pVector[0], pVector[1], pVector[2], mag);
}

//converts 3 float vector into one 32 bit int (input data must be -1.0 to 1.0)
BOOL CUtil::CompressVector(UINT& comp_vector, float *pVector)
{
  BOOL bStatus = TRUE;
  float cx, cy, cz;
  int vector_x_int, vector_y_int, vector_z_int;

  cx = pVector[0];
  cy = pVector[1];
  cz = pVector[2];

  if(cx > 1.0)cx = 1.0;
  if(cx < -1.0)cx = -1.0;

  if(cy > 1.0)cy = 1.0;
  if(cy < -1.0)cy = -1.0;

  if(cz > 1.0)cz = 1.0;
  if(cz < -1.0)cz = -1.0;

  vector_x_int = ((int)floor(cx*1023.5)) & 0x7FF;
  vector_y_int = ((int)floor(cy*1023.5)) & 0x7FF;
  vector_z_int = ((int)floor(cz*511.5)) & 0x3FF;

  comp_vector = ((vector_y_int | (vector_z_int << 11)) << 11) | vector_x_int;

  //TRACE("%08X\n", comp_vector);
  return(FALSE);
}

void CUtil::CalculateCrossProduct(float *pU, float *pV, float *pUVcross)
{
  //u x v = (u2*v3 - u3*v2), -(u1*v3 - u3*v1), (u1*v2 - u2*v1)
  pUVcross[0] = pU[1]*pV[2] - pU[2]*pV[1];
  pUVcross[1] = pU[2]*pV[0] - pU[0]*pV[2];
  pUVcross[2] = pU[0]*pV[1] - pU[1]*pV[0];
}

float CUtil::CalculateCameraDist(float *pCoord)
{
  float cam[3];
  float dx, dy, dz, dist;

  g_pRenderWnd->GetCameraPosition(cam);

  dx = cam[0] - pCoord[0];
  dy = cam[1] - pCoord[1];
  dz = cam[2] - pCoord[2];

  dist = sqrt(dx*dx + dy*dy + dz*dz);

  return(dist);
}

float CUtil::DotProduct(float *pV1, float *pV2)
{
  float scalar;

  scalar = pV1[0]*pV2[0] + pV1[1]*pV2[1] + pV1[2]*pV2[2];

  return(scalar);
}

void CUtil::AnalyzeFileSection2(CFile *pMapFile, UINT offset, UINT len, UINT magic)
{
  // break up large buffers into sections
  DWORD i, j, buf_size, read_count;
  SHORT uw, lw;
  float *pFloat = NULL;
  CString tmp(""), out(""), type, reference, text, tag;
  CString ascii;
  UINT recast;
  int loop_size;
  char convert_buf[128];

  buf_size = 0x10000;

  pFloat = new float[buf_size];

  read_count = 0;
  out.Format("%08X: ", offset);
  for(i=0; i<len; i++)  //not sure why we divide by 4 yet
  {
    pMapFile->Seek(offset+read_count*buf_size*4, 0);
    pMapFile->Read(pFloat, buf_size*4); // worry about special case when past EOF?
    read_count++;

    if(read_count*buf_size <= len)
      loop_size = buf_size;
    else
      loop_size = (len%buf_size);

    /* loop through the data and reformat */ 
    for(j=0; j<loop_size; j++)
    {
      // raw data field
      tmp.Format(" %08X  ", *((UINT*)&pFloat[j]));
      out += tmp;

      // data type interpreter
      recast = *((UINT*)&pFloat[j]);
      reference = CheckForReference(pMapFile, recast, magic);
      
      if((recast & 0xF0000000) == 0xE0000000)
        int test = 0;

      tag = gTagManager.GetTagName(recast);

      if(reference != "")
        tmp.Format("NamePtr=\"%s\"", reference);
      else if(tag != "")
        tmp.Format("Tag_Id=\"%s\"", tag);
      else if((pFloat[j] > 0.0001f)&&(pFloat[j]<10000.0f))
        tmp.Format("%-10.3f ", pFloat[j]);
      else if((pFloat[j] < -0.0001f)&&(pFloat[j] > -10000.0f))
        tmp.Format("%-10.3f ", pFloat[j]);
      else if(IsAscii(pFloat[j], &ascii))
      {
        memcpy(convert_buf, &pFloat[j], 128);
        TruncateAsciiGarbage(convert_buf, 128);
        tmp.Format("%s  \"%s\"", ascii, convert_buf);
      }
      else
      {
        lw = recast & 0xFFFF;
        uw = (recast & 0xFFFF0000) >> 16;
        tmp.Format("(%5d  %5d) %12d ", uw, lw, recast);
      }


      out += tmp;


      //text += ConvertToText(recast);

      out += text;
      out += "  ";
      //out += reference;
      out += "\n";
      printf(out);
      TRACE(out);
      reference = "";
      text = "";
      out.Format("%08X: ", offset+((read_count-1)*buf_size + j+1)*4);

      tmp = "";
    }

    i += (loop_size-1);
  }

  delete pFloat;
}

BOOL CUtil::IsAscii(float val, CString * pStr)
{
  char buf[5];
  ZeroMemory(buf,5);
  CString str;
  char tmp;
  int ascii_count = 0;
  BOOL bIsAscii = FALSE;

  for(int i=0; i<4; i++)
  {
    tmp = ((BYTE*)&val)[i];

    if((tmp >= 0x20)&&(tmp <= 0x7E))
    {
      buf[i] = tmp;
      ascii_count++;
    }
    else
      buf[i] = '.';
  }

  *pStr = buf;
  if(ascii_count > 2)
    bIsAscii = TRUE;

  return bIsAscii;
}

void CUtil::TruncateAsciiGarbage(char* pBuf, int buf_size)
{
  for(int i=0; i<buf_size; i++)
  {
    if((pBuf[i] < 0x20)||(pBuf[i] > 0x7E))
    {
      pBuf[i] = NULL;
      break;
    }
  }
}

void CUtil::AppendXrefs(UINT *pIn, UINT in_count, UINT buffer_base, UINT min, UINT max, OFFSET_XREF **pOut, UINT *out_count, UINT magic)
{
  int i;
  UINT val;
  UINT old_count = *out_count;
  OFFSET_XREF *pTemp;

  //make a temp copy of existing data
  if(*pOut)
  {
    pTemp = new OFFSET_XREF[old_count];
    memcpy(pTemp, *pOut, sizeof(OFFSET_XREF)*old_count);
  }

  //count number of xrefs
  *out_count = 0;
  for(i=0; i<in_count; i++)
  {
    val = pIn[i] - magic;
    
    if((val >= min)&&(val <= max))
      (*out_count)++;
  }

  //build xref list
  if((*out_count)+old_count)
  {
    if(*pOut)
      delete [] *pOut;

    *pOut = new OFFSET_XREF[(*out_count)+old_count];
    memcpy(*pOut, pTemp, sizeof(OFFSET_XREF)*old_count);
  }

  *out_count = old_count;

  if(*pOut)
  {
    for(i=0; i<in_count; i++)
    {
      val = pIn[i] - magic;
    
      if((val >= min)&&(val <= max))
      {
        (*pOut)[*out_count].source = buffer_base + i*4;
        (*pOut)[*out_count].dest = val;
        (*out_count)++;
      }
    }
  }
}

BOOL CUtil::CompressFloatToShort(float input, SHORT &output)
{
  int temp;

  if(input > 1.0)
    input = 1.0;
  if(input < -1.0)
    input = -1.0;

  temp = (int)floor(input*32767.5);

  output = temp;

  return(FALSE);
}

void CUtil::DumpReflexiveInfo(REFLEXIVE *pRefl, CString Name, int struct_size, UINT magic)
{
  CString str;

  str.Format("%08X : %08X  %s %d\n", pRefl->Offset - magic, 
    pRefl->Offset - magic + pRefl->Count*struct_size, Name, pRefl->Count);
  TRACE(str);
}
