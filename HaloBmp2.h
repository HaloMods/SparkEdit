/*-------------------------------------------------------------------
 *  Original Author: Grenadiac
 *
 *  Contributing Authors:
 *
 *  File Description: 
 *    
 *
 *-----------------------------------------------------------------*/
#if !defined(AFX_HALOBMP_H__CE1D3B80_72DD_4649_8845_C1DCFBF3A291__INCLUDED_)
#define AFX_HALOBMP_H__CE1D3B80_72DD_4649_8845_C1DCFBF3A291__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HaloStructDefs.h"
#include "Util.h"

typedef struct
{
  char            unk1[4];
  UINT            version;
  UINT            unk2[6];
  char            unk3[32];
  float           unk4[3];
  UINT            unk5[3];
	//int							unknown[22]; // [7] == [6]+108[9]
	int							offset_to_first;
	int							unknown23;	// always 0x0
	int							image_count;
	int							image_offset;
	int							unknown25;	// always 0x0

} bitm_header_t;


typedef struct
{
	int							unknown[16];

} bitm_first_t;

typedef struct
{
	int							id;			// 'bitm'
	short						width;
	short						height;
	short						depth;
	short						type;
	short						format;
	short						flags;
	short						reg_point_x;
	short						reg_point_y;
	short						num_mipmaps;
	short						pixel_offset;
	int         				offset;
	int							size;
	int							unknown8;
	int							unknown9;	// always 0xFFFFFFFF?
	int							unknown10;	// always 0x00000000?
	int							unknown11;	// always 0x024F0040?
} bitm_image_t;

// formats
#define BITM_FORMAT_A8			0x00
#define BITM_FORMAT_Y8			0x01
#define BITM_FORMAT_AY8			0x02
#define BITM_FORMAT_A8Y8		0x03
#define BITM_FORMAT_R5G6B5		0x06
#define BITM_FORMAT_A1R5G5B5	0x08
#define BITM_FORMAT_A4R4G4B4	0x09
#define BITM_FORMAT_X8R8G8B8	0x0A
#define BITM_FORMAT_A8R8G8B8	0x0B
#define BITM_FORMAT_DXT1		0x0E
#define BITM_FORMAT_DXT2AND3	0x0F
#define BITM_FORMAT_DXT4AND5	0x10
#define BITM_FORMAT_P8			0x11

// Types
#define BITM_TYPE_2D			0x00
#define BITM_TYPE_3D			0x01
#define BITM_TYPE_CUBEMAP		0x02

// Flags
#define BITM_FLAG_LINEAR		(1 << 4)

struct rgba_color_t
{
	unsigned int r, g, b, a;
};

typedef struct
{
	unsigned char		id_length, colormap_type, image_type;
	unsigned short		colormap_index, colormap_length;
	unsigned char		colormap_size;
	unsigned short		x_org, y_org, width, height;
	unsigned char		pixel_size, attributes;

} MTarga_Header;

static unsigned int rgba_to_int (rgba_color_t color) 
{ 
	return (color.a << 24) | (color.r << 16) | (color.g << 8) | color.b;
}

static rgba_color_t short_to_rgba (unsigned short color)
{
	rgba_color_t rc;
	rc.r = (((color >> 11) & 0x1F) * 0xFF) / 31;
	rc.g = (((color >>  5) & 0x3F) * 0xFF) / 63;
	rc.b = (((color >>  0) & 0x1F) * 0xFF) / 31;
	rc.a = 255;

	return rc;
}

// Swizzle class. I've got no idea how this works, but it does. Thanks Stephen. :)

// Hmm, maybe there are some problems with it...
class Swizzler
{
public:
	Swizzler(int Width, int Height, int Depth = 0)
	{
		m_MaskX = 0;
		m_MaskY = 0;
		m_MaskZ = 0;

		for(int Bit(1), Idx(1); (Bit < Width) || (Bit < Height) || (Bit < Depth); Bit <<= 1)
		{
			if (Bit < Width)
			{
				m_MaskX |= Idx;
				Idx <<= 1;
			}

			if (Bit < Height)
			{
				m_MaskY |= Idx;
				Idx <<= 1;
			}

			if (Bit < Depth)
			{
				m_MaskZ |= Idx;
				Idx <<= 1;
			}
		}
	}

public:
	unsigned int Swizzle(int Sx, int Sy, int Sz = -1)
	{
		return SwizzleAxis(Sx, m_MaskX) | SwizzleAxis(Sy, m_MaskY) | ((Sz != -1) ? SwizzleAxis(Sz, m_MaskZ) : 0);
	}

private:
	unsigned int SwizzleAxis(int Value, const int Mask)
	{
		unsigned int Result(0);

		for(int Bit = 1; Bit <= Mask; Bit <<= 1)
		{
			if (Mask & Bit)
				Result |= Value & Bit;
			else
				Value <<= 1;
		}

		return Result;
	}

private:
	int m_MaskX, m_MaskY, m_MaskZ;
};


