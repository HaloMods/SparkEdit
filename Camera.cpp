//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen		DigiBen@GameTutorials.com			 //
//																		 //
//		$Program:		Camera5	(strafing)								 //
//																		 //
//		$Description:	Demonstrates camera strafing right and left		 //
//																		 //
//		$Date:			1/1/02											 //
//																		 //
//***********************************************************************//


#include "stdafx.h"
#include "math.h"
#include "Camera.h"


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

// This is how fast our camera moves (Sped up a bit due to normalizing our vectors)
#define kSpeed	0.1f									

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


/////////////////////////////////////// CROSS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns a perpendicular vector from 2 given vectors by taking the cross product.
/////
/////////////////////////////////////// CROSS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
												
CVector3 Cross(CVector3 vVector1, CVector3 vVector2)
{
	CVector3 vNormal;	

	// Calculate the cross product with the non communitive equation
	vNormal.x = ((vVector1.y * vVector2.z) - (vVector1.z * vVector2.y));
	vNormal.y = ((vVector1.z * vVector2.x) - (vVector1.x * vVector2.z));
	vNormal.z = ((vVector1.x * vVector2.y) - (vVector1.y * vVector2.x));

	// Return the cross product
	return vNormal;										 
}


/////////////////////////////////////// MAGNITUDE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns the magnitude of a vector
/////
/////////////////////////////////////// MAGNITUDE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

float Magnitude(CVector3 vNormal)
{
	// Here is the equation:  magnitude = sqrt(V.x^2 + V.y^2 + V.z^2) : Where V is the vector
	return (float)sqrt( (vNormal.x * vNormal.x) + 
						(vNormal.y * vNormal.y) + 
						(vNormal.z * vNormal.z) );
}


/////////////////////////////////////// NORMALIZE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This returns a normalize vector (A vector exactly of length 1)
/////
/////////////////////////////////////// NORMALIZE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CVector3 Normalize(CVector3 vVector)
{
	// Get the magnitude of our normal
	float magnitude = Magnitude(vVector);				

	// Now that we have the magnitude, we can divide our vector by that magnitude.
	// That will make our vector a total length of 1.  
	vVector = vVector / magnitude;		
	
	// Finally, return our normalized vector
	return vVector;										
}


///////////////////////////////// CCAMERA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This is the class constructor
/////
///////////////////////////////// CCAMERA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CCamera::CCamera()
{
	CVector3 vZero = CVector3(0.0, 0.0, 0.0);		// Init a vVector to 0 0 0 for our position
	CVector3 vView = CVector3(0.0, 1.0, 0.5);		// Init a starting view vVector (looking up and out the screen) 
	CVector3 vUp   = CVector3(1.0, 0.0, 0.0);		// Init a standard up vVector (Rarely ever changes)

	m_vPosition	= vZero;					// Init the position to zero
	m_vView		= vView;					// Init the view to a std starting view
	m_vUpVector	= vUp;						// Init the UpVector
  m_LastMouseX = -1;
  m_LastMouseY = -1;
  m_Speed = 0.1f;
}


///////////////////////////////// POSITION CAMERA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function sets the camera's position and view and up vVector.
/////
///////////////////////////////// POSITION CAMERA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CCamera::PositionCamera(float positionX, float positionY, float positionZ,
				  		     float viewX,     float viewY,     float viewZ,
							 float upVectorX, float upVectorY, float upVectorZ)
{
	CVector3 vPosition	= CVector3(positionX, positionY, positionZ);
	CVector3 vView		= CVector3(viewX, viewY, viewZ);
	CVector3 vUpVector	= CVector3(upVectorX, upVectorY, upVectorZ);

	// The code above just makes it cleaner to set the variables.
	// Otherwise we would have to set each variable x y and z.

	m_vPosition = vPosition;					// Assign the position
	m_vView     = vView;						// Assign the view
	m_vUpVector = vUpVector;					// Assign the up vector

  //TRACE("View = (%.2f %.2f %.2f)\n", m_vView.x, m_vView.y, m_vView.z);
  m_CurrentRotX = 0;
}


