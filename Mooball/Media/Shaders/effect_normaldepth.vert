
// Constant values //
uniform mat4	g_matWorldViewProj;			
uniform mat4	g_matWorldView;
uniform float	g_farPlane;	





// Passed to frag shader //
varying vec4	out_normalDepth;
varying vec4	out_color;




void main()
{
	gl_Position = g_matWorldViewProj * gl_Vertex;
	
	vec4 normDepth;
	vec4 vsPos = g_matWorldView * gl_Vertex;
	normDepth.w = vsPos.z / g_farPlane;
	
	vec4 vsNorm = g_matWorldView * vec4(gl_Normal, 0);
	vsNorm = normalize(vsNorm);
	normDepth.xyz = vsNorm.xyz;
	
	out_color = gl_Color;
	out_normalDepth.xyz = normDepth.xyz;
	out_normalDepth.w = normDepth.w;
}