/*-----------------------------------------------------------------
LOG
    GEM - Graphics Environment for Multimedia

    camera control gem object
    tigital@mac.com

    For information on usage and redistribution, and for a DISCLAIMER OF ALL
    WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.

-----------------------------------------------------------------*/

#ifndef INCLUDE_CAMERA_H_
#define INCLUDE_CAMERA_H_

#include "Base/GemBase.h"
#include <stdlib.h>
#include <math.h>
#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif
#define PI	3.1415926535897932384626433832795

#ifndef _WINDOWS
using namespace std;
#endif

// This is our basic 3D point/vector class
class CVector3
{
public:
    
    // A default constructor
    CVector3() {}

    // This is our constructor that allows us to initialize our data upon creating an instance
    CVector3(float X, float Y, float Z) 
    { 
        x = X; y = Y; z = Z;
    }

    // Here we overload the + operator so we can add vectors together 
    CVector3 operator+(CVector3 vVector)
    {
        // Return the added vectors result.
        return CVector3(vVector.x + x, vVector.y + y, vVector.z + z);
    }

    // Here we overload the - operator so we can subtract vectors 
    CVector3 operator-(CVector3 vVector)
    {
        // Return the subtracted vectors result
        return CVector3(x - vVector.x, y - vVector.y, z - vVector.z);
    }
    
    // Here we overload the * operator so we can multiply by scalars
    CVector3 operator*(float num)
    {
        // Return the scaled vector
        return CVector3(x * num, y * num, z * num);
    }

    // Here we overload the / operator so we can divide by a scalar
    CVector3 operator/(float num)
    {
        // Return the scale vector
        return CVector3(x / num, y / num, z / num);
    }

    float x, y, z;                        
};

/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS
    camera
    
    camera gem object

DESCRIPTION
    

-----------------------------------------------------------------*/
class GEM_EXTERN camera : public GemBase
//class GEM_EXTERN camera : public CPPExtern
{
    CPPEXTERN_HEADER(camera, GemBase)
    //CPPEXTERN_HEADER(camera, CPPExtern)

    public:

        //////////
        // Constructor
    	camera(int argc, t_atom *argv);
	//CCamera	g_Camera;

    protected:
    	
    	//////////
    	// Destructor
    	virtual ~camera();

    	//////////
    	// When rendering occurs
    	virtual void	render(GemState *state);

    	//////////
    	// Angles changed
    	void	    	hAngleMess(float val);
    	void	    	vAngleMess(float val);
	void	    	distanceMess(float val);
    	
    	//////////
    	// roll value changed
    	void	    	rollMess(float val);

    	//////////
    	// value changed
    	void	    	forwardMess(float val);
	void	    	reverseMess(float val);
    	
    	//////////
    	// Pitch value changed
    	void	    	leftMess(bool val);
	void		rightMess(bool val);
    	void	    	slideLeftMess(bool val);
	void		slideRightMess(bool val);    	
    	//////////
    	// roll value changed
    	void	    	upMess(bool val);
	void		downMess( bool val );
	void		resetState();
	
	
	CVector3 m_vPosition;                   // The camera's position
	CVector3 m_vView;                       // The camera's View
	CVector3 m_vUpVector;                   // The camera's UpVector
	CVector3 m_vSlide;			// The camera's slide
	
	int	left, right, up, down, forward, reverse, m_mode;
	float	lookX, lookY, lookZ, m_speed;
	float 	hAngle, vAngle, distance;
	int	slideLeft, slideRight;
	
	// This changes the position, view, and up vector of the camera.
	// (Used for initialization)
	void PositionCamera(float positionX, float positionY, float positionZ,
			    float viewX,     float viewY,     float viewZ,
			    float upVectorX, float upVectorY, float upVectorZ);
			
	// This rotates the camera's view around the position using axis-angle rotation
	void RotateView(float angle, float x, float y, float z);

	// This will move the camera forward or backward depending on the speed
	void MoveCamera( float speed );
	void SlideCamera( float speed );
	void calcCameraVals( void );
	void calcUpVector( void );
	void incHRot( float val );
	void decHRot( float val );
	void incVRot( float val );
	void decVRot( float val );

    	
    private:
    	
    	//////////
    	// static member functions
    	static void 	hAngleMessCallback(void *data, t_floatarg val);
    	static void 	vAngleMessCallback(void *data, t_floatarg val);
    	static void 	distanceMessCallback(void *data, t_floatarg val);
    	static void 	speedMessCallback(void *data, t_floatarg val);
    	static void 	forwardMessCallback(void *data, t_floatarg val);
	static void	reverseMessCallback(void *data, t_floatarg val);
    	static void 	leftMessCallback(void *data, t_floatarg val);
	static void 	rightMessCallback(void *data, t_floatarg val);
    	static void 	slideLeftMessCallback(void *data, t_floatarg val);
	static void 	slideRightMessCallback(void *data, t_floatarg val);
    	static void 	upMessCallback(void *data, t_floatarg val);
	static void 	downMessCallback(void *data, t_floatarg val);
	static void 	resetMessCallback(void *);
	static void 	modeMessCallback(void *data, t_floatarg val);
	static void 	lookXMessCallback(void *data, t_floatarg val);
    	static void 	lookYMessCallback(void *data, t_floatarg val);
	static void 	lookZMessCallback(void *data, t_floatarg val);
};

#endif	// for header file
