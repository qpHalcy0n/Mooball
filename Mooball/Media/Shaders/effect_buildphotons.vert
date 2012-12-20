

varying vec2		out_texcoords;
varying vec3		out_viewRays;

void main()
{
	gl_Position = gl_Vertex;
	out_texcoords = gl_MultiTexCoord0.xy;
	out_viewRays = gl_Normal;
}