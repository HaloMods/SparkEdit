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
#include "HaloBmp2.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*-------------------------------------------------------------------
 * Name: CHaloBmp()
 * Description:
 *   
 *-----------------------------------------------------------------*/
CHaloBmp::CHaloBmp()
{
  m_ObjTag = "bitm";
  m_Version = 0;
}

/*-------------------------------------------------------------------
 * Name: ~CHaloBmp()
 * Description:
 *   
 *-----------------------------------------------------------------*/
CHaloBmp::~CHaloBmp()
{

}

/*-------------------------------------------------------------------
 * Name: writeTGA()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CHaloBmp::writeTGA(UINT *texdata, int height, int width, CFile *pOutFile)
{
	MTarga_Header	hdr;
	
	hdr.id_length = 0;
	hdr.colormap_type = 0;
	hdr.image_type = 2;
	hdr.colormap_index = 0;
	hdr.colormap_length = 0;
	hdr.colormap_size = 0;
	hdr.x_org = 0;
	hdr.y_org = 0;
	hdr.width = width;
	hdr.height = height;
	hdr.pixel_size = 32;
	hdr.attributes = 0;
	
	// WRITE that shit, yo!
  pOutFile->Write(&hdr, sizeof(hdr));

	int *datptr = (int *)texdata;

	//for (int y = 0; y < height; y++)
	//	for (int x = 0; x < width; x++, datptr++)
	//		*datptr = Swap ((int)*datptr);
	
	for (int y = height - 1; y >= 0; y--)
		pOutFile->Write(&(texdata[y * width]), 4 * width);
}

/*-------------------------------------------------------------------
 * Name: read_bitm()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CHaloBmp::read_bitm(CFile *pMapFile, INDEX_ITEM *item, int magic, CString Name)
{
	bitm_header_t		hdr;
	bitm_first_t		first;
//	bitm_second_t		second;
  bitm_image_t    *pImages = NULL;
  BOOL bWriteOutput = FALSE;

  /* Read bmp header from metadata */ 
	pMapFile->Seek(item->offset,0);
  pMapFile->Read(&hdr, sizeof(hdr));

  hdr.offset_to_first -= magic;
	hdr.image_offset -= magic;

  /* Read 1st section */ 
	pMapFile->Seek(hdr.offset_to_first,0);
  pMapFile->Read(&first, sizeof(first));
	
  /* New format - multiple images in the raw data */ 
	pImages = new bitm_image_t[hdr.image_count];

	pMapFile->Seek(hdr.image_offset,0);
  pMapFile->Read(pImages, hdr.image_count*sizeof(bitm_image_t));

  for(int i=0; i<hdr.image_count; i++)
  {
    if(pImages[i].type == BITM_TYPE_2D)
    {
			if(!pImages[i].num_mipmaps) //no mipmaps
			{
				//DecodeBitmSurface (texdata, image->width, image->height, -1, image->format, image->flags, imagefile);			
			}
			else
			{
				int	sx = pImages[i].width;
        int sy = pImages[i].height;
				int	offset = 0;
			
				for(int y = 0; y < 1; y++)//pImages[i].num_mipmaps; y++)
				{
					//DecodeBitmSurface(texdata + offset, sx, sy, -1, image->format, image->flags, imagefile);
				
					offset += GetImageSize (pImages[i].format, sx, sy);				
					sx >>= 1;
					sy >>= 1;
				}
			}
    }
  }
}

////////////////////////////////////////////////////////////////////////
//                                                                    //
// DXT Format Converter                                               //
// Copyright 2003, Defenderz                                          //
// Open for Free Use and Export                                       //
// DXT Data Format Copyright Microsoft Inc.                           //
// Notes:                                                             //
// -Height/Width should be powers of 2 (By default for D3D Textures)  //
//                                                                    //
// -IData should point to the DXT data, and should point to a buffer  //
// of size Height*Width/2                                             //
//                                                                    //
// -PData should point to a preallocated storage buffer of size       //
// Width*Height*4 (Width*Height DWORDs)                               //
//                                                                    //
// -Function will never return a fail code :-)                        //
////////////////////////////////////////////////////////////////////////

