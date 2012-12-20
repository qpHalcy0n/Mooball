////////////////////////////////////////////////////////////////////////////////////
//
// QUICKCAM.H
//
// Quick stub for that which will encapsulate the camera object and functionality.
// Any functionality here was adapted for the Mooball project 12/09
//
// see: http://googlecode.com/p/mooball
//
////////////////////////////////////////////////////////////////////////////////////



#include "extensions.h"
#include "quickmath.h"


#ifndef __QUICKCAM_H_
#define __QUICKCAM_H_







/*
    CCameraObject

    Camera object which maintains the view state.
*/

class CCameraObject
{
    public:

        CCameraObject();
        ~CCameraObject();


        /*
            SetCamera

            Used for initial setting of the camera state.
            Sets camera position, look at position, and up vector

            "camPos" - position of camera in world space
            "lookAt" - position of the look at point in world space
            "up" - worldspace up vector (relative)
        */
        void        SetCamera(vec3 camPos, vec3 lookAt, vec3 up);

        /*
            SetCamera

            Acts the same as above, except does not take view parameters.
            This migrates the camera's coordinate frame one for one

            "camPos" - position of camera in world space
        */
        void        SetCamera(vec3 camPos);


        /*
            SetPerspective

            Sets camera parameters for generating perspective projection

            "fovy" - field of view (vertical) in degrees
            "aspect" - aspect ratio of the frustum
            "nearClip" - near clip plane distance from camera
            "farClip" - far clip plane distance from camera
        */
        void        SetPerspective(float fovy, float aspect, float nearClip, float farClip);


        /*
            RotateByMouse

            Rotates the camera view coordinate frame by mouse input.
            Accepts current mouse position and a mid-screen position

            "mouseX" - mouse X position (relative to window)
            "mouseY" - mouse Y position (relative to window)
            "midScreenX" - middle of the screen in X
            "midScreenY" - middle of the screen in Y
        */
        void        RotateByMouse(int mouseX, int mouseY, int midScreenX, int midScreenY);


        /*
            MoveX

            The MoveForward/MoveBack functions move forwards and backwards along the view vector
            The MoveRight/MoveLeft functions move left and right along the strafe vector

            "fMul" - multiplier for sensitivity (default is 0.1F)
        */
        void        MoveForward(float fMul = 0.1F);
        void        MoveBack(float fMul = 0.1F);
        void        MoveRight(float fMul = 0.1F);
        void        MoveLeft(float fMul = 0.1F);


        /*
            Update

            Applies any changes and sets the modelview and projection
            matrices according to this camera.

            Wipes the old modelview/projection matrices

            MUST BE CALLED PRIOR TO RENDER for camera to be applied
        */
        void        Update();


        /*
            GetPosition

            Obtain the camera position
        */
        const inline void       GetPosition(vec3 pos) const { vec3_copy(pos, m_curPosition); }


		/*
			GetViewVector

			Obtain the camera's current view vector
		*/
		const inline void		GetViewVector( vec3 pos ) const { vec3_copy( pos, m_curViewVec ); }

        /*
            GetFarClip/GetNearClip

            Get far clip distance/Get near clip distance
        */
        const inline float      GetFarClip() const { return m_zFar; }
        const inline float      GetNearClip() const { return m_zNear; }

        /*
            GetFOV

            Get current cam field of view (degrees)
        */
        const inline float      GetFOV() const { return m_curFOV; }


    private:

        vec3        m_curPosition;      // Current camera worldspace position
        vec3        m_curLookAt;        // Current camera look at position
        vec3        m_curUpVec;         // Current worldspace up vector (relative)

        vec3        m_curViewVec;       // Current camera view vector
        vec3        m_curStrafeVec;     // Current camera strafe vector (Y cross Z)

        float       m_curFOV;           // Current camera field of view (degrees)
        float       m_curAspect;        // Current camera frustum aspect ratio
        float       m_zNear;            // Near clip distance
        float       m_zFar;             // Far clip distance

};



#endif
