#include "quicklights.h"


static unsigned short plist[24] =
{
	0, 3, 2, 1, 3, 7, 6, 2, 7, 4, 5, 6, 4, 0, 1, 5, 1, 2, 6, 5, 4, 7, 3, 0,
};

static float g_vList[20] = 
{
	-1.0f, -1.0f, 1.0f, 0.0f, 0.0f,
	1.0f, -1.0f, 1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
};


static unsigned short g_pList[4] = 
{
	0, 1, 2, 3,
};





CLightObject::CLightObject()
{
	memset(m_position, 0, sizeof(float) * 3);
	memset(m_color, 0, sizeof(float) * 3);

	m_constantAtten = m_linearAtten = m_quadraticAtten = 0.0F;

	m_type	= UNDEFINED_LIGHT;

	m_bIsTransformed = false;
}

CLightObject::~CLightObject()
{

}



CPointLight::CPointLight() : CLightObject()
{
	m_type = POINT_LIGHT;
}

CPointLight::CPointLight(CPointLight& rhs)
{
	m_type = rhs.GetType();
	m_constantAtten = rhs.GetConstantAttenuation();
	m_linearAtten = rhs.GetLinearAttenuation();
	m_quadraticAtten = rhs.GetQuadraticAttenuation();

	rhs.GetPosition(m_position);
	rhs.GetColor(m_color);

	m_radius = rhs.GetRadius();
	m_bIsTransformed = rhs.IsTransformed();
}


CPointLight::CPointLight(CPointLight* rhs)
{
	m_type = rhs->GetType();
	m_constantAtten = rhs->GetConstantAttenuation();
	m_linearAtten = rhs->GetLinearAttenuation();
	m_quadraticAtten = rhs->GetQuadraticAttenuation();

	rhs->GetPosition(m_position);
	rhs->GetColor(m_color);

	m_radius = rhs->GetRadius();
	m_bIsTransformed = rhs->IsTransformed();
}


CPointLight::~CPointLight()
{

}


CPointLight* CPointLight::Clone()
{
	return new CPointLight(*this);
}


CSpotLight::CSpotLight() : CLightObject()
{
	memset(m_dir, 0, sizeof(float) * 3);

	m_type = SPOT_LIGHT;
}

CSpotLight::CSpotLight(CSpotLight& rhs)
{
	m_type = rhs.GetType();
	m_constantAtten = rhs.GetConstantAttenuation();
	m_linearAtten = rhs.GetLinearAttenuation();
	m_quadraticAtten = rhs.GetQuadraticAttenuation();

	rhs.GetPosition(m_position);
	rhs.GetColor(m_color);
	rhs.GetDirection(m_dir);
	m_FOV = rhs.GetFOV();
}


CSpotLight::CSpotLight(CSpotLight* rhs)
{
	m_type = rhs->GetType();
	m_constantAtten = rhs->GetConstantAttenuation();
	m_linearAtten = rhs->GetLinearAttenuation();
	m_quadraticAtten = rhs->GetQuadraticAttenuation();

	rhs->GetPosition(m_position);
	rhs->GetColor(m_color);
	rhs->GetDirection(m_dir);
	m_FOV = rhs->GetFOV();
}


CSpotLight::~CSpotLight()
{

}

CSpotLight* CSpotLight::Clone()
{
	return new CSpotLight(*this);
}



CDirectionalLight::CDirectionalLight() : CLightObject()
{
	memset( m_dir, 0, sizeof( float ) * 3 );

	m_type = DIRECTIONAL_LIGHT;
}

CDirectionalLight::CDirectionalLight( const CDirectionalLight& l )
{
	vec3_copy( m_dir, l.m_dir );
	l.GetColor( m_color );

	m_type = DIRECTIONAL_LIGHT;
}

CDirectionalLight::~CDirectionalLight() 
{
	
}

CDirectionalLight* CDirectionalLight::Clone()
{
	return new CDirectionalLight( *this );
}


