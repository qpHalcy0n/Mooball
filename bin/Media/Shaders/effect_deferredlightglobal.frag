varying vec2	out_texCoords;
varying vec2	out_vec;
varying vec3 	out_lightVec;

uniform sampler2D	g_normalDepthSampler;
uniform vec3		g_lightColor;

void main()
{
	vec4 drs = texture2D( g_normalDepthSampler, out_texCoords );
	vec3 sNorm = drs.xyz;
	
	vec3 L = normalize( out_lightVec );
	float NL = max( dot( sNorm, L ), 0.0 );

	gl_FragColor = vec4( g_lightColor * NL, 1.0 );
}