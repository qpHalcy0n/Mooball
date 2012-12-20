uniform sampler2D	g_lightBuffer;
uniform sampler2D   g_diffuseTexture;
uniform int			g_bIsTextured;


varying vec4		out_ssCoords;
varying vec4		out_color;
varying vec2		out_samplingCoords;

void main()
{
	vec4 diffuseColor;
	if( g_bIsTextured == 1 )
		diffuseColor = texture2D( g_diffuseTexture, out_samplingCoords );
	else
		diffuseColor = out_color;
	
	vec4 lightBufferColor = texture2DProj(g_lightBuffer, out_ssCoords);
	gl_FragColor = vec4(diffuseColor.xyz * lightBufferColor.xyz + lightBufferColor.xyz, out_color.w);	
}