CLightManager::CLightManager()
{
	m_bFirstCall = true;
	m_bFlashlight = false;
	m_nInsideLights = 0;
	m_nOutsideLights = 0;
	m_nDirectionalLights = 0;
	m_shaderHandle = g_pShaderInterface->AddShader("Media/Shaders/effect_deferredlight.vert", "Media/Shaders/effect_deferredlight.frag");
	m_globalShaderHandle = g_pShaderInterface->AddShader( "Media/Shaders/effect_deferredlightglobal.vert", "Media/Shaders/effect_deferredlightglobal.frag" );

	insideLights  = new s_light_vertex[MAX_LIGHTS * 8];
	outsideLights = new s_light_vertex[MAX_LIGHTS * 8];

	glGenBuffers(1, &m_insidevboID);
	glGenBuffers(1, &m_outsidevboID);
	glGenBuffers(1, &m_iboID);
	glGenVertexArrays(1, &m_insidevaoID);
	glGenVertexArrays(1, &m_outsidevaoID);

	// Add default flashlight //
	CSpotLight* flashlight = new CSpotLight;
	vec3 color = {0.33f, 0.33f, 0.27f};
	vec3 pos = {0.0f, 0.0f, 0.0f};
	vec3 dir = {0.0f, 1.0f, 0.0f};
	flashlight->SetColor(color);
	flashlight->SetPosition(pos);
	flashlight->SetDirection(dir);
	flashlight->SetFOV(75.0f);
	AddLight(flashlight);

	memset( m_globalLightVec, 0, sizeof( float ) * 3 );



	memset( m_globalLightColor, 0, sizeof( float ) * 3 );
}

CLightManager::~CLightManager()
{
	// Loop through Light list and clean up clones //
	std::list <CLightObject*>::iterator iter = m_lights.begin();
	while(iter != m_lights.end())
	{
		if(*iter)
		{
			delete (*iter);
			(*iter) = NULL;
		}

		++iter;
	}


	if(insideLights)
	{
	    delete[] insideLights;
	    insideLights = NULL;
	}

	if(outsideLights)
	{
	    delete[] outsideLights;
	    outsideLights = NULL;
	}


	glDeleteBuffers(1, &m_insidevboID);
	glDeleteBuffers(1, &m_outsidevboID);
	glDeleteBuffers(1, &m_iboID);
	glDeleteVertexArrays(1, &m_insidevaoID);
	glDeleteVertexArrays(1, &m_outsidevaoID);

	g_pShaderInterface->DeleteShader(m_shaderHandle);

	m_bFlashlight = false;
	m_nDirectionalLights = 0;
}


void CLightManager::AddLight(CLightObject* newLight)
{
	// We can simply query the light type, clone it //
	// and pack it into the light list //
	if(newLight->GetType() == POINT_LIGHT)
	{
		CPointLight* pNewLight = ((CPointLight*)newLight)->Clone();

		m_lights.push_back(pNewLight);
	}

	if(newLight->GetType() == SPOT_LIGHT)
	{
		CSpotLight* pNewLight = ((CSpotLight*)newLight)->Clone();

		m_lights.push_back(pNewLight);
	}

	if( newLight->GetType() == DIRECTIONAL_LIGHT )
	{
		CDirectionalLight* pNewLight = ( (CDirectionalLight*)newLight )->Clone();

		m_lights.push_back( pNewLight );
	}
}






