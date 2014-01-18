
uniform sampler2D	g_normalDepthSampler;

uniform vec3		g_clipDims;
uniform float		g_dispWidth;
uniform float		g_dispHeight;

varying vec4		out_texcoords;
varying vec3		out_lightcolor;
varying vec4		out_lightPosVS;
varying float		out_radius;
varying vec4		out_vertexPosVS;

varying vec3		out_dir;
varying float		out_fov;

float atten_s = 100.0;

float get_attenuation(vec3 lightVec, float radius)
{
//	float dist = length(lightVec);
//	float atten = (dist * dist) / (radius * radius);
//	atten = 1.0 / (atten * atten_s + 1.0);
//	atten_s = 1.0 / (atten_s + 1.0);
//	atten = atten - atten_s;
//	atten /= 1.0 - atten_s;
//	return atten;

	float dist = length(lightVec);
	float atten = clamp((1.0 - dist / radius), 0.0, 1.0);
	atten *= atten;
	return atten;
}


float get_spotlight_attenuation(vec3 L, vec3 adjustedDir, float outerFOV, float innerFOV)
{
	float cosDir = dot(L, adjustedDir);
	return 1.0 - smoothstep(outerFOV, innerFOV, cosDir);
}

void main()
{
	vec4 drs = texture2DProj(g_normalDepthSampler, out_texcoords);
	vec3 snorm = drs.xyz;
	
	vec3 vFrustumRay = out_vertexPosVS.xyz * ( g_clipDims.z / out_vertexPosVS.z );
	vec3 surfacePos = vFrustumRay * drs.w;
	
	vec3 L = out_lightPosVS.xyz - surfacePos;
	float atten = get_attenuation(L, out_radius);
	L = normalize(L);
	
	float spotlightAtten = 1.0;
	if(out_fov >= 0.001)
	{
		spotlightAtten = 0.0;
		spotlightAtten = get_spotlight_attenuation( -L, out_dir, out_fov, out_fov - 0.1 );
//		atten = 0.25;
//		atten = 100.0;
	}
	
	float NL = max(dot(L, snorm), 0.0);
	vec3 V = -surfacePos;
	vec3 H = normalize(L + V);
	float NS = pow(max(dot(snorm, H), 0), 80.0);
	if(NL <= 0)
		NS = 0;
	
	vec3 finalLight = out_lightcolor.xyz * (NL + NS) * atten;// * spotlightAtten;
	
	gl_FragColor = vec4( finalLight, 0.0 );
}