#define BIT16BMASK (0x01|0x02|0x04|0x08|0x10) 
#define BIT16GMASK (0x20|0x40|0x80|0x100|0x200|0x400) 
#define BIT16RMASK (0x800|0x1000|0x2000|0x4000|0x8000) 
 
#define BIT16BSIZE     32 
#define BIT16GSIZE     64 
#define BIT16RSIZE     32 
 
#define BIT16RDIST     11 
#define BIT16GDIST     5 
#define BIT16BDIST     0 

struct RGB
{
	DWORD R,G,B,T;
};

/*-------------------------------------------------------------------
 * Name: ConvertWORDToRGB()
 * Description:
 *   
 *-----------------------------------------------------------------*/
static RGB ConvertWORDToRGB(WORD Color)
{
	//Color = Swap ((short)Color);

	RGB rc;
	rc.R = ((Color&BIT16RMASK)>>BIT16RDIST)*0xff/BIT16RSIZE; 
	rc.G = ((Color&BIT16GMASK)>>BIT16GDIST)*0xff/BIT16GSIZE; 
	rc.B = ((Color&BIT16BMASK)>>BIT16BDIST)*0xff/BIT16BSIZE;
	rc.T = 255;

	return rc;
}

/*-------------------------------------------------------------------
 * Name: RGBToDWORD()
 * Description:
 *   
 *-----------------------------------------------------------------*/
static DWORD RGBToDWORD(RGB Color) 
{ 
	return (Color.T<<24)|(Color.R<<16)|(Color.G<<8)|Color.B;
}


/*-------------------------------------------------------------------
 * Name: GradientColors()
 * Description:
 *   
 *-----------------------------------------------------------------*/
rgba_color_t CHaloBmp::GradientColors (rgba_color_t Col1, rgba_color_t Col2)
{
	rgba_color_t ret;
	ret.r = ((Col1.r*2 + Col2.r))/3;
	ret.g = ((Col1.g*2 + Col2.g))/3;
	ret.b = ((Col1.b*2 + Col2.b))/3;
	ret.a = 255;
	return ret;
}

/*-------------------------------------------------------------------
 * Name: GradientColorsHalf()
 * Description:
 *   
 *-----------------------------------------------------------------*/
rgba_color_t CHaloBmp::GradientColorsHalf (rgba_color_t Col1, rgba_color_t Col2)
{
	rgba_color_t ret;
	ret.r = (Col1.r/2 + Col2.r/2);
	ret.g = (Col1.g/2 + Col2.g/2);
	ret.b = (Col1.b/2 + Col2.b/2);
	ret.a = 255;
	return ret;
}

/*-------------------------------------------------------------------
 * Name: DecodeDXT1()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CHaloBmp::DecodeDXT1(int Height, int Width, byte* IData, unsigned int* PData)
{
	rgba_color_t		Color[4];
	int					i;
	int					dptr;
	rgba_color_t		CColor;
	unsigned int		CData;
	unsigned int		ChunksPerHLine = Width / 4;
	bool				trans;
	static rgba_color_t	zeroColor = {0, 0, 0, 0};
	
	for (i = 0, dptr = 0; i < (Width * Height); i += 16, dptr += 8)
	{
		unsigned short	c1, c2;
		
		c1 = *(unsigned short *)&IData[dptr];
		c2 = *(unsigned short *)&IData[dptr+2];

		if(c1 > c2)
			trans = false;
		else
			trans = true;
	
		Color[0] = short_to_rgba(c1);
		Color[1] = short_to_rgba(c2);
		if (!trans)
		{
			Color[2] = GradientColors(Color[0], Color[1]);
			Color[3] = GradientColors(Color[1], Color[0]);
		}
		else
		{
			Color[2] = GradientColorsHalf (Color[0], Color[1]);
			Color[3] = zeroColor;
		}
		
		CData = *(unsigned int *)&(IData[dptr+4]);

		unsigned int ChunkNum = i / 16;
		unsigned int XPos = ChunkNum % ChunksPerHLine;
		unsigned int YPos = (ChunkNum - XPos) / ChunksPerHLine;
		
		int		sizew, sizeh;

		sizeh = (Height < 4) ? Height : 4;
		sizew = (Width < 4) ? Width : 4;

		for (int x = 0; x < sizeh; x++)
		{
			for (int y = 0; y < sizew; y++)
			{
				CColor = Color[CData & 0x03];
				CData >>= 2;
				PData[(YPos*4+x)*Width + XPos*4+y] = rgba_to_int(CColor);
			}
		}
	}
}


/*-------------------------------------------------------------------
 * Name: ReadHeader()
 * Description:
 *   
 *-----------------------------------------------------------------*/