bool CLightManager::SortLights(const CCameraObject& cam)
{
	float fSqrt2 = sqrtf(2.0F);
	int nInsideLights, nOutsideLights, nIndices;
	bool camInVolume = false;
	nInsideLights = nOutsideLights = nIndices = 0;
	vec3 camPos, viewVec;
	cam.GetPosition(camPos);
	cam.GetViewVector(viewVec);
	vec3_copy(m_curCamPos, camPos);
	vec3_copy(m_curCamDir, viewVec);
	m_curFarClipDist = cam.GetFarClip();
	m_curCamFOV = cam.GetFOV();
	m_nDirectionalLights = 0;

	
	vec3_clear( m_globalLightVec );
	vec3_clear( m_globalLightColor );


	nInsideLights = nOutsideLights = 0;
	mat4 curModelview;
	vec4 vsLightPos;
	glGetFloatv(GL_MODELVIEW_MATRIX, curModelview);

	vec3 vlist[8];

	std::list <CLightObject*>::iterator iter = m_lights.begin();
	if(!m_bFlashlight) iter++;
	while(iter != m_lights.end())
	{
		camInVolume = false;
		vec3 pos, color, dir;
		float linAtten, constAtten, quadAtten, fov, radius, lenDir;
		unsigned char type = (*iter)->GetType();

		// Get the parameters that are independent of light type //
		if(type == POINT_LIGHT)
		{
			CPointLight* pLight = ((CPointLight*)(*iter))->Clone();

			// no spot light info //
			fov = 0.0F;
			vec3_clear(dir);

			pLight->GetPosition(pos);
			pLight->GetColor(color);
			linAtten = pLight->GetLinearAttenuation();
			constAtten = pLight->GetConstantAttenuation();
			quadAtten = pLight->GetQuadraticAttenuation();
			radius = pLight->GetRadius();

			vec3_set(vlist[0], pos[0] - radius, pos[1] - radius, pos[2] + radius);
			vec3_set(vlist[1], pos[0] - radius, pos[1] + radius, pos[2] + radius);
			vec3_set(vlist[2], pos[0] + radius, pos[1] + radius, pos[2] + radius);
			vec3_set(vlist[3], pos[0] + radius, pos[1] - radius, pos[2] + radius);
			vec3_set(vlist[4], pos[0] - radius, pos[1] - radius, pos[2] - radius);
			vec3_set(vlist[5], pos[0] - radius, pos[1] + radius, pos[2] - radius);
			vec3_set(vlist[6], pos[0] + radius, pos[1] + radius, pos[2] - radius);
			vec3_set(vlist[7], pos[0] + radius, pos[1] - radius, pos[2] - radius);

			// Check whether cam is in frustum or not //
			vec3 mins, maxs;
			vec3_copy(mins, vlist[4]);
			vec3_copy(maxs, vlist[2]);

			// Use a little algebra to simplify the radius to the far corner of the bounding box
			// the function becomes:   radius * sqrt(2)
			if(point_in_sphere(camPos, pos, (radius + 0.5f) * fSqrt2))
				camInVolume = true;
//			if( point_in_aabb( camPos, mins, maxs ) )
	//			camInVolume = true;

			// Clones must be cleaned up //
			delete pLight;
		}

		else if(type == SPOT_LIGHT)
		{
			CSpotLight* pLight = ((CSpotLight*)(*iter))->Clone();

			// no point light info //
			radius = 0.0F;

			if(iter != m_lights.begin())
			{
				fov = deg_2_rad(pLight->GetFOV());
				pLight->GetDirection(dir);
				pLight->GetPosition(pos);
				pLight->GetColor(color);
			}

			else
			{
				vec3_copy(dir, viewVec);
				vec3_copy(pos, camPos);
				lenDir = 15.0f;
				vec3_normalize(dir);

				radius = lenDir * 0.5f;
				fov = deg_2_rad(75.0f);
			}

//			fov = pLight->GetFOV();
//			pLight->GetDirection(dir);
//			pLight->GetPosition(pos);
//			pLight->GetColor(color);
//			linAtten = pLight->GetLinearAttenuation();
//			constAtten = pLight->GetConstantAttenuation();
//			quadAtten = pLight->GetQuadraticAttenuation();

//			lenDir = vec3_length(dir);
//			vec3_normalize(dir);

			// Now we must construct the spot light frustum //
			// Construct first arb world y axis //
			vec3 yax = {0.0F, 1.0F, 0.0F};
			vec3 right, up;

			// Right becomes dir X yax //
			vec3_cross(yax, dir, right);
			vec3_normalize(right);

			// True up becomes dir X right //
			vec3_cross(dir, right, up);
			vec3_normalize(up);

			// We now have our coordinate frame for the light set //
			// Now we must figure the half vertical length of the frustum //
			float leg = tanf(fov * 0.5F) * lenDir;

			// Now we can create the far plane dimensions //
			// Rescale the light direction to its original length //
			vec3 farPlane, rightFar, upFar;
			vec3_scale(dir, lenDir);

			// the far plane center becomes the light position + dir //
			vec3_add(pos, dir, farPlane);

			// The horizontal and vertical extents then become the scaled //
			// frustum length by the normalized up and right coord-frame vectors //
			vec3_copy(rightFar, right);
			vec3_copy(upFar, up);
			vec3_scale(rightFar, leg);
			vec3_scale(upFar, leg);

			// Make the shape //
			vec3_set(vlist[0], pos[0] - right[0] - up[0], pos[1] - right[1] - up[1], pos[2] - right[2] - up[2]);
			vec3_set(vlist[1], pos[0] + right[0] - up[0], pos[1] + right[1] - up[1], pos[2] + right[2] - up[2]);
			vec3_set(vlist[2], pos[0] + right[0] + up[0], pos[1] + right[1] + up[1], pos[2] + right[2] + up[2]);
			vec3_set(vlist[3], pos[0] - right[0] + up[0], pos[1] - right[1] + up[1], pos[2] - right[2] + up[2]);

			vec3_set(vlist[4], farPlane[0] - rightFar[0] - upFar[0], farPlane[1] - rightFar[1] - upFar[1], farPlane[2] - rightFar[2] - upFar[2]);
			vec3_set(vlist[5], farPlane[0] + rightFar[0] - upFar[0], farPlane[1] + rightFar[1] - upFar[1], farPlane[2] + rightFar[2] - upFar[2]);
			vec3_set(vlist[6], farPlane[0] + rightFar[0] + upFar[0], farPlane[1] + rightFar[1] + upFar[1], farPlane[2] + rightFar[2] + upFar[2]);
			vec3_set(vlist[7], farPlane[0] - rightFar[0] + upFar[0], farPlane[1] - rightFar[1] + upFar[1], farPlane[2] - rightFar[2] + upFar[2]);

			// renormalize light dir //
			vec3_normalize(dir);

			// TODO: add frustum or spherical/conical collision check to light frustum //
			camInVolume = true;

			delete pLight;
		}

		else if( type == DIRECTIONAL_LIGHT )
		{
			CDirectionalLight* pLight = ( (CDirectionalLight*)(*iter) )->Clone();
			vec3 dir, color;
			pLight->GetDirection( dir );
			pLight->GetColor( color );
			vec3_add_replace( m_globalLightVec, dir );
			vec3_add_replace( m_globalLightColor, color );
			vec3_normalize( dir );

			++m_nDirectionalLights;
		}

		else
			return false;

		vec4 lightPos = {pos[0], pos[1], pos[2], 1.0F};
		vec4_copy( vsLightPos, lightPos );
		mat4_mul_vec4(curModelview, lightPos, vsLightPos);

		// Now if we're in the volume pack the determined info into the inside
		// export list //
		if(camInVolume)
		{
			for(int j = 0; j < 8; ++j)
			{
				memcpy(&insideLights[nInsideLights].lightPos, vsLightPos, sizeof(float) * 3);

				insideLights[nInsideLights].colorDist[0] = color[0];
				insideLights[nInsideLights].colorDist[1] = color[1];
				insideLights[nInsideLights].colorDist[2] = color[2];
				insideLights[nInsideLights].colorDist[3] = radius;

				insideLights[nInsideLights].wPosition[0] = vlist[j][0];
				insideLights[nInsideLights].wPosition[1] = vlist[j][1];
				insideLights[nInsideLights].wPosition[2] = vlist[j][2];

				if(type == POINT_LIGHT)
					vec4_set(insideLights[nInsideLights].dirFOV, 0.0F, 0.0F, 0.0F, 0.0F);
				else if(type == SPOT_LIGHT)
					vec4_set(insideLights[nInsideLights].dirFOV, dir[0], dir[1], dir[2], cosf((fov * 0.5)));
				else
					vec4_set(insideLights[nInsideLights].dirFOV, 0.0F, 0.0F, 0.0F, 0.0F);

				++nInsideLights;
			}
		}

		// Otherwise to the outside list //
		else
		{
			for(int j = 0; j < 8; ++j)
			{
				memcpy(&outsideLights[nOutsideLights].lightPos, vsLightPos, sizeof(float) * 3);

				outsideLights[nOutsideLights].colorDist[0] = color[0];
				outsideLights[nOutsideLights].colorDist[1] = color[1];
				outsideLights[nOutsideLights].colorDist[2] = color[2];
				outsideLights[nOutsideLights].colorDist[3] = radius;

				outsideLights[nOutsideLights].wPosition[0] = vlist[j][0];
				outsideLights[nOutsideLights].wPosition[1] = vlist[j][1];
				outsideLights[nOutsideLights].wPosition[2] = vlist[j][2];

				if(type == POINT_LIGHT)
					vec4_set(outsideLights[nOutsideLights].dirFOV, 0.0F, 0.0F, 0.0F, 0.0F);
				else if(type == SPOT_LIGHT)
					vec4_set(outsideLights[nOutsideLights].dirFOV, dir[0], dir[1], dir[2], cosf(fov * 0.5));
				else
					vec4_set(outsideLights[nOutsideLights].dirFOV, 0.0F, 0.0F, 0.0F, 0.0F);

				++nOutsideLights;
			}
		}

		++iter;
	}

	m_nInsideLights = nInsideLights / 8;
	m_nOutsideLights = nOutsideLights / 8;

	// Upload the buffer if this is object instance time //
	if(m_bFirstCall)
	{
		m_bFirstCall = false;

		glBindBuffer(GL_ARRAY_BUFFER_ARB, m_insidevboID);
		glBufferData(GL_ARRAY_BUFFER_ARB, sizeof(s_light_vertex) * MAX_LIGHTS * 8, insideLights, GL_DYNAMIC_COPY_ARB);


		if(post_gl_errors())
		{
			printf("Error creating light vertex buffer (inside)\n");
			system("PAUSE");
			m_bFirstCall = false;
			return false;
		}

		glBindBuffer(GL_ARRAY_BUFFER_ARB, 0);


		glBindBuffer(GL_ARRAY_BUFFER_ARB, m_outsidevboID);
		glBufferData(GL_ARRAY_BUFFER_ARB, sizeof(s_light_vertex) * MAX_LIGHTS * 8, outsideLights, GL_DYNAMIC_COPY_ARB);
		glBindBuffer(GL_ARRAY_BUFFER_ARB, 0);

		if(post_gl_errors())
		{
			printf("Error Creating light vertex buffer (outside)\n");
			system("PAUSE");
			m_bFirstCall = false;
			return false;
		}

		// Populate poly list //
		unsigned short* polyList = new unsigned short[MAX_LIGHTS * 24];
		int nIndices = 0;
		for(int z = 0; z < MAX_LIGHTS; ++z)
		{
			for(int p = 0; p < 24; ++p, ++nIndices)
			{
				polyList[nIndices] = plist[p] + (z * 8);
			}
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, m_iboID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER_ARB, sizeof(unsigned short) * MAX_LIGHTS * 24, polyList, GL_STATIC_DRAW_ARB);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

		if(post_gl_errors())
		{
			printf("Error Creating light index buffer\n");
			system("PAUSE");
			m_bFirstCall = false;
			return false;
		}
	}


	// Otherwise we are simply updating the data //
	else
	{
		glBindBuffer(GL_ARRAY_BUFFER_ARB, m_insidevboID);
		glBufferData(GL_ARRAY_BUFFER_ARB, sizeof(s_light_vertex) * m_nInsideLights * 8, insideLights, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER_ARB, m_outsidevboID);
		glBufferData(GL_ARRAY_BUFFER_ARB, sizeof(s_light_vertex) * m_nOutsideLights * 8, outsideLights, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER_ARB, 0);
/*
		s_light_vertex* vmap = NULL;
		glBindBuffer(GL_ARRAY_BUFFER_ARB, m_insidevboID);

		// Get device memory and lock the memory for vbuffer //
		vmap = (s_light_vertex*)glMapBuffer(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);

        if(!post_gl_errors())
        {
            // DMA the data //
            memcpy(vmap, insideLights, sizeof(s_light_vertex) * MAX_LIGHTS * 8);
        }

        else
        {
            printf("Error in critical section, mapping to insideVBO\n");
            return false;
        }
		// Unlock the memory //
		if(glUnmapBuffer(GL_ARRAY_BUFFER_ARB) == GL_FALSE)
		{
			glBindBuffer(GL_ARRAY_BUFFER_ARB, 0);
			printf("There was an error copying vertex buffer data\n");
			return false;
		}
		else
			vmap = NULL;

		// Do the same for outside buffer //
		glBindBuffer(GL_ARRAY_BUFFER_ARB, m_outsidevboID);

		vmap = (s_light_vertex*)glMapBuffer(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);
		if(!post_gl_errors())
            memcpy(vmap, outsideLights, sizeof(s_light_vertex) * MAX_LIGHTS * 8);
        else
        {
            printf("Error in critical section, mapping to outsideVBO\n");
            return false;
        }

		if(glUnmapBuffer(GL_ARRAY_BUFFER_ARB) == GL_FALSE)
		{
			glBindBuffer(GL_ARRAY_BUFFER_ARB, 0);
			printf("There was an error copying vertex buffer data\n");
			return false;
		}
		else
			vmap = NULL;

		glBindBuffer(GL_ARRAY_BUFFER_ARB, 0);
*/
	}

	m_bFirstCall = false;
	return true;
}


