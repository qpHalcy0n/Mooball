//////////////////////////////////////////////////////////////////////////////////
//
// QUICKLIGHTS.H
//
// Adapted for Mooball project 12/09
//
// This header contains the manager class for the light prepass
// render technique.
//
// This file is extensible and anything pertinent to real time lighting can
// be placed in this header.
//
// see: http://googlecode.com/p/mooball
//
///////////////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <stdlib.h>
#include <list>
#include "extensions.h"
#include "quickmath.h"
#include "quickshader.h"
#include "quickerror.h"
#include "quickcam.h"

#ifndef __QUICKLIGHTS_H_
#define __QUICKLIGHTS_H_


#define MAX_LIGHTS 2056


#define POINT_LIGHT			0x01
#define SPOT_LIGHT			0x02
#define DIRECTIONAL_LIGHT	0x03
#define UNDEFINED_LIGHT		0x04




/*
	CLightObject

	Interface type for all light objects.
	This class is overridden by CPointLight and CSpotLight currently.
*/
class CLightObject
{
	public:

		CLightObject();
		virtual ~CLightObject();


		/*
			Clone

			Hands back a newed object of the light type.
			This object is newed and must be "delete"d after use or a memory leak will occur
		*/
		virtual CLightObject*		Clone() = 0;

		/*
			GetPosition/SetPosition

			Obtain/Set the light's position in world space

			"pos" (SetPosition)- output parameter for position
		*/
		const inline void	GetPosition(vec3 pos) { vec3_copy(pos, m_position); }
		const inline void	SetPosition(vec3 pos) { vec3_copy(m_position, pos); }

		/*
			GetColor/SetColor

			Obtain/Set the light's emissive color as a normalized vector

			"color" (GetColor)- output parameter for color
		*/
		const inline void	GetColor(vec3 color) const { vec3_copy(color, m_color); }
		const inline void	SetColor(vec3 color) { vec3_copy(m_color, color); }



		/*
			GetType

			Obtain the light's type as one of the flags above
		*/
		const inline unsigned char	GetType() { return m_type; }

		/*
			GetXAtten/SetXAtten

			One of GetLinearAttenuation, GetConstantAttenuation, and GetQuadraticAttenuation
			Obtains or sets the light's current attenuation factors
		*/
		const inline float		GetConstantAttenuation() { return m_constantAtten; }
		const inline float		GetLinearAttenuation() { return m_linearAtten; }
		const inline float		GetQuadraticAttenuation() { return m_quadraticAtten; }

		const inline void		SetConstantAttenuation(float ca) { m_constantAtten = ca; }
		const inline void		SetLinearAttenuation(float la) { m_linearAtten = la; }
		const inline void		SetQuadraticAttenuation(float qa) { m_quadraticAtten = qa; }


		/*
			IsTransformed

			Is the light already in view space?
		*/
		const inline bool		IsTransformed() { return m_bIsTransformed; }
		const inline void		SetTransformStatus(bool isTransformed) { m_bIsTransformed = isTransformed; }


	protected:

		vec3		m_position;			// Light position in worldspace
		vec3		m_color;			// Light's color (normalized)

		float		m_constantAtten;	// Constant attenuation factor
		float		m_linearAtten;		// Linear attenuation factor
		float		m_quadraticAtten;   // Quadratic attenuation factor

		unsigned char	m_type;			// Light type

		bool		m_bIsTransformed;	// Is this light already in view space?

	private:
};



/*
	CPointLight

	Point light type (omnidirectional light). Child of CLightObject
*/
class CPointLight : public CLightObject
{
	public:

		CPointLight();
		CPointLight(CPointLight& rhs);
		CPointLight(CPointLight* rhs);
		~CPointLight();

		/*
			Clone

			Hands back a newed object of the light type.
			This object is newed and must be "delete"d after use or a memory leak will occur
		*/
		virtual CPointLight*		Clone();

		/*
			GetRadius/SetRadius

			Obtain/Set the light's radius in world units

			"rad" (SetRadius) - new light radius
		*/
		const inline float		GetRadius() { return m_radius; }
		const inline void		SetRadius(float rad) { m_radius = rad; }

	private:

		float			m_radius;			// Light's radius in world units
};



/*
	CSpotLight

	Spotlight (cone) light type. Child of CLightObject
*/
class CSpotLight : public CLightObject
{
	public:

		CSpotLight();
		CSpotLight(CSpotLight& rhs);
		CSpotLight(CSpotLight* rhs);
		~CSpotLight();


		/*
			Clone

			Hands back a newed object of the light type.
			This object is newed and must be "delete"d after use or a memory leak will occur
		*/
		virtual CSpotLight*			Clone();

		/*
			GetDirection/SetDirection

			Obtain the light's direction (normalized vector)

			"dir" - output/input parameter for direction
		*/
		const inline void		GetDirection(vec3 dir) { vec3_copy(dir, m_dir); }
		const inline void		SetDirection(vec3 dir) { vec3_copy(m_dir, dir); }

		/*
			GetFOV/SetFOV

			Obtain/Set the light's field of view in degrees

			"fov" (SetFOV) - Input paramter for new field of view
		*/
		const inline float		GetFOV() { return m_FOV; }
		const inline void		SetFOV(float fov) { m_FOV = fov; }

	private:

		vec3			m_dir;		// Light cone direction
		float			m_FOV;		// Field of view (degrees)
};



/*
	CDirectionalLight

	Universal directional light (like the sun).
*/
class CDirectionalLight : public CLightObject
{
	public:

		CDirectionalLight();
		CDirectionalLight( const CDirectionalLight& l );
		~CDirectionalLight();

		virtual CDirectionalLight*			Clone();

		const inline void		GetDirection(vec3 dir) { vec3_copy(dir, m_dir); }
		const inline void		SetDirection(vec3 dir) { vec3_copy(m_dir, dir); }

	private:

		vec3			m_dir;

};



/*
	Deferred light vertex type
*/
typedef struct
{
	vec3		wPosition;			// Bound to Vertex    (vert pos: xyz)
	vec4		colorDist;			// Bound to Color     (color: xyz, radius: w)
	vec3		lightPos;			// Bound to Normal    (view space light pos: xyz)
	vec4		dirFOV;				// Bound to texcoord  (dir: xyz, fov: w)
	char		buf[8];				// Align to 32 bit boundary.
} s_light_vertex;


class CLightManager
{
	public:

		CLightManager();
		~CLightManager();


		/*
			AddLight

			Adds a CLightObject to the light manager. Any object of type: CLightObject
			is acceptable.

			"newLight" - ptr to light object to add (this object is cloned)
		*/
		void		AddLight(CLightObject* newLight);


		/*
			SortLights

			Sorts lights based on visibility and depth from the camera, as such
			this function accepts worldspace camera position. This function should
			be called everyframe prior to render

			"camPos"- world space camera position
		*/
		bool		SortLights(const CCameraObject& cam);

		/*
			RenderLights

			Renders the lights to whichever buffer is current
		*/
		void		RenderLights(const int& ndc);


		/*
			DebugRenderLights

			Renders the lights wireframes
		*/
		void		DebugRenderLights();

		void 		ToggleFlashlight() { m_bFlashlight = !m_bFlashlight; }


	private:

		std::list <CLightObject*>		m_lights;			// Light list

		s_light_vertex*         insideLights;				// inside volume lights (exported)
		s_light_vertex*         outsideLights;				// outside volume lights (exported)


		int						m_nInsideLights;			// Number of inside volume lights per frame
		int						m_nOutsideLights;			// Number of outside volume lights per frame
		int						m_shaderHandle;
		int						m_globalShaderHandle;

		GLuint		m_insidevboID;					// Inside volume VBO handle
		GLuint		m_outsidevboID;					// Outside volume VBO handle
		GLuint		m_iboID;						// Global IBO handle
		GLuint		m_insidevaoID;
		GLuint		m_outsidevaoID;

		bool		m_bFirstCall;					// Is this the first iteration through sort?
		bool		m_bFlashlight;					// Flashlight on?

		vec3		m_curCamPos;
		vec3		m_curCamDir;
		float		m_curFarClipDist;
		float		m_curCamFOV;

		vec3		m_globalLightVec;
		vec3		m_globalLightColor;
		int			m_nDirectionalLights;
};



extern CLightManager*	g_lightManager;



#endif
