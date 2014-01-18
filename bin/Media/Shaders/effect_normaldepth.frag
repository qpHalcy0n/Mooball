

varying vec4	out_normalDepth;
varying vec4	out_color;


void main()
{
	gl_FragData[0] = out_normalDepth;
	gl_FragData[1] = out_color;
}