static void draw_bounding_light_vol(vec3 pos, float rad, vec3 color, float camDist)
{
	vec3 vlist[8] =
	{
		{pos[0] - rad, pos[1] - rad, pos[2] + rad},
		{pos[0] - rad, pos[1] + rad, pos[2] + rad},
		{pos[0] + rad, pos[1] + rad, pos[2] + rad},
		{pos[0] + rad, pos[1] - rad, pos[2] + rad},
		{pos[0] - rad, pos[1] - rad, pos[2] - rad},
		{pos[0] - rad, pos[1] + rad, pos[2] - rad},
		{pos[0] + rad, pos[1] + rad, pos[2] - rad},
		{pos[0] + rad, pos[1] - rad, pos[2] - rad},
	};

	glBegin(GL_QUADS);
		glColor4f(color[0], color[1], color[2], camDist);

		glVertex3fv(vlist[0]);
		glVertex3fv(vlist[4]);
		glVertex3fv(vlist[5]);
		glVertex3fv(vlist[1]);

		glVertex3fv(vlist[7]);
		glVertex3fv(vlist[3]);
		glVertex3fv(vlist[2]);
		glVertex3fv(vlist[6]);

		glVertex3fv(vlist[4]);
		glVertex3fv(vlist[7]);
		glVertex3fv(vlist[6]);
		glVertex3fv(vlist[5]);

		glVertex3fv(vlist[5]);
		glVertex3fv(vlist[6]);
		glVertex3fv(vlist[2]);
		glVertex3fv(vlist[1]);

		glVertex3fv(vlist[4]);
		glVertex3fv(vlist[7]);
		glVertex3fv(vlist[3]);
		glVertex3fv(vlist[0]);
	glEnd();
}