void CHaloBmp::ReadHeader(CFile *pMapFile, INDEX_ITEM *item, int magic, CString Name)
{
//	bitm_header_t		hdr;
//	bitm_first_t		first;
//	//bitm_second_t		second;
//
//  /* Read bmp header from metadata */ 
//	pMapFile->Seek(item->offset,0);
//  pMapFile->Read(&hdr, sizeof(hdr));
//
//  printf("hdr_offset_to_first = %08X  offset to second = %08X\n", hdr.offset_to_first, hdr.offset_to_second);
//  hdr.offset_to_first -= magic;
//	//hdr.offset_to_second -= magic;
//
//  /* Read 1st section */ 
//	pMapFile->Seek(hdr.offset_to_first,0);
//  pMapFile->Read(&first, sizeof(first));
//	
//  /* Read 2nd section */ 
//	//pMapFile->Seek(hdr.offset_to_second,0);
//  //pMapFile->Read(&second, sizeof(second));
//
//  /* Dump Info */ 
//  //DumpInfo(second.offset, second.size, Name);
}

/*================================
 * GetImageSize 
================================*/
int CHaloBmp::GetImageSize (int format, int width, int height)
{
	int		size = 0;
	
	if (!width)
		width = 1;
	
	if (!height)
		height = 1;
	
	if (format == BITM_FORMAT_DXT1 || format == BITM_FORMAT_DXT2AND3 || format == BITM_FORMAT_DXT4AND5)
	{
		if (width < 4)
			width = 4;
		if (height < 4)
			height = 4;
	}

	switch (format)
	{
		case BITM_FORMAT_A8:
		case BITM_FORMAT_Y8:
		case BITM_FORMAT_AY8:
		case BITM_FORMAT_P8:
			size = width * height;
			break;
		
		case BITM_FORMAT_A8Y8:
		case BITM_FORMAT_R5G6B5:
		case BITM_FORMAT_A1R5G5B5:
		case BITM_FORMAT_A4R4G4B4:
			size = width * height * 2;
			break;
		
		case BITM_FORMAT_X8R8G8B8:
		case BITM_FORMAT_A8R8G8B8:
			size = width * height * 4;
			break;
		
		case BITM_FORMAT_DXT1:
			size = (width >> 2) * (height >> 2) * 8;
			break;

		case BITM_FORMAT_DXT2AND3:
		case BITM_FORMAT_DXT4AND5:
			size = (width >> 2) * (height >> 2) * 16;
			break;
		
		default:
			fprintf (stderr, "Unknown format in GetImageSize!\n");
			break;
	}
	
	return size;
}

