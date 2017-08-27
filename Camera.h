#ifndef _CAMERA_H
#define _CAMERA_H

// This was created to allow you to use this camera code in your projects
// without having to cut and paste code.  This file and camera.cpp should be
// added to your project.
struct CVector3
{
public:
	CVector3() {}
	
  CVector3(float X, float Y, float Z) 
	{ 
		x = X; y = Y; z = Z;
	}
	
  CVector3 operator+(CVector3 vVector)
	{
		return CVector3(vVector.x + x, vVector.y + y, vVector.z + z);
	}

  CVector3 operator-(CVector3 vVector)
	{
		return CVector3(x - vVector.x, y - vVector.y, z - vVector.z);
	}
	
	CVector3 operator*(float num)
	{
		// Return the scaled vector
		return CVector3(x * num, y * num, z * num);
	}

	CVector3 operator/(float num)
	{
		// Return the scale vector
		return CVector3(x / num, y / num, z / num);
	}

	float x, y, z;						
};

// This is our camera class
class CCamera
{
public:
	void GetCameraFrontCoord(float *pCoord);
	void RestoreSavedPosition(void);
	void SaveCurrentPosition(void);
  void DoUnProject(float ifX, float ifY, float *ofObjX, float *ofObjY, float *ofObjZ, float ifZ, bool ibUseZ);
	void GetCameraAltAzimuth(float *pAlt, float *pAzimuth, float *pCamX, float *pCamY);
	void GetCameraPosition(float *pPosition);
	void LevitateCamera(float speed);
	void SetCamSpeed(float speed);
	CCamera();	
	
  // These are are data access functions for our camera's private data
	CVector3 Position() {	return m_vPosition;		}
	CVector3 View()		{	return m_vView;			}
	CVector3 UpVector() {	return m_vUpVector;		}
	CVector3 Strafe()	{	return m_vStrafe;		}

  // This changes the position, view, and up vector of the camera.
	// This is primarily used for initialization
	void PositionCamera(float positionX, float positionY, float positionZ,
			 		    float viewX,     float viewY,     float viewZ,
						float upVectorX, float upVectorY, float upVectorZ);

	// This rotates the camera's view around the position depending on the values passed in.
	void RotateView(float angle, float X, float Y, float Z);

  void UpdateMouseMove(int DeltaX, int DeltaY);
	void CheckMouse(); 

	// This rotates the camera around a point (I.E. your character).
	void RotateAroundPoint(CVector3 vCenter, float X, float Y, float Z);

	// This strafes the camera left or right depending on the speed (+/-) 
	void StrafeCamera(float speed);

	// This will move the camera forward or backward depending on the speed
	void MoveCamera(float speed);

	void CheckKeyboard();

	// This updates the camera's view and other data (Should be called each frame)
	void Update();

	// This uses gluLookAt() to tell OpenGL where to look
	void Look();

private:
	CVector3 m_vPosition;					
	CVector3 m_vView;						
	CVector3 m_vUpVector;		
	CVector3 m_vStrafe;
 
	CVector3 m_vSavePosition;					
	CVector3 m_vSaveView;						
	CVector3 m_vSaveUpVector;		

  int m_LastMouseX;
  int m_LastMouseY;
  float m_Speed;
  float m_CurrentRotX;
};


#endif


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// This file contains the prototype for our camera class.  
//
// We add a few more functions to our CCamera class, along with
// some measly data access functions for our now private member variables.
//
// 
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
//