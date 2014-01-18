
uniform sampler2D		g_normalDepthBuffer;
uniform sampler2D		g_colorBuffer;
uniform sampler2D		g_randomRays;

uniform mat4			matrix_projection;
uniform mat4			matrix_modelview;
uniform float			g_farClip;
uniform vec4			g_frustumDims;
uniform vec3			g_lightPos;

varying vec2			out_texcoords;
varying vec3			out_viewRays;



void main()
{
	vec4 randomSample = texture2D(g_randomRays, out_texcoords);
	vec4 depthSample = texture2D(g_normalDepthBuffer, randomSample.xy);
	vec4 colorSample = texture2D(g_colorBuffer, randomSample.xy);
	vec3 vDir;
	vDir.x = mix(g_frustumDims.x, g_frustumDims.y, randomSample.x);
	vDir.y = mix(g_frustumDims.z, g_frustumDims.w, randomSample.y);
	vDir.z = g_farClip;
	vDir *= depthSample.w;
	
	vec4 lightPosVS = matrix_modelview * vec4(g_lightPos, 0.0);
	vec3 L = vec3(lightPosVS.xy, -lightPosVS.z) - vDir;
	
	gl_FragColor = vec4(vDir + (normalize(randomSample.xyz) * 0.5), 1.0);
}