class CHaloBmp : public CUtil
{
public:
  BOOL DecompressDXT(bitm_image_t *pHeader,
                     int inbuf_size, 
                     BYTE *pInBuf, 
                     UINT *pOutBuf);
	CHaloBmp();
	virtual ~CHaloBmp();
  void ReadHeader(CFile *pMapFile, INDEX_ITEM *item, int magic, CString Name);
  void read_bitm(CFile *pMapFile, INDEX_ITEM *item, int magic, CString Name);
  void writeTGA(UINT *texdata, int height, int width, CFile *pOutFile);

  UINT m_Version;

protected:
  rgba_color_t GradientColorsHalf (rgba_color_t Col1, rgba_color_t Col2);
  rgba_color_t GradientColors (rgba_color_t Col1, rgba_color_t Col2);
  void DecodeDXT1(int Height, int Width, byte* IData, unsigned int* PData);
  void DecodeDXT2And3 (int Height, int Width, byte* IData, unsigned int* PData);
  void DecodeDXT4And5 (int Height, int Width, byte* IData, unsigned int* PData);
  void DecodeBitmSurface (byte *data, int width, int height, int depth, int format, int flags, UINT* pOutBuf);
  int GetImageSize (int format, int width, int height);


  void DecodeLinearA8 (int width, int height, byte *texdata, unsigned int *outdata);
  void DecodeSwizzledA8 (int width, int height, int depth, byte *texdata, unsigned int *outdata);
  void DecodeLinearY8 (int width, int height, byte *texdata, unsigned int *outdata);
  void DecodeSwizzledY8 (int width, int height, int depth, byte *texdata, unsigned int *outdata);
  void DecodeLinearAY8 (int width, int height, byte *texdata, unsigned int *outdata);
  void DecodeSwizzledAY8 (int width, int height, int depth, byte *texdata, unsigned int *outdata);
  void DecodeLinearA8Y8 (int width, int height, byte *texdata, unsigned int *outdata);
  void DecodeSwizzledA8Y8 (int width, int height, int depth, byte *texdata, unsigned int *outdata);
  void DecodeLinearR5G6B5 (int width, int height, byte *texdata, unsigned int *outdata);
  void DecodeSwizzledR5G6B5 (int width, int height, int depth, byte *texdata, unsigned int *outdata);
  void DecodeLinearA1R5G5B5 (int width, int height, byte *texdata, unsigned int *outdata);
  void DecodeSwizzledA1R5G5B5 (int width, int height, int depth, byte *texdata, unsigned int *outdata);
  void DecodeLinearA4R4G4B4 (int width, int height, byte *texdata, unsigned int *outdata);
  void DecodeSwizzledA4R4G4B4 (int width, int height, int depth, byte *texdata, unsigned int *outdata);
  void DecodeLinearX8R8G8B8 (int width, int height, byte *texdata, unsigned int *outdata);
  void DecodeSwizzledX8R8G8B8 (int width, int height, int depth, byte *texdata, unsigned int *outdata);
  void DecodeLinearA8R8G8B8 (int width, int height, byte *texdata, unsigned int *outdata);
  void DecodeSwizzledA8R8G8B8 (int width, int height, int depth, byte *texdata, unsigned int *outdata);
  void DecodeLinearP8 (int width, int height, byte *texdata, unsigned int *outdata);
  void DecodeSwizzledP8 (int width, int height, int depth, byte *texdata, unsigned int *outdata);
};

#endif // !defined(AFX_HALOBMP_H__CE1D3B80_72DD_4649_8845_C1DCFBF3A291__INCLUDED_)