void CLightManager::DebugRenderLights()
{
	std::list <CLightObject*>::iterator iter = m_lights.begin();
	while( iter != m_lights.end() )
	{
		vec3 pos;
		(*iter)->GetPosition( pos );

		float rad = 0.0f;
		if( (*iter)->GetType() == POINT_LIGHT )
			rad = ( (CPointLight*)(*iter) )->GetRadius();
		else
			continue;

		vec3 vlist[8] =
		{
			{pos[0] - rad, pos[1] - rad, pos[2] + rad},
			{pos[0] - rad, pos[1] + rad, pos[2] + rad},
			{pos[0] + rad, pos[1] + rad, pos[2] + rad},
			{pos[0] + rad, pos[1] - rad, pos[2] + rad},
			{pos[0] - rad, pos[1] - rad, pos[2] - rad},
			{pos[0] - rad, pos[1] + rad, pos[2] - rad},
			{pos[0] + rad, pos[1] + rad, pos[2] - rad},
			{pos[0] + rad, pos[1] - rad, pos[2] - rad},
		};

		glBegin( GL_LINES );
			glColor3f( 1.0f, 1.0f, 1.0f );

			glVertex3fv( vlist[0] );
			glVertex3fv( vlist[1] );
			glVertex3fv( vlist[1] );
			glVertex3fv( vlist[2] );
			glVertex3fv( vlist[2] );
			glVertex3fv( vlist[3] );
			glVertex3fv( vlist[3] );
			glVertex3fv( vlist[0] );

			glVertex3fv( vlist[4] );
			glVertex3fv( vlist[5] );
			glVertex3fv( vlist[5] );
			glVertex3fv( vlist[6] );
			glVertex3fv( vlist[6] );
			glVertex3fv( vlist[7] );
			glVertex3fv( vlist[7] );
			glVertex3fv( vlist[4] );

			glVertex3fv( vlist[1] );
			glVertex3fv( vlist[5] );
			glVertex3fv( vlist[2] );
			glVertex3fv( vlist[6] );
			glVertex3fv( vlist[0] );
			glVertex3fv( vlist[4] );
			glVertex3fv( vlist[3] );
			glVertex3fv( vlist[7] );
		glEnd();

		iter++;
	}
}



