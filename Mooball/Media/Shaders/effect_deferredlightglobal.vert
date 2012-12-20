
in vec3		in_position;
in vec2		in_texCoords;


varying vec2	out_texCoords;
varying vec2	out_vec;
varying vec3 	out_lightVec;

uniform mat4	g_matWorldView;
uniform vec3	g_lightVec;

void main()
{
	gl_Position = vec4( in_position, 1.0 );
	out_vec = in_position.xy;
	out_texCoords = in_texCoords;
	out_lightVec = (g_matWorldView * vec4( g_lightVec, 0.0 )).xyz;
}