///////////////////////////////// SET VIEW BY MOUSE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This allows us to look around using the mouse, like in most first person games.
/////
///////////////////////////////// SET VIEW BY MOUSE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CCamera::CheckMouse()
{
	POINT mousePos;									// This is a window structure that holds an X and Y
	int middleX = 640  >> 1;				// This is a binary shift to get half the width
	int middleY = 480 >> 1;				// This is a binary shift to get half the height
//	int middleX = SCREEN_WIDTH  >> 1;				// This is a binary shift to get half the width
//	int middleY = SCREEN_HEIGHT >> 1;				// This is a binary shift to get half the height
	float angleY = 0.0f;							// This is the direction for looking up or down
	float angleZ = 0.0f;							// This will be the value we need to rotate around the Y axis (Left and Right)
	static float currentRotX = 0.0f;

  

	// Get the mouse's current X,Y position
	GetCursorPos(&mousePos);						
	
	// If our cursor is still in the middle, we never moved... so don't update the screen
	if( (mousePos.x == middleX) && (mousePos.y == middleY) ) return;

	// Set the mouse position to the middle of our window
	SetCursorPos(middleX, middleY);							

	// Get the direction the mouse moved in, but bring the number down to a reasonable amount
	angleY = (float)( (middleX - mousePos.x) ) / 500.0f;		
	angleZ = (float)( (middleY - mousePos.y) ) / 500.0f;		

	// Here we keep track of the current rotation (for up and down) so that
	// we can restrict the camera from doing a full 360 loop.
	currentRotX -= angleZ;  

	// If the current rotation (in radians) is greater than 1.0, we want to cap it.
	if(currentRotX > 0.3f)
		currentRotX = 0.3f;
	// Check if the rotation is below -1.0, if so we want to make sure it doesn't continue
	else if(currentRotX < -0.3f)
		currentRotX = -0.3f;
	// Otherwise, we can rotate the view around our position
	else
	{
		// To find the axis we need to rotate around for up and down
		// movements, we need to get a perpendicular vector from the
		// camera's view vector and up vector.  This will be the axis.
		CVector3 vAxis = Cross(m_vView - m_vPosition, m_vUpVector);
		vAxis = Normalize(vAxis);

		// Rotate around our perpendicular axis and along the y-axis
		RotateView(angleZ, vAxis.x, vAxis.y, vAxis.z);
		RotateView(angleY, 0, 1, 0);
	}
}

void CCamera::UpdateMouseMove(int DeltaX, int DeltaY)
{
	float angleY = 0.0f;							// This is the direction for looking up or down
	float angleZ = 0.0f;							// This will be the value we need to rotate around the Y axis (Left and Right)
//	static float currentRotX = 0.0f;

  if((DeltaX == 0)&&(DeltaY == 0))
    return;

//  TRACE("DX = %d  DY = %d\n", DeltaX, DeltaY);


	// Get the direction the mouse moved in, but bring the number down to a reasonable amount
	angleY = (float)(DeltaX) / -200.0f;		
	angleZ = (float)(DeltaY) / -200.0f;		

	// Here we keep track of the current rotation (for up and down) so that
	// we can restrict the camera from doing a full 360 loop.
	m_CurrentRotX -= angleZ;  

	// If the current rotation (in radians) is greater than 1.0, we want to cap it.
	if(m_CurrentRotX > 0.5f)
		m_CurrentRotX = 0.5f;
	// Check if the rotation is below -1.0, if so we want to make sure it doesn't continue
	else if(m_CurrentRotX < -1.5f)
		m_CurrentRotX = -1.5f;
	else
	{
		// To find the axis we need to rotate around for up and down
		// movements, we need to get a perpendicular vector from the
		// camera's view vector and up vector.  This will be the axis.
		CVector3 vAxis = Cross(m_vView - m_vPosition, m_vUpVector);
		vAxis = Normalize(vAxis);

		// Rotate around our perpendicular axis and along the y-axis
		//RotateView(angleZ, vAxis.x, vAxis.y, vAxis.z);
		//RotateView(angleY, 0, 1, 0);
		RotateView(angleZ, vAxis.x, vAxis.y, vAxis.z);
		RotateView(angleY, 0, 0, 1);
	}

  //TRACE("ROTX = %.3f\n", m_CurrentRotX);

}