/*================================
 * DecodeBitmSurface 
================================*/
void CHaloBmp::DecodeBitmSurface (byte *data, int width, int height, int depth, 
                                   int format, int flags, UINT *pOutBuf)
{
	if (!width)
		width = 1;
	
	if (!height)
		height = 1;

	if((flags & BITM_FLAG_LINEAR)||(m_Version == 7)||(m_Version == 609))
	{
		switch (format)
		{
			case BITM_FORMAT_A8:		DecodeLinearA8 (width, height, data, pOutBuf); break;
			case BITM_FORMAT_Y8:		DecodeLinearY8 (width, height, data, pOutBuf); break;
			case BITM_FORMAT_AY8:		DecodeLinearAY8 (width, height, data, pOutBuf); break;
			case BITM_FORMAT_A8Y8:		DecodeLinearA8Y8 (width, height, data, pOutBuf); break;
			case BITM_FORMAT_R5G6B5:	DecodeLinearR5G6B5 (width, height, data, pOutBuf); break;
			case BITM_FORMAT_A1R5G5B5:	DecodeLinearA1R5G5B5 (width, height, data, pOutBuf); break;
			case BITM_FORMAT_A4R4G4B4:	DecodeLinearA4R4G4B4 (width, height, data, pOutBuf); break;
			case BITM_FORMAT_X8R8G8B8:	DecodeLinearX8R8G8B8 (width, height, data, pOutBuf); break;
			case BITM_FORMAT_A8R8G8B8:	DecodeLinearA8R8G8B8 (width, height, data, pOutBuf); break;
			case BITM_FORMAT_DXT1:		DecodeDXT1 (height, width, data, pOutBuf); break;
			case BITM_FORMAT_DXT2AND3:	DecodeDXT2And3 (height, width, data, pOutBuf); break;
			case BITM_FORMAT_DXT4AND5:	DecodeDXT4And5 (height, width, data, pOutBuf); break;
			case BITM_FORMAT_P8:		DecodeLinearP8 (width, height, data, pOutBuf); break;
			
			default:
				//fprintf (stderr, "Unknown format %x (%s) (linear)!\n", format, outfile->getpath());
				return;
				break;
		}
	}
	else	// Swizzled
	{	
		switch (format)
		{
			case BITM_FORMAT_A8:		DecodeSwizzledA8 (width, height, depth, data, pOutBuf); break;
			case BITM_FORMAT_Y8:		DecodeSwizzledY8 (width, height, depth, data, pOutBuf); break;
			case BITM_FORMAT_AY8:		DecodeSwizzledAY8 (width, height, depth, data, pOutBuf); break;
			case BITM_FORMAT_A8Y8:		DecodeSwizzledA8Y8 (width, height, depth, data, pOutBuf); break;
			case BITM_FORMAT_R5G6B5:	DecodeSwizzledR5G6B5 (width, height, depth, data, pOutBuf); break;
			case BITM_FORMAT_A1R5G5B5:	DecodeSwizzledA1R5G5B5 (width, height, depth, data, pOutBuf); break;
			case BITM_FORMAT_A4R4G4B4:	DecodeSwizzledA4R4G4B4 (width, height, depth, data, pOutBuf); break;
			case BITM_FORMAT_X8R8G8B8:	DecodeSwizzledX8R8G8B8 (width, height, depth, data, pOutBuf); break;
			case BITM_FORMAT_A8R8G8B8:	DecodeSwizzledA8R8G8B8 (width, height, depth, data, pOutBuf); break;
			case BITM_FORMAT_DXT1:		DecodeDXT1 (height, width, data, pOutBuf); break;
			case BITM_FORMAT_DXT2AND3:	DecodeDXT2And3 (height, width, data, pOutBuf); break;
			case BITM_FORMAT_DXT4AND5:	DecodeDXT4And5 (height, width, data, pOutBuf); break;
			case BITM_FORMAT_P8:		DecodeSwizzledP8 (width, height, depth, data, pOutBuf); break;
			
			default:
				//fprintf (stderr, "Unknown format %x (%s) (swizzled)!\n", format, outfile->getpath());
				return;
				break;
		}
	}
}

/*================================
 * DecodeLinearA8 
================================*/
void CHaloBmp::DecodeLinearA8 (int width, int height, byte *texdata, unsigned int *outdata)
{
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++)
			outdata[(y * width) + x] = texdata[(y * width) + x] << 24;
}

/*================================
 * DecodeSwizzledA8 
================================*/
void CHaloBmp::DecodeSwizzledA8 (int width, int height, int depth, byte *texdata, unsigned int *outdata)
{
	Swizzler		swz (width, height);
	
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++)
			outdata[(y * width) + x] = texdata[swz.Swizzle (x, y, depth)] << 24;
}

/*================================
 * DecodeLinearY8 
================================*/
void CHaloBmp::DecodeLinearY8 (int width, int height, byte *texdata, unsigned int *outdata)
{
	rgba_color_t	color;

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			color.r = color.g = color.b = texdata[(y * width) + x];
			color.a = 0;
			
			outdata[(y * width) + x] = rgba_to_int (color);
		}
	}
}

/*================================
 * DecodeSwizzledY8 
================================*/
void CHaloBmp::DecodeSwizzledY8 (int width, int height, int depth, byte *texdata, unsigned int *outdata)
{
	rgba_color_t	color;
	Swizzler		swz (width, height);

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			color.r = color.g = color.b = texdata[swz.Swizzle (x, y, depth)];
			color.a = 0;
			
			outdata[(y * width) + x] = rgba_to_int (color);
		}
	}
}

