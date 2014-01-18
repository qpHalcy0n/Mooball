uniform sampler2D	g_edgeBuffer;
uniform sampler2D	g_colorBuffer;



varying vec2		out_viewDims;
varying vec2		out_texCoords;

vec2 qsamples[8] =
{
	vec2(-1.0, -1.0),
	vec2(1.0, -1.0),
	vec2(1.0, 1.0),
	vec2(-1.0, 1.0),

	vec2(1.0, 0.0),
	vec2(-1.0, 0.0),
	vec2(0.0, 1.0),
	vec2(0.0, -1.0)
};

void main()
{
	float weight = texture2D(g_edgeBuffer, out_texCoords).r;
	vec4 oColor = texture2D(g_colorBuffer, out_texCoords);
	if(weight <= 0)
	{
		gl_FragColor = oColor;
		return;
	}
		
	vec4 aColor = vec4(0, 0, 0, 0);
	vec2 pixsize = vec2(1.0 / out_viewDims.x, 1.0 / out_viewDims.y);
	
	for(int i = 0; i < 4; ++i)
	{
		// Add half pixel bias into the equation //
		aColor += texture2D(g_colorBuffer, out_texCoords + (qsamples[i] * vec2(0.5, 0.5)) * pixsize);
	}
	 
	aColor /= 4;
	oColor = mix(oColor, aColor, weight); 
	
	gl_FragColor = oColor;
}