///////////////////////////////// ROTATE VIEW \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This rotates the view around the position using an axis-angle rotation
/////
///////////////////////////////// ROTATE VIEW \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CCamera::RotateView(float angle, float x, float y, float z)
{
	CVector3 vNewView;

	// Get the view vector (The direction we are facing)
	CVector3 vView = m_vView - m_vPosition;		

	// Calculate the sine and cosine of the angle once
	float cosTheta = (float)cos(angle);
	float sinTheta = (float)sin(angle);

	// Find the new x position for the new rotated point
	vNewView.x  = (cosTheta + (1 - cosTheta) * x * x)		* vView.x;
	vNewView.x += ((1 - cosTheta) * x * y - z * sinTheta)	* vView.y;
	vNewView.x += ((1 - cosTheta) * x * z + y * sinTheta)	* vView.z;

	// Find the new y position for the new rotated point
	vNewView.y  = ((1 - cosTheta) * x * y + z * sinTheta)	* vView.x;
	vNewView.y += (cosTheta + (1 - cosTheta) * y * y)		* vView.y;
	vNewView.y += ((1 - cosTheta) * y * z - x * sinTheta)	* vView.z;

	// Find the new z position for the new rotated point
	vNewView.z  = ((1 - cosTheta) * x * z - y * sinTheta)	* vView.x;
	vNewView.z += ((1 - cosTheta) * y * z + x * sinTheta)	* vView.y;
	vNewView.z += (cosTheta + (1 - cosTheta) * z * z)		* vView.z;

	// Now we just add the newly rotated vector to our position to set
	// our new rotated view of our camera.
	m_vView = m_vPosition + vNewView;
}


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

///////////////////////////////// STRAFE CAMERA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This strafes the camera left and right depending on the speed (-/+)
/////
///////////////////////////////// STRAFE CAMERA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CCamera::StrafeCamera(float speed)
{
	// Strafing is quite simple if you understand what the cross product is.
	// If you have 2 vectors (say the up vVector and the view vVector) you can
	// use the cross product formula to get a vVector that is 90 degrees from the 2 vectors.
	// For a better explanation on how this works, check out the OpenGL "Normals" tutorial at our site.
	// In our new Update() function, we set the strafing vector (m_vStrafe).  Due
	// to the fact that we need this vector for many things including the strafing
	// movement and camera rotation (up and down), we just calculate it once.
	//
	// Like our MoveCamera() function, we add the strafing vector to our current position 
	// and view.  It's as simple as that.  It has already been calculated in Update().
	
	// Add the strafe vector to our position
	//m_vPosition.x += m_vStrafe.x * speed;
	//m_vPosition.z += m_vStrafe.z * speed;

	// Add the strafe vector to our view
	//m_vView.x += m_vStrafe.x * speed;
	//m_vView.z += m_vStrafe.z * speed;

	m_vPosition.x += m_vStrafe.x * speed;
	m_vPosition.y += m_vStrafe.y * speed;
	m_vView.x += m_vStrafe.x * speed;
	m_vView.y += m_vStrafe.y * speed;
}

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


///////////////////////////////// MOVE CAMERA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This will move the camera forward or backward depending on the speed
/////
///////////////////////////////// MOVE CAMERA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CCamera::MoveCamera(float speed)
{
	// Get the current view vector (the direction we are looking)
	CVector3 vVector = m_vView - m_vPosition;


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// I snuck this change in here!  We now normalize our view vector when
	// moving throughout the world.  This is a MUST that needs to be done.
	// That way you don't move faster than you strafe, since the strafe vector
	// is normalized too.
	vVector = Normalize(vVector);
	
/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


	//m_vPosition.x += vVector.x * speed;		// Add our acceleration to our position's X
	//m_vPosition.z += vVector.z * speed;		// Add our acceleration to our position's Z
	//m_vView.x += vVector.x * speed;			// Add our acceleration to our view's X
	//m_vView.z += vVector.z * speed;			// Add our acceleration to our view's Z
	
  m_vPosition.x += vVector.x * speed;		// Add our acceleration to our position's X
	m_vPosition.y += vVector.y * speed;		// Add our acceleration to our position's Z
	m_vView.x += vVector.x * speed;			// Add our acceleration to our view's X
	m_vView.y += vVector.y * speed;			// Add our acceleration to our view's Z
}


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

// The next 3 functions were added to our camera class.  The less code in 
// Main.cpp the better.

//////////////////////////// CHECK FOR MOVEMENT \\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function handles the input faster than in the WinProc()
/////
//////////////////////////// CHECK FOR MOVEMENT \\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CCamera::CheckKeyboard()
{
	// Check if we hit the Up arrow or the 'w' key
	if(GetKeyState(VK_UP) & 0x80 || GetKeyState('W') & 0x80) {				

		// Move our camera forward by a positive SPEED
		MoveCamera(m_Speed);				
	}

	// Check if we hit the Down arrow or the 's' key
	if(GetKeyState(VK_DOWN) & 0x80 || GetKeyState('S') & 0x80) {			

		// Move our camera backward by a negative SPEED
		MoveCamera(-m_Speed);				
	}

	// Check if we hit the Left arrow or the 'a' key
	if(GetKeyState(VK_LEFT) & 0x80 || GetKeyState('A') & 0x80) {			

		// Strafe the camera left
		StrafeCamera(-m_Speed);
	}

	// Check if we hit the Right arrow or the 'd' key
	if(GetKeyState(VK_RIGHT) & 0x80 || GetKeyState('D') & 0x80) {			

		// Strafe the camera right
		StrafeCamera(m_Speed);
	}	
}