/*================================
 * DecodeLinearAY8 
================================*/
void CHaloBmp::DecodeLinearAY8 (int width, int height, byte *texdata, unsigned int *outdata)
{
	rgba_color_t	color;

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			color.r = color.g = color.b = color.a = texdata[(y * width) + x];
			outdata[(y * width) + x] = rgba_to_int (color);
		}
	}
}

/*================================
 * DecodeSwizzledAY8 
================================*/
void CHaloBmp::DecodeSwizzledAY8 (int width, int height, int depth, byte *texdata, unsigned int *outdata)
{
	rgba_color_t	color;
	Swizzler		swz (width, height);

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			color.r = color.g = color.b = color.a = texdata[swz.Swizzle (x, y, depth)];			
			outdata[(y * width) + x] = rgba_to_int (color);
		}
	}
}

/*================================
 * DecodeLinearA8Y8 
================================*/
void CHaloBmp::DecodeLinearA8Y8 (int width, int height, byte *texdata, unsigned int *outdata)
{
	rgba_color_t	color;
	unsigned short	cdata;

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			cdata = ((short *)texdata)[(y * width) + x];
			color.r = color.g = color.b = cdata >> 8;
			color.a = cdata & 0xFF;
			outdata[(y * width) + x] = rgba_to_int (color);
		}
	}
}

/*================================
 * DecodeSwizzledA8Y8 
================================*/
void CHaloBmp::DecodeSwizzledA8Y8 (int width, int height, int depth, byte *texdata, unsigned int *outdata)
{
	rgba_color_t	color;
	Swizzler		swz (width, height);
	unsigned short	cdata;

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			cdata = ((short *)texdata)[swz.Swizzle (x, y, depth)];
		
			color.r = color.g = color.b = cdata >> 8;
			color.a = cdata & 0xFF;
			outdata[(y * width) + x] = rgba_to_int (color);
		}
	}
}

/*================================
 * DecodeLinearR5G6B5 
================================*/
void CHaloBmp::DecodeLinearR5G6B5 (int width, int height, byte *texdata, unsigned int *outdata)
{
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++)
			outdata[(y * width) + x] = rgba_to_int (short_to_rgba (((short *)texdata)[(y * width) + x]));
}

/*================================
 * DecodeSwizzledR5G6B5 
================================*/
void CHaloBmp::DecodeSwizzledR5G6B5 (int width, int height, int depth, byte *texdata, unsigned int *outdata)
{
	Swizzler		swz (width, height);

	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++)
			outdata[(y * width) + x] = rgba_to_int (short_to_rgba (((short *)texdata)[swz.Swizzle (x, y, depth)]));
}

/*================================
 * DecodeLinearA1R5G5B5 
================================*/
void CHaloBmp::DecodeLinearA1R5G5B5 (int width, int height, byte *texdata, unsigned int *outdata)
{
	rgba_color_t	color;
	unsigned short	cdata;

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			cdata = ((short *)texdata)[(y * width) + x];
			
			color.a = (cdata >> 15) * 0xFF;
			color.r = (((cdata >> 10) & 0x1F) * 0xFF) / 31;
			color.g = (((cdata >>  5) & 0x1F) * 0xFF) / 31;
			color.b = (((cdata >>  0) & 0x1F) * 0xFF) / 31;

			outdata[(y * width) + x] = rgba_to_int (color);
		}
	}
}

/*================================
 * DecodeSwizzledA1R5G5B5 
================================*/
void CHaloBmp::DecodeSwizzledA1R5G5B5 (int width, int height, int depth, byte *texdata, unsigned int *outdata)
{
	Swizzler		swz (width, height);
	rgba_color_t	color;
	unsigned short	cdata;

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			cdata = ((short *)texdata)[swz.Swizzle (x, y, depth)];
			
			color.a = (cdata >> 15) * 0xFF;
			color.r = (((cdata >> 10) & 0x1F) * 0xFF) / 31;
			color.g = (((cdata >>  5) & 0x1F) * 0xFF) / 31;
			color.b = (((cdata >>  0) & 0x1F) * 0xFF) / 31;

			outdata[(y * width) + x] = rgba_to_int (color);
		}
	}
}

