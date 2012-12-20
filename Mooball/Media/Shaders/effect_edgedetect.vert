
uniform vec2		g_viewDims;

varying vec2		out_texCoords;
varying vec2		out_dispDims;

void main()
{
	gl_Position = gl_Vertex;

	out_dispDims  = g_viewDims;
	out_texCoords = gl_MultiTexCoord0.xy;
}