///////////////////////////////// UPDATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This updates the camera's view and strafe vector
/////
///////////////////////////////// UPDATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CCamera::Update() 
{
	// Below we calculate the strafe vector every time we update
	// the camera.  This is because many functions use it so we might
	// as well calculate it only once.  

	// Initialize a variable for the cross product result
	CVector3 vCross = Cross(m_vView - m_vPosition, m_vUpVector);

	// Normalize the strafe vector
	m_vStrafe = Normalize(vCross);

  //CheckKeyboard();
  //CheckMouse();
}


///////////////////////////////// LOOK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This updates the camera according to the 
/////
///////////////////////////////// LOOK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CCamera::Look()
{
	// Give openGL our camera position, then camera view, then camera up vector
	gluLookAt(m_vPosition.x, m_vPosition.y, m_vPosition.z,	
			  m_vView.x,	 m_vView.y,     m_vView.z,	
			  m_vUpVector.x, m_vUpVector.y, m_vUpVector.z);
	//gluLookAt(100, 100, 0,	
	//		      0, 0, 0, 
	//		  0, 0, 0);
}

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// This file should now be close to complete.  Now that we have all the basic camera 
// functionality you can use this file in your projects and not have to cut and paste code.
// I created a camera.h for this purpose as well.
//
// We added strafing to this file.  3 other functions were added to our camera class
// so that it would clean up main.cpp a bit more.  In Camera.h you will find some
// new changes as well, which include the move of our camera data to private.  We now
// will use data access functions to get camera data.
//
// Here are the notes from main.cpp about the concept and application:
//
// This tutorial was taken from the Camera4 tutorial.  This is our last camera tutorial.
// We might create a bezier curve camera tutorial though in the future.  This tutorial
// shows how to strafe the camera right or left.  This might seem easy when you think to
// attempt it, but if you don't know your linear algebra it can be tricky.  Strafing the
// camera is moving the camera 90 degrees left or right from the current view.  In other
// words, it's as if you were side stepping while you look forward.  This is used in most
// first person shooters games, and comes in handy when peering around corners or running
// past a hallway while firing off some rounds.  It's also a great way to move diagonal
// while doing a shootout at close ranges to avoid being hit.
//
// Since we understand what strafing is and what it does, let me actually explain how it works.
// We know that we want to walk in the direction that is 90 degrees from the view vVector (with
// the view vVector being m_vView - m_vPosition).  So how do we then get the vVector that is 90
// degrees from our view vVector?  If you know what the cross product is, you can easily see how
// this would be done.  The cross product is a mathematical formula that takes 2 vectors and
// returns the vVector 90 degrees from those 2 vectors.  This is how you find the normal to a plane.
// Well, we have a view vVector, but what would the other vVector be?  Does the up vVector come to mind?
// That's it!  We want to take the cross product between the up vVector and the view vVector.  This
// will return the vVector (or direction) that we want to strafe in.  In games like Descent, the
// up vVector will change because you can go upside down and spin in crazy directions.  The cross
// product will ensure that we will always strafe correctly no matter what orientation the camera is in.
// Once we have the strafe vVector, we need to add it to the position and view points.
// Here are the controls for this tutorial:
//
// w, s, UP_ARROW, DOWN_ARROW - Move the camera forward and backward
// a, d, RIGHT_ARROW, LEFT_ARROW - Strafe the camera left and right
// Mouse movement - Moves the view for first person mode
// ESCAPE - Quits the program
//
// You may notice that we don't use all the methods in the camera class.  I decided to leave
// them in there because since this is the last camera tutorial it would be nice to just use
// this in your projects without having to cut and paste code.  That's also why I finally
// added a camera.h so you don't have the camera class in main.h.
// 
// Enjoy!
// 
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
//

void CCamera::SetCamSpeed(float speed)
{
  m_Speed = speed;
}

void CCamera::LevitateCamera(float speed)
{
	// Get the current view vector (the direction we are looking)
	//CVector3 vVector = m_vView - m_vPosition;


	//vVector = Normalize(vVector);
	
	//m_vPosition.y += speed;
	//m_vView.y += speed;
	m_vPosition.z += speed;
	m_vView.z += speed;
}

