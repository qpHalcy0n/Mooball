uniform sampler2D	edgeSampler;

varying vec2		out_texCoords;

varying vec2		out_dispDims;



vec2 offsets[9] =
{
	vec2(0.0, 0.0),
	vec2(-1.0, -1.0),
	vec2(0.0, -1.0),
	vec2(1.0, -1.0),
	vec2(1.0, 0.0),
	vec2(1.0, 1.0),
	vec2(0.0, 1.0),
	vec2(-1.0, 1.0),
	vec2(-1.0, 0.0)
};

void main()
{
	float depth[9];
	vec3 norm[9];
	vec2 pixelSize = vec2(1.0 / out_dispDims.x, 1.0 / out_dispDims.y);
	
	for(int i = 0; i < 9; ++i)
	{
		vec2 uv = out_texCoords + offsets[i] * pixelSize;
		float d;
		vec3 n;
	
		vec4 db = texture2D(edgeSampler, uv);
		d = db.w;
		n = db.xyz;
		
		depth[i] = d;
		norm[i] = n;
	}
	
	vec4 d1 = vec4(depth[1], depth[2], depth[3], depth[4]);
	vec4 d2 = vec4(depth[5], depth[6], depth[7], depth[8]);
	
	d1 = abs(d1 - depth[0]);
	d2 = abs(depth[0] - d2);
	
	vec4 maxd = max(d1, d2);
	vec4 mind = max(min(d1, d2), 0.00001);
	vec4 finald = step(mind * 25.0, maxd);
	
	d1.x = dot(norm[1], norm[0]);
	d1.y = dot(norm[2], norm[0]);
	d1.z = dot(norm[3], norm[0]);
	d1.w = dot(norm[4], norm[0]);
	d2.x = dot(norm[5], norm[0]);
	d2.y = dot(norm[6], norm[0]);
	d2.z = dot(norm[7], norm[0]);
	d2.w = dot(norm[8], norm[0]);
	d1 = abs(d1 - d2);
	
	vec4 finalnormal = step(0.4, d1);
	finalnormal = max(finalnormal, finald);
	
	gl_FragColor = vec4(finalnormal.x + finalnormal.y + finalnormal.z + finalnormal.w) * 0.25;
}