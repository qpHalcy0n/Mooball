

uniform vec2		g_viewDims;

varying vec2		out_viewDims;
varying vec2		out_texCoords;

void main()
{
	gl_Position = gl_Vertex;

	out_viewDims = g_viewDims;
	out_texCoords = gl_MultiTexCoord0.xy;
}