void CLightManager::RenderLights(const int& ndc)
{
//	g_pShaderInterface->BindShader(m_shaderHandle);


	// Light buffers are stored in two buffers depending on whether the cam is inside or outside the light
	// The render state changes based on this fact
//	g_pShaderInterface->SetStateMatrix("g_matWorldViewProj", MATRIX_MODELVIEW | MATRIX_PROJECTION);
//	g_pShaderInterface->SetStateMatrix("g_matWorldView", MATRIX_MODELVIEW);

	float dw, dh;
	dw = (float)GetScreenWidth();
	dh = (float)GetScreenHeight();

//	g_pShaderInterface->SetFloat("g_dispWidth", &dw, 1);
//	g_pShaderInterface->SetFloat("g_dispHeight", &dh, 1);

	float tHalfFOV = tanf(deg_2_rad(m_curCamFOV) * 0.5F);
	float farHeight = tHalfFOV * m_curFarClipDist;
	float farWidth = farHeight * (dw / dh);
	vec3 dispDims = {farWidth, farHeight, m_curFarClipDist};


//	g_pShaderInterface->SetFloat("g_clipDims", dispDims, 3);
//	g_pShaderInterface->SetTexture("g_normalDepthSampler", ndc);


	// Setup for a depth only pass //
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glCullFace(GL_FRONT);

	char* base = 0;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, m_iboID);

	// Now setup for a color pass w/ depth test only (no z-writes) //
	g_pShaderInterface->BindShader(m_shaderHandle);
	g_pShaderInterface->SetStateMatrix("g_matWorldViewProj", MATRIX_MODELVIEW | MATRIX_PROJECTION);
	g_pShaderInterface->SetStateMatrix("g_matWorldView", MATRIX_MODELVIEW);
	g_pShaderInterface->SetFloat("g_dispWidth", &dw, 1);
	g_pShaderInterface->SetFloat("g_dispHeight", &dh, 1);
	g_pShaderInterface->SetFloat("g_clipDims", dispDims, 3);
	g_pShaderInterface->SetTexture("g_normalDepthSampler", ndc);
	if(m_nOutsideLights > 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER_ARB, m_outsidevboID);

		GLint posLoc = g_pShaderInterface->GetAttribLoc("in_position");
		GLint colorLoc = g_pShaderInterface->GetAttribLoc("in_colorDist");
		GLint lightPosLoc = g_pShaderInterface->GetAttribLoc("in_lightPosVS");
		GLint lightDirLoc = g_pShaderInterface->GetAttribLoc("in_lightDirFOV");
		glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, sizeof(s_light_vertex), base);
		glEnableVertexAttribArray(posLoc);
		glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, sizeof(s_light_vertex), base + 12);
		glEnableVertexAttribArray(colorLoc);
		glVertexAttribPointer(lightPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(s_light_vertex), base + 28);
		glEnableVertexAttribArray(lightPosLoc);
		glVertexAttribPointer(lightDirLoc, 4, GL_FLOAT, GL_FALSE, sizeof(s_light_vertex), base + 40);
		glEnableVertexAttribArray(lightDirLoc);
	

		glDrawElements(GL_QUADS, m_nOutsideLights * 6 * 4, GL_UNSIGNED_SHORT, 0);

		glDisableVertexAttribArray(posLoc);
		glDisableVertexAttribArray(colorLoc);
		glDisableVertexAttribArray(lightPosLoc);
		glDisableVertexAttribArray(lightDirLoc);

		glBindBuffer(GL_ARRAY_BUFFER_ARB, 0);
	}

	glCullFace(GL_BACK);
	glDepthFunc(GL_GREATER);
	if(m_nInsideLights > 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER_ARB, m_insidevboID);

		GLint posLoc = g_pShaderInterface->GetAttribLoc("in_position");
		GLint colorLoc = g_pShaderInterface->GetAttribLoc("in_colorDist");
		GLint lightPosLoc = g_pShaderInterface->GetAttribLoc("in_lightPosVS");
		GLint lightDirLoc = g_pShaderInterface->GetAttribLoc("in_lightDirFOV");
		glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, sizeof(s_light_vertex), base);
		glEnableVertexAttribArray(posLoc);
		glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, sizeof(s_light_vertex), base + 12);
		glEnableVertexAttribArray(colorLoc);
		glVertexAttribPointer(lightPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(s_light_vertex), base + 28);
		glEnableVertexAttribArray(lightPosLoc);
		glVertexAttribPointer(lightDirLoc, 4, GL_FLOAT, GL_FALSE, sizeof(s_light_vertex), base + 40);
		glEnableVertexAttribArray(lightDirLoc);

		glDrawElements(GL_QUADS, m_nInsideLights * 6 * 4, GL_UNSIGNED_SHORT, base);

		glDisableVertexAttribArray(posLoc);
		glDisableVertexAttribArray(colorLoc);
		glDisableVertexAttribArray(lightPosLoc);
		glDisableVertexAttribArray(lightDirLoc);

		glBindBuffer(GL_ARRAY_BUFFER_ARB, 0);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

	

	glDisable(GL_CULL_FACE);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);

	g_pShaderInterface->UnbindShader();

