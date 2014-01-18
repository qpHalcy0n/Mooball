uniform mat4		g_matWorldViewProj;
uniform mat4		g_matWorldView;
uniform float		g_farClip;
uniform float		g_bias;


varying float		out_dist;


void main()
{
	gl_Position = g_matWorldViewProj * gl_Vertex;

	vec4 adjustedWPos = (g_matWorldView * gl_Vertex) * g_bias;
	out_dist = adjustedWPos.z / g_farClip;
}