void CCamera::GetCameraPosition(float *pPosition)
{
	pPosition[0] = m_vPosition.x;
	pPosition[1] = m_vPosition.y;
	pPosition[2] = m_vPosition.z;
}

void CCamera::GetCameraAltAzimuth(float *pAlt, float *pAzimuth, float *pCamX, float *pCamY)
{
  float xy_mag;
  float dx,dy,dz;

  dx = m_vPosition.x - m_vView.x;
  dy = m_vView.y - m_vPosition.y;
  dz = m_vPosition.z - m_vView.z;

  xy_mag = sqrt(dx*dx + dy*dy);

  *pCamX = dx/xy_mag;
  *pCamY = dy/xy_mag;

  TRACE("camx=%.1f camy = %.1f\n", *pCamX, *pCamY);
  if(dx != 0)
  {
    *pAzimuth = atan(dy/dx);

    if(dx>0)
    {
      if(dy>0)
        *pAzimuth += 0;
      else
      {
        *pAzimuth *= -1;
        *pAzimuth += 0.78539816f*2;
      }
    }
    else
    {
      if(dy>0)
      {
//        *pAzimuth *= -1;
//        *pAzimuth += 0.78539816f*2;
      }
      //se
      //*pAzimuth += 0.78539816f*2;
    }
  }
  else
  {
  }

  //TRACE("dx = %.2f  dy=%.2f az=%f.2\n", dx, dy, *pAzimuth);

  if(xy_mag != 0) //this should never happen because of camera restrictions
  {
    *pAlt = atan(dz/xy_mag);
  }
  else
  {
    *pAlt = 0;
  }
}

void CCamera::DoUnProject(float ifX, float ifY, float *ofObjX, float *ofObjY, float *ofObjZ, float ifZ, bool ibUseZ)
{

	//this function is basically just a wrapper for gluUnProject.
	//It does all of the getting of the projection and modelview
	//matrices, and if you pass true in the parameter ibUseZ,
	//it will use the ifZ parameter you passed in, otherwise it
	//will use glReadPixels to calculate it.
	//
	//ifX and ifY are expected to be relative to the upper-left
	//corner of the OpenGL context.  The calls to ReadPixels
	//and UnProject expect coordinates relative to the *lower*
	//left corner, so we will convert them here.

	float fMouseX, fMouseY, fMouseZ;
	fMouseX = ifX;
	fMouseY = ifY;
	fMouseZ = 0.0f;

	//now, fMouseX and fMouseY are relative to the *upper* left
	//corner of the OpenGL context, but OpenGL expects 
	//coordinates relative to the *lower* left corner of the 
	//screen, so we need to reverse Y.
	int anViewport[4];
	glGetIntegerv(GL_VIEWPORT, anViewport);
	//height is the fourth element
	fMouseY = (float)anViewport[3] - fMouseY;
	if (ibUseZ)
	{
		fMouseZ = ifZ;
	}
	else
	{
		glReadPixels(fMouseX, fMouseY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &fMouseZ);
	}
	//get the matrices for their passing to gluUnProject
	double afModelviewMatrix[16];
	double afProjectionMatrix[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, afModelviewMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, afProjectionMatrix);

	double dTempX, dTempY, dTempZ;
	gluUnProject(fMouseX, fMouseY, fMouseZ, afModelviewMatrix, afProjectionMatrix, anViewport, &dTempX, &dTempY, &dTempZ);
	//ofObjX, Y and Z should be populated and returned now
	*ofObjX = (float)dTempX;
	*ofObjY = (float)dTempY;
	*ofObjZ = (float)dTempZ;

}


void CCamera::SaveCurrentPosition()
{
	m_vSavePosition = m_vPosition;
	m_vSaveView = m_vView;
	m_vSaveUpVector = m_vUpVector;
}

void CCamera::RestoreSavedPosition()
{
	m_vPosition = m_vSavePosition;
	m_vView = m_vSaveView;
	m_vUpVector = m_vSaveUpVector;
}

void CCamera::GetCameraFrontCoord(float *pCoord)
{
  float dx, dy, dz, mag;

  dx = m_vView.x - m_vPosition.x;
  dy = m_vView.y - m_vPosition.y;
  dz = m_vView.z - m_vPosition.z;
  mag = sqrt(dx*dx + dy*dy + dz*dz);


  pCoord[0] = m_vPosition.x + ((m_vView.x - m_vPosition.x)/mag)*2;
  pCoord[1] = m_vPosition.y + ((m_vView.y - m_vPosition.y)/mag)*2;
  pCoord[2] = m_vPosition.z + ((m_vView.z - m_vPosition.z)/mag)*2;
}