/*
	if( m_nDirectionalLights > 0 )
	{
		glDisable( GL_DEPTH_TEST );
		glDepthMask( GL_FALSE );

		g_pShaderInterface->BindShader( m_globalShaderHandle );
		g_pShaderInterface->SetFloat( "g_lightVec", m_globalLightVec, 3 );
		g_pShaderInterface->SetFloat( "g_lightColor", m_globalLightColor, 3 );
		g_pShaderInterface->SetTexture( "g_normalDepthSampler", ndc );
		g_pShaderInterface->SetStateMatrix( "g_matWorldView", MATRIX_MODELVIEW );

		GLint posLoc = g_pShaderInterface->GetAttribLoc( "in_position" );
		GLint texLoc = g_pShaderInterface->GetAttribLoc( "in_texCoords" );

		glVertexAttribPointer( posLoc, 3, GL_FLOAT, GL_FALSE, 20, g_vList );
		glEnableVertexAttribArray( posLoc );
		glVertexAttribPointer( texLoc, 2, GL_FLOAT, GL_FALSE, 20, g_vList + 3 );
		glEnableVertexAttribArray( texLoc );

		glDrawElements( GL_QUADS, 4, GL_UNSIGNED_SHORT, g_pList );

		glDisableVertexAttribArray( posLoc );
		glDisableVertexAttribArray( texLoc );

		g_pShaderInterface->UnbindShader();

		glEnable( GL_DEPTH_TEST );
		glDepthMask( GL_TRUE );
	}
*/
}