/*================================
 * DecodeLinearA4R4G4B4 
================================*/
void CHaloBmp::DecodeLinearA4R4G4B4 (int width, int height, byte *texdata, unsigned int *outdata)
{
	rgba_color_t	color;
	unsigned short	cdata;

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			cdata = ((short *)texdata)[(y * width) + x];
			
			color.a = ((cdata >> 12) * 0xFF) / 15;
			color.r = (((cdata >>  8) & 0x0F) * 0xFF) / 15;
			color.g = (((cdata >>  4) & 0x0F) * 0xFF) / 15;
			color.b = (((cdata >>  0) & 0x0F) * 0xFF) / 15;

			outdata[(y * width) + x] = rgba_to_int (color);
		}
	}
}

/*================================
 * DecodeSwizzledA4R4G4B4 
================================*/
void CHaloBmp::DecodeSwizzledA4R4G4B4 (int width, int height, int depth, byte *texdata, unsigned int *outdata)
{
	Swizzler		swz (width, height);
	rgba_color_t	color;
	unsigned short	cdata;

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			cdata = ((short *)texdata)[swz.Swizzle (x, y, depth)];
			
			color.a = ((cdata >> 12) * 0xFF) / 15;
			color.r = (((cdata >>  8) & 0x0F) * 0xFF) / 15;
			color.g = (((cdata >>  4) & 0x0F) * 0xFF) / 15;
			color.b = (((cdata >>  0) & 0x0F) * 0xFF) / 15;

			outdata[(y * width) + x] = rgba_to_int (color);
		}
	}
}

/*================================
 * DecodeLinearX8R8G8B8 
================================*/
void CHaloBmp::DecodeLinearX8R8G8B8 (int width, int height, byte *texdata, unsigned int *outdata)
{
	rgba_color_t	color;
	unsigned int	cdata;

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			cdata = ((int *)texdata)[(y * width) + x];			
			color.a = 0;
			color.r = (cdata >> 16) & 0xFF;
			color.g = (cdata >>  8) & 0xFF;
			color.b = (cdata >>  0) & 0xFF;

			outdata[(y * width) + x] = rgba_to_int (color);
		}
	}
}

/*================================
 * DecodeSwizzledX8R8G8B8 
================================*/
void CHaloBmp::DecodeSwizzledX8R8G8B8 (int width, int height, int depth, byte *texdata, unsigned int *outdata)
{
	Swizzler		swz (width, height);
	//Swizzler		swz (height, width);
	rgba_color_t	color;
	unsigned int	cdata;

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			cdata = ((int *)texdata)[(y * width) + x];
      
//JPC 12/18/03 - removed swizzle call
//			cdata = ((int *)texdata)[swz.Swizzle (x, y, depth)];
			
			color.a = 0;
			color.r = (cdata >> 16) & 0xFF;
			color.g = (cdata >>  8) & 0xFF;
			color.b = (cdata >>  0) & 0xFF;

			outdata[(y * width) + x] = rgba_to_int (color);
		}
	}
}

/*================================
 * DecodeLinearA8R8G8B8 
================================*/
void CHaloBmp::DecodeLinearA8R8G8B8 (int width, int height, byte *texdata, unsigned int *outdata)
{
	rgba_color_t	color;
	unsigned int	cdata;

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			cdata = ((int *)texdata)[(y * width) + x];
			
			color.a = (cdata >> 24);
			color.r = (cdata >> 16) & 0xFF;
			color.g = (cdata >>  8) & 0xFF;
			color.b = (cdata >>  0) & 0xFF;

			outdata[(y * width) + x] = rgba_to_int (color);
		}
	}
}

/*================================
 * DecodeSwizzledA8R8G8B8 
================================*/
void CHaloBmp::DecodeSwizzledA8R8G8B8 (int width, int height, int depth, byte *texdata, unsigned int *outdata)
{
	Swizzler		swz (width, height);
	rgba_color_t	color;
	unsigned int	cdata;

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			cdata = ((int *)texdata)[swz.Swizzle (x, y, depth)];
			
			color.a = (cdata >> 24);
			color.r = (cdata >> 16) & 0xFF;
			color.g = (cdata >>  8) & 0xFF;
			color.b = (cdata >>  0) & 0xFF;

			outdata[(y * width) + x] = rgba_to_int (color);
		}
	}
}

