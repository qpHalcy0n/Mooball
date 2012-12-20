uniform mat4		g_matWorldViewProj;
uniform vec2		g_viewDims;


varying vec4		out_ssCoords;
varying vec2		out_samplingCoords;
varying vec4		out_color;


void main()
{
	vec4 oPos;
	oPos = g_matWorldViewProj * gl_Vertex;
	gl_Position = oPos;
	
	oPos.x = ((oPos.x + oPos.w) * g_viewDims.x + oPos.w) * (1.0 / (2.0 * g_viewDims.x));
	oPos.y = ((oPos.y + oPos.w) * g_viewDims.y + oPos.w) * (1.0 / (2.0 * g_viewDims.y));

	float iX = 0.5 / g_viewDims.x;
	float iY = 0.5 / g_viewDims.y;
	oPos.x -= iX;
	oPos.y -= iY;
	
	out_ssCoords = oPos;
	out_color = gl_Color;
	out_samplingCoords = gl_MultiTexCoord0.xy;
}
