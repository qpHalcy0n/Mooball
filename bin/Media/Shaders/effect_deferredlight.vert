uniform mat4		g_matWorldViewProj;
uniform mat4		g_matWorldView;
uniform float		g_dispWidth;
uniform float		g_dispHeight;


varying vec4		out_texcoords;
varying vec3		out_lightcolor;
varying vec4		out_lightPosVS;
varying float		out_radius;
varying vec4		out_vertexPosVS;

varying vec3		out_dir;
varying float		out_fov;


in vec3				in_position;
in vec4				in_colorDist;
in vec3				in_lightPosVS;
in vec4				in_lightDirFOV;

void main()
{	
	vec4 tpos;
	tpos = g_matWorldViewProj * vec4(in_position, 1.0);
	gl_Position = tpos;
	
	vec4 tc = tpos;
	tc.x = ((tpos.x + tpos.w) * g_dispWidth + tpos.w) * (1.0 / (2.0 * g_dispWidth));
	tc.y = ((tpos.y + tpos.w) * g_dispHeight + tpos.w) * (1.0 / (2.0 * g_dispHeight));
	
	float iX = 0.5 / g_dispWidth;
	float iY = 0.5 / g_dispHeight;
	tc.x -= iX;
	tc.y -= iY;
	
	out_texcoords = tc;
	out_lightcolor = in_colorDist.xyz;
	out_lightPosVS = vec4(in_lightPosVS, 1.0);
	out_vertexPosVS = g_matWorldView * vec4(in_position, 1.0);
	out_radius = in_colorDist.w;

	out_dir = (g_matWorldView * vec4(in_lightDirFOV.xyz, 0.0)).xyz;
	
	out_fov = in_lightDirFOV.w;
}