/*================================
 * DecodeLinearP8 
================================*/
void CHaloBmp::DecodeLinearP8 (int width, int height, byte *texdata, unsigned int *outdata)
{
	rgba_color_t	color;

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			color.a = 0;
			color.r = color.g = color.b = texdata[(y * width) + x];
			outdata[(y * width) + x] = rgba_to_int (color);
		}
	}
}

/*================================
 * DecodeSwizzledP8 
================================*/
void CHaloBmp::DecodeSwizzledP8 (int width, int height, int depth, byte *texdata, unsigned int *outdata)
{
	Swizzler		swz (width, height);
	rgba_color_t	color;

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			color.a = 0;
			color.r = color.g = color.b = texdata[swz.Swizzle (x, y, depth)];
			outdata[(y * width) + x] = rgba_to_int (color);
		}
	}
}

/*================================
 * DecodeDXT2And3 
================================*/
void CHaloBmp::DecodeDXT2And3 (int Height, int Width, byte* IData, unsigned int* PData)
{
	rgba_color_t	Color[4];
	int				i;
	rgba_color_t	CColor;
	unsigned int	CData;
	unsigned int	ChunksPerHLine = Width / 4;
	unsigned short	Alpha;
	
	if (!ChunksPerHLine)
		ChunksPerHLine = 1;
	
	for (i = 0; i < (Width * Height); i += 16)
	{
		Color[0] = short_to_rgba(*(unsigned short *)&IData[i+8]);
		Color[1] = short_to_rgba(*(unsigned short *)&IData[i+10]);
		Color[2] = GradientColors(Color[0], Color[1]);
		Color[3] = GradientColors(Color[1], Color[0]);

		CData = *(unsigned int *)&IData[i+12];

		unsigned int ChunkNum = i / 16;
		unsigned int XPos = ChunkNum % ChunksPerHLine;
		unsigned int YPos = (ChunkNum - XPos) / ChunksPerHLine;
		
		int		sizew, sizeh;

		sizeh = (Height < 4) ? Height : 4;
		sizew = (Width < 4) ? Width : 4;
		
		for(int x = 0; x < sizeh; x++)
		{
			Alpha = *(short *)&IData[i + (2 * x)];
		
			for(int y = 0; y < sizew; y++)
			{
				CColor = Color[CData & 0x03];
				CData >>= 2;

				CColor.a = (Alpha & 0x0F) * 16;
				Alpha >>= 4;

				PData[(YPos*4+x)*Width + XPos*4+y] = rgba_to_int(CColor);
			}
		}
	}
}

