// Frustum1.h: interface for the CFrustum class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FRUSTUM1_H__333896D5_535B_4EF4_AD04_69167E4BBDB1__INCLUDED_)
#define AFX_FRUSTUM1_H__333896D5_535B_4EF4_AD04_69167E4BBDB1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Util.h"



class CFrustum : public CUtil  
{
enum PlaneData
{
	A = 0,				// The X value of the plane's normal
	B = 1,				// The Y value of the plane's normal
	C = 2,				// The Z value of the plane's normal
	D = 3				// The distance the plane is from the origin
};

enum FrustumSide
{
	RIGHT	= 0,	
	LEFT	= 1,	
	BOTTOM	= 2,
	TOP		= 3,	
	BACK	= 4,	
	FRONT	= 5		
}; 

public:
	BOOL BoxInFrustum(BOUNDING_BOX *pBox);
	CFrustum();
	virtual ~CFrustum();
  void NormalizePlane(float frustum[6][4], int side);
  void CalculateFrustum();
  BOOL PointInFrustum(float x, float y, float z);
  BOOL SphereInFrustum(float x, float y, float z, float radius);
  BOOL CubeInFrustum(float x, float y, float z, float size);

protected:
  float m_Frustum[6][4];
};

#endif // !defined(AFX_FRUSTUM1_H__333896D5_535B_4EF4_AD04_69167E4BBDB1__INCLUDED_)