/*================================
 * DecodeDXT4And5 
================================*/
void CHaloBmp::DecodeDXT4And5 (int Height, int Width, byte* IData, unsigned int* PData)
{
	rgba_color_t	Color[4];
	int				i;
	rgba_color_t	CColor;
	unsigned int	CData;
	unsigned int	ChunksPerHLine = Width / 4;
	unsigned char	Alpha[8];
	__int64		AlphaDat;
	
	if (!ChunksPerHLine)
		ChunksPerHLine = 1;
	
	for (i = 0; i < (Width * Height); i += 16)
	{
		Color[0] = short_to_rgba(*(unsigned short *)&IData[i+8]);
		Color[1] = short_to_rgba(*(unsigned short *)&IData[i+10]);
		Color[2] = GradientColors(Color[0], Color[1]);
		Color[3] = GradientColors(Color[1], Color[0]);

		CData = (int)*(unsigned int *)&IData[i+12];

		Alpha[0] = IData[i];
		Alpha[1] = IData[i+1];

		// Do the alphas
		if (Alpha[0] > Alpha[1])
		{    
			// 8-alpha block:  derive the other six alphas.
			// Bit code 000 = alpha_0, 001 = alpha_1, others are interpolated.
			Alpha[2] = (6 * Alpha[0] + 1 * Alpha[1] + 3) / 7;	// bit code 010
			Alpha[3] = (5 * Alpha[0] + 2 * Alpha[1] + 3) / 7;	// bit code 011
			Alpha[4] = (4 * Alpha[0] + 3 * Alpha[1] + 3) / 7;	// bit code 100
			Alpha[5] = (3 * Alpha[0] + 4 * Alpha[1] + 3) / 7;	// bit code 101
			Alpha[6] = (2 * Alpha[0] + 5 * Alpha[1] + 3) / 7;	// bit code 110
			Alpha[7] = (1 * Alpha[0] + 6 * Alpha[1] + 3) / 7;	// bit code 111
		}
		else
		{
			// 6-alpha block.
			// Bit code 000 = alpha_0, 001 = alpha_1, others are interpolated.
			Alpha[2] = (4 * Alpha[0] + 1 * Alpha[1] + 2) / 5;	// Bit code 010
			Alpha[3] = (3 * Alpha[0] + 2 * Alpha[1] + 2) / 5;	// Bit code 011
			Alpha[4] = (2 * Alpha[0] + 3 * Alpha[1] + 2) / 5;	// Bit code 100
			Alpha[5] = (1 * Alpha[0] + 4 * Alpha[1] + 2) / 5;	// Bit code 101
			Alpha[6] = 0;										// Bit code 110
			Alpha[7] = 255;										// Bit code 111
		}
		
		// Byte	Alpha
		// 0	Alpha_0
		// 1	Alpha_1 
		// 2	[0][2] (2 LSBs), [0][1], [0][0]
		// 3	[1][1] (1 LSB), [1][0], [0][3], [0][2] (1 MSB)
		// 4	[1][3], [1][2], [1][1] (2 MSBs)
		// 5	[2][2] (2 LSBs), [2][1], [2][0]
		// 6	[3][1] (1 LSB), [3][0], [2][3], [2][2] (1 MSB)
		// 7	[3][3], [3][2], [3][1] (2 MSBs)
		// [0
		
		// Read an int and a short
		unsigned int	tmpdword;
		unsigned short	tmpword;
		
		tmpword = (short)*(unsigned short *)&IData[i + 2];
		tmpdword = (int)*(unsigned int *)&IData[i + 4];

		AlphaDat = tmpword | ((__int64)tmpdword << 16);

		unsigned int ChunkNum = i / 16;
		unsigned int XPos = ChunkNum % ChunksPerHLine;
		unsigned int YPos = (ChunkNum - XPos) / ChunksPerHLine;
		
		int		sizew, sizeh;

		sizeh = (Height < 4) ? Height : 4;
		sizew = (Width < 4) ? Width : 4;

		for (int x = 0; x < sizeh; x++)
		{
			for (int y = 0; y < sizew; y++)
			{
				CColor = Color[CData & 0x03];
				CData >>= 2;

				CColor.a = Alpha[AlphaDat & 0x07];
				AlphaDat >>= 3;

				PData[(YPos*4+x)*Width + XPos*4+y] = rgba_to_int(CColor);
			}
		}
	}
}

BOOL CHaloBmp::DecompressDXT(bitm_image_t *pHeader, 
                              int inbuf_size, 
                              BYTE *pInBuf, 
                              UINT *pOutBuf)
{
//  for(int i=0; i<hdr.image_count; i++)
//  {
    if(pHeader[0].type == BITM_TYPE_2D)
    {
			if(!pHeader[0].num_mipmaps) //no mipmaps
			{
				DecodeBitmSurface(pInBuf, 
                          pHeader[0].width, 
                          pHeader[0].height, 
                          -1, 
                          pHeader[0].format, 
                          pHeader[0].flags, 
                          pOutBuf);			
			}
			else
			{
				int	sx = pHeader[0].width;
        int sy = pHeader[0].height;
				int	offset = 0;
			
				for(int y = 0; y < 1; y++)//pImages[i].num_mipmaps; y++)
				{
					DecodeBitmSurface(pInBuf, 
                            pHeader[0].width, 
                            pHeader[0].height, 
                            -1, 
                            pHeader[0].format, 
                            pHeader[0].flags, 
                            pOutBuf);			
				
					offset += GetImageSize (pHeader[0].format, sx, sy);				
					sx >>= 1;
					sy >>= 1;
				}
			}
    }
//  }
  return(FALSE);
}
