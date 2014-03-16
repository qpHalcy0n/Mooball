///////////////////////////////////////////////////////////////////////////
//
// QUICKMATH.H
//
// Adapted for Mooball project 12/09
//
// This header contains all essential mathematical operations, matrix,
// vector, quaternion ops. It also contains several visibility routines
//
// see: http://googlecode.com/p/mooball
//
////////////////////////////////////////////////////////////////////////////


#include <math.h>
#include <string.h>



#ifndef __QUICKMATH_H_
#define __QUICKMATH_H_


// Math type definitions //
typedef float vec2[2];
typedef float vec3[3];
typedef float vec4[4];

typedef float mat3[12];
typedef float mat4[16];

typedef float quat[4];


static float deg_2_rad(const float& deg)
{
	return 3.14159F * deg / 180.0F;
}

static float rad_2_deg(const float& rad)
{
	return 180.0F / 3.14159F * rad;
}

static void swap(float l, float r)
{
	float tmp = l;
	l = r;
	r = tmp;
}




//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
//-=-=-=-=-=-=-=-= VECTOR3 ROUTINES -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//


static void vec3_clear(vec3 l)
{
	l[0] = 0.0F;
	l[1] = 0.0F;
	l[2] = 0.0F;
}


static void vec3_set(vec3 l, float x, float y, float z)
{
	l[0] = x;
	l[1] = y;
	l[2] = z;
}

static void vec4_set(vec4 l, float x, float y, float z, float w)
{
	l[0] = x;
	l[1] = y;
	l[2] = z;
	l[3] = w;
}


// 3 component piecewise sub //
static void vec3_sub(const vec3 l, const vec3 r, vec3 ret)
{
	ret[0] = l[0] - r[0];
	ret[1] = l[1] - r[1];
	ret[2] = l[2] - r[2];
}

// 3 component piecewise sub and copy to lhs //
static void vec3_sub_replace(vec3 l, const vec3 r)
{
    l[0] -= r[0];
    l[1] -= r[1];
    l[2] -= r[2];
}

// 3 component piecewise add //
static void vec3_add(const vec3 l, const vec3 r, vec3 ret)
{
	ret[0] = l[0] + r[0];
	ret[1] = l[1] + r[1];
	ret[2] = l[2] + r[2];
}

// 3 component piecewise add and copy to lhs //
static void vec3_add_replace(vec3 l, const vec3 r)
{
    l[0] += r[0];
    l[1] += r[1];
    l[2] += r[2];
}

// Vector dot product //
static float vec3_dot(const vec3 l, const vec3 r)
{
	return l[0] * r[0] + l[1] * r[1] + l[2] * r[2];
}

// Vector cross product //
static void vec3_cross(const vec3 l, const vec3 r, vec3 ret)
{
	ret[0] = l[1] * r[2] - l[2] * r[1];
	ret[1] = l[2] * r[0] - l[0] * r[2];
	ret[2] = l[0] * r[1] - l[1] * r[0];
}

// Vector length as a scalar //
static float vec3_length(const vec3 v)
{
	return sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

// Vector normalization (length unrecoverable) //
static void vec3_normalize(vec3 v)
{
	float iLen = 1.0F / vec3_length(v);
	v[0] *= iLen;
	v[1] *= iLen;
	v[2] *= iLen;
}


// Vector scale (scalar) //
static void vec3_scale(vec3 v, const float& s)
{
	v[0] *= s;
	v[1] *= s;
	v[2] *= s;
}


// Vector copy (piecewise) //
static void vec3_copy(vec3 l, const vec3 r)
{
	l[0] = r[0];
	l[1] = r[1];
	l[2] = r[2];
}

static void vec4_copy(vec4 l, const vec4 r)
{
	l[0] = r[0];
	l[1] = r[1];
	l[2] = r[2];
	l[3] = r[3];
}



//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= MATRIX4 ROUTINES -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//


static void mat4_load_identity(mat4 m)
{
	m[0] = 1.0F;	m[4] = 0.0F;	m[8] = 0.0F;		m[12] = 0.0F;
	m[1] = 0.0F;	m[5] = 1.0F;	m[9] = 0.0F;		m[13] = 0.0F;
	m[2] = 0.0F;	m[6] = 0.0F;	m[10] = 1.0F;		m[14] = 0.0F;
	m[3] = 0.0F;    m[7] = 0.0F;	m[11] = 0.0F;       m[15] = 1.0F;
}

static void mat4_transpose(mat4 m)
{
	swap(m[1], m[4]);
	swap(m[2], m[8]);
	swap(m[3], m[12]);
	swap(m[6], m[9]);
	swap(m[7], m[13]);
	swap(m[11], m[14]);
}

// Brute force 4x4 matrix mul //
static void mat4_mul_mat4(const mat4 l, const mat4 r, mat4 output)
{
	output[0] = (l[0] * r[0]) + (l[4] * r[1]) + (l[8] * r[2]) + (l[12] * r[3]);
	output[4] = (l[0] * r[4]) + (l[4] * r[5]) + (l[8] * r[6]) + (l[12] * r[7]);
	output[8] = (l[0] * r[8]) + (l[4] * r[9]) + (l[8] * r[10]) + (l[12] * r[11]);
	output[12] = (l[0] * r[12]) + (l[4] * r[13]) + (l[8] * r[14]) + (l[12] * r[15]);

	output[1] = (l[1] * r[0]) + (l[5] * r[1]) + (l[9] * r[2]) + (l[13] * r[3]);
	output[5] = (l[1] * r[4]) + (l[5] * r[5]) + (l[9] * r[6]) + (l[13] * r[7]);
	output[9] = (l[1] * r[8]) + (l[5] * r[9]) + (l[9] * r[10]) + (l[13] * r[11]);
	output[13] = (l[1] * r[12]) + (l[5] * r[13]) + (l[9] * r[14]) + (l[13] * r[15]);

	output[2] = (l[2] * r[0]) + (l[6] * r[1]) + (l[10] * r[2]) + (l[14] * r[3]);
	output[6] = (l[2] * r[4]) + (l[6] * r[5]) + (l[10] * r[6]) + (l[14] * r[7]);
	output[10] = (l[2] * r[8]) + (l[6] * r[9]) + (l[10] * r[10]) + (l[14] * r[11]);
	output[14] = (l[2] * r[12]) + (l[6] * r[13]) + (l[10] * r[14]) + (l[14] * r[15]);

	output[3] = (l[3] * r[0]) + (l[7] * r[1]) + (l[11] * r[2]) + (l[15] * r[3]);
	output[7] = (l[3] * r[4]) + (l[7] * r[5]) + (l[11] * r[6]) + (l[15] * r[7]);
	output[11] = (l[3] * r[8]) + (l[7] * r[9]) + (l[11] * r[10]) + (l[15] * r[11]);
	output[15] = (l[3] * r[12]) + (l[7] * r[13]) + (l[11] * r[14]) + (l[15] * r[15]);
}


static void mat4_mul_vec4SSE(const mat4 l, const vec4 r, vec4 output)
{
	float* r0 = (float*)&l[0];

	#ifdef WIN32
        __asm
        {
            mov			esi, r					// Move address of r into esi
            mov 		edi, output				// Move address of out into edi

            mov			edx, r0
            movups		xmm4, [edx]				// Copy first col into xmm4    (16b)  from edx
            movups		xmm5, [edx + 0x10]		// Copy second col into xmm5   (16b)  from edx
            movups		xmm6, [edx + 0x20]		// Copy third col into xmm6    (16b)  from edx
            movups		xmm7, [edx + 0x30]		// Copy fourth col into xmm7   (16b)  from edx

            movups		xmm0, [esi]			// load vec into xmm0 from esi
            xorps		xmm2, xmm2			// Clear xmm2 to 0

            movups		xmm1, xmm0			// load vector into xmm1 from xmm0
            shufps		xmm1, xmm1, 0x00	// No shuf for x
            mulps		xmm1, xmm4			// Mul col0 by vec
            addps		xmm2, xmm1			// Add result to xmm2

            // Same for 2nd column //
            movups		xmm1, xmm0
            shufps		xmm1, xmm1, 0x55
            mulps		xmm1, xmm5
            addps		xmm2, xmm1

            // 3rd column //
            movups		xmm1, xmm0
            shufps		xmm1, xmm1, 0xAA
            mulps		xmm1, xmm6
            addps		xmm2, xmm1

            // 4th column //
            movups		xmm1, xmm0
            shufps		xmm1, xmm1, 0xFF
            mulps		xmm1, xmm7
            addps		xmm2, xmm1

            // Broadcast result to edi (out) //
            movups		[edi], xmm2
        }

    // ATT Syntax //
    #else
/*
        asm(".intel_syntax noprefix");
        asm("mov    esi, r");
        asm("mov    edi, out");
        asm("mov    edx, r0");
        asm("movups xmm4, [edx]");
        asm("movups xmm5, [edx + 0x10]");
        asm("movups xmm6, [edx + 0x20]");
        asm("movups xmm7, [edx + 0x30]");
        asm("movups xmm0, [esi]");
        asm("xorps  xmm2, xmm2");
        asm("movups xmm1, xmm0");
        asm("shufps xmm1, xmm1, 0x00");
        asm("mulps  xmm1, xmm4");
        asm("addps  xmm2, xmm1");
        asm("movups xmm1, xmm0");
        asm("shufps xmm1, xmm1, 0x55");
        asm("mulps  xmm1, xmm5");
        asm("addps  xmm2, xmm1");
        asm("movups xmm1, xmm0");
        asm("shufps xmm1, xmm1, 0xAA");
        asm("mulps  xmm1, xmm6");
        asm("addps  xmm2, xmm1");
        asm("movups xmm1, xmm0");
        asm("shufps xmm1, xmm1, 0xFF");
        asm("mulps  xmm1, xmm7");
        asm("addps  xmm2, xmm1");
        asm("movups [edi], xmm2");
        asm(".att_syntax noprefix");
*/
    #endif

}


// Brute force 4x4 matrix to vector mul //
static void mat4_mul_vec4(const mat4 l, const vec4 r, vec4 output)
{
//	if(CPUSSE2)
//	{
//		mat4_mul_vec4SSE(l, r, out);
//		return;
//	}

	output[0] = l[0] * r[0] + l[4] * r[1] + l[8] * r[2] + l[12] * r[3];
	output[1] = l[1] * r[0] + l[5] * r[1] + l[9] * r[2] + l[13] * r[3];
	output[2] = l[2] * r[0] + l[6] * r[1] + l[10] * r[2] + l[14] * r[3];
	output[3] = l[3] * r[0] + l[7] * r[1] + l[11] * r[2] + l[15] * r[3];
}



static void mat4_load_lookat_rh(vec3 pos, vec3 lookAt, vec3 up, mat4 output)
{
    // Find normalized view vector //
    vec3 z, x, y;
    vec3_sub(lookAt, pos, z);
    vec3_normalize(z);

    // Construct x axis from Z X UP //
    vec3_cross(z, up, x);
    vec3_normalize(x);

    // Cam y then becomes already normalized X X Z //
    vec3_cross(x, z, y);

    z[0] = -z[0];
    z[1] = -z[1];
    z[2] = -z[2];
    vec3 ne = {-pos[0], -pos[1], -pos[2]};

    output[0] = x[0];        output[4] = x[1];    output[8] = x[2];        output[12] = vec3_dot(ne, x);
    output[1] = y[0];        output[5] = y[1];    output[9] = y[2];        output[13] = vec3_dot(ne, y);
    output[2] = z[0];        output[6] = z[1];    output[10] = z[2];       output[14] = vec3_dot(ne, z);
    output[3] = 0.0F;        output[7] = 0.0F;    output[11] = 0.0F;       output[15] = 1.0F;
}



static void mat4_load_perspective_rh(float fovy, float aspect, float nearClip, float farClip, mat4 output)
{

    float iTan = 1.0F / tanf(deg_2_rad(fovy) * 0.5F);
    float dNF = (nearClip - farClip);
    
    output[0] = iTan / aspect;     output[4] = 0.0F;      output[8] = 0.0F;
    output[1] = 0.0F;              output[5] = iTan;      output[9] = 0.0F;
    output[2] = 0.0F;              output[6] = 0.0F;      output[10] = (farClip + nearClip) / dNF;
    output[3] = 0.0F;              output[7] = 0.0F;      output[11] = -1.0F;

    output[12] = 0.0F;
    output[13] = 0.0F;
    output[14] = (2.0F* farClip * nearClip) / dNF;
    output[15] = 0.0F;

}



//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= QUATERNION ROUTINES -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//



// Quaternion rotation //
static void quat_rotate(quat q, const float& rad, vec3 axis)
{
	if((axis[0] != 0 && axis[0] != 1) || (axis[1] != 0 && axis[1] != 1) || (axis[2] != 0 && axis[2] != 1))
		vec3_normalize(axis);

	float s = (float)sinf(rad * 0.5F);

	vec3 tm = {axis[0] * s, axis[1] * s, axis[2] * s};
	float w = (float)cosf(rad * 0.5F);
	float len = 1.0F / (float)sqrtf(tm[0] * tm[0] + tm[1] * tm[1] + tm[2] * tm[2] + w * w);
	tm[0] *= len;
	tm[1] *= len;
	tm[2] *= len;

	memcpy(q, tm, sizeof(float) * 3);
	q[3] = w;

}

// Quaternion multiplication //
static void quat_mul(quat l, quat r, quat output)
{
	output[0] = l[3] * r[0] + l[0] * r[3] + l[1] * r[2] - l[2] * r[1];
	output[1] = l[3] * r[1] - l[0] * r[2] + l[1] * r[3] + l[2] * r[0];
	output[2] = l[3] * r[2] + l[0] * r[1] - l[1] * r[0] + l[2] * r[3];
	output[3] = l[3] * r[3] - l[0] * r[0] - l[1] * r[1] - l[2] * r[2];
}

// Quaternion conjugation //
static void quat_conjugate(quat q)
{
	q[0] = -q[0]; q[1] = -q[1]; q[2] = -q[2]; q[3] = q[3];
}




//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= VISIBILITY DETERMINATION -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//




// Is point in AABB //
static bool point_in_aabb(const vec3 p, const vec3 mins, const vec3 maxs)
{
	return (!((p[0] <= mins[0] || p[1] <=  mins[1] || p[2] <= mins[2]) ||
		      (p[0] >= maxs[0] || p[1] >= maxs[1] || p[2] >= maxs[2])));
}


// Is point in Sphere //
// p: point to test
// sV: sphere center
// rad: sphere radius
static bool point_in_sphere(const vec3 p, const vec3 sV, const float rad)
{
	vec3 vPS;
	vec3_sub(p, sV, vPS);
	if(vec3_length(vPS) > rad)
		return false;
	
	return true;
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= GEOMETRY MISC -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=--=-=-=-//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

static void create_vertex_normals(vec3* verts, unsigned int nVerts, unsigned int* polys, unsigned int nPolys, float* norms)
{
	unsigned int* tmp;
	vec3 a, b, c, tmpNorm;
	vec3 tmpList[64];
	int counter;

	// iterate through every vertex //
	for(unsigned int j = 0; j < nVerts; ++j)
	{
		counter = 0;

		// check every poly for this vert //
		for(unsigned int v = 0; v < nPolys; ++v)
		{
			tmp = (unsigned int*)&polys[v * 3];
			if(*tmp != j)
			{
				++tmp;
				if(*tmp != j)
				{
					++tmp;
					if(*tmp != j)
						continue;
				}
			}

			// if we're here, this vert is in the tri, so fetch verts //
			unsigned int i1, i2, i3;
			i1 = polys[v * 3];
			i2 = polys[v * 3 + 1];
			i3 = polys[v * 3 + 2];

			a[0] = verts[i1][0];
			a[1] = verts[i1][1];
			a[2] = verts[i1][2];
			b[0] = verts[i2][0];
			b[1] = verts[i2][1];
			b[2] = verts[i2][2];
			c[0] = verts[i3][0];
			c[1] = verts[i3][1];
			c[2] = verts[i3][2];

			vec3 ab, ac;
			vec3_sub(a, c, ac);
			vec3_sub(a, b, ab);

			vec3_cross(ac, ab, tmpNorm);
			vec3_normalize(tmpNorm);

			// check through the buffer list for this vert and look for dupes //
			for(int z = 0; z < counter; ++z)
			{
				if(tmpNorm == tmpList[z])
					goto end;
			}

			// add to buffer list //
			tmpList[counter][0] = tmpNorm[0];
			tmpList[counter][1] = tmpNorm[1];
			tmpList[counter][2] = tmpNorm[2];

			++counter;
			end:;
		}

		tmpNorm[0] = 0.0F;
		tmpNorm[1] = 0.0F;
		tmpNorm[2] = 0.0F;
		for(int q = 0; q < counter; ++q)
		{
			tmpNorm[0] += tmpList[q][0];
			tmpNorm[1] += tmpList[q][1];
			tmpNorm[2] += tmpList[q][2];
		}

		// copy to norm list //
		vec3_normalize(tmpNorm);
		norms[j * 3] = tmpNorm[0];
		norms[j * 3 + 1] = tmpNorm[1];
		norms[j * 3 + 2] = tmpNorm[2];
	}
}



//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
//				VECTOR CLASS DEFINITIONS					  //
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

class vec2f
{
	public:
	
		float x, y;
		
		vec2f();
		~vec2f(){}
		
		vec2f(const float* v);
		vec2f(const vec2f& v);
		vec2f(const float& nx, const float& ny);
		
		const inline void set(const float* v) { x = v[0]; y = v[1]; }
		const inline void set(const vec2f& v) { x = v.x; y = v.y; }
		const inline void set(const float& nx, const float& ny) { x = nx; y = ny; }
		
		const inline void operator*= (const float& s) { x *= s; y *= s; }
		const inline void operator/= (const float& s) { x /= s; y /= s; }
		const inline void operator+= (const vec2f& v) { x += v.x; y += v.y; }
		const inline void operator+= (const float* v) { x += v[0]; y += v[1]; }
		const inline void operator-= (const vec2f& v) { x -= v.x; y -= v.y; }
		const inline void operator-= (const float* v) { x -= v[0]; y -= v[1]; }
		const inline void operator=  (const vec2f& v) { x = v.x; y = v.y; }
		const inline void operator=  (const float* v) { x = v[0]; y = v[1]; }
		const inline bool operator== (const vec2f& v) { return ((x == v.x) && (y == v.y)); }
		const inline bool operator== (const float* v) { return ((x == v[0]) && (y == v[1])); }
		const inline bool operator!= (const vec2f& v) { return !(*this == v); }
		const inline bool operator!= (const float* v) { return !(*this == v); }
		
		
		float getLength();
		
		const inline float dotProd(const vec2f& v)
		{
			return ( x * v.x ) + ( y * v.y );
		}
		
		const inline vec2f getPerpVector()
		{
			return vec2f( -y, x );
		}
		
		const inline void normalize()
		{
			float iLen = 1.0F / getLength();
			x *= iLen;
			y *= iLen;
		}
	
	private:
};


class vec3f
{
	public:
	
		float x, y, z;
	
		vec3f();
		~vec3f(){}
		
		vec3f(const float* v);
		vec3f(const vec3f& v);
		vec3f(const float& nx, const float& ny, const float& nz);
		
		const inline void set(const float* v) { x = v[0]; y = v[1]; z = v[2]; }
		const inline void set(const vec3f& v) { x = v.x; y = v.y; z = v.z; }
		const inline void set(const float& nx, const float& ny, const float& nz) { x = nx; y = ny; z = nz; }
		
		const inline vec3f operator- () { return vec3f(-x, -y, -z); }
		const inline void operator*= (const float& s) { x *= s; y *= s; z *= s; }
		const inline void operator/= (const float& s) { float inv = 1.0F / s; x *= inv; y *= inv; z *= inv; }
		const inline void operator+= (const vec3f& v) { x += v.x; y += v.y; z += v.z; }
		const inline void operator+= (const float* v) { x += v[0]; y += v[1]; z += v[2]; }
		const inline void operator-= (const vec3f& v) { x -= v.x; y -= v.y; z -= v.z; }
		const inline void operator-= (const float* v) { x -= v[0]; y -= v[1]; z -= v[2]; }
		const inline void operator=  (const vec3f& v) { x = v.x; y = v.y; z = v.z; }
		const inline void operator=  (const float* v) { x = v[0]; y = v[1]; z = v[2]; }
		const inline bool operator== (const vec3f& v) { return ((x == v.x) && (y == v.y) && (z == v.z)); }
		const inline bool operator== (const float* v) { return ((x == v[0]) && (y == v[1]) && (z == v[2])); }
		const inline bool operator!= (const vec3f& v) { return !(*this == v); }
		const inline bool operator!= (const float* v) { return !(*this == v); }
		
		const inline vec3f lerp(const vec3f& op, const float& dv)
		{
			return vec3f(x + dv * (op.x - x), y + dv * (op.y - y), z + dv * (op.z - z));
		}

		float getLength();

		const inline float dotProd(const vec3f& v)
		{
			return ((x * v.x) + (y * v.y) + (z * v.z));
		}
		
		const inline void normalize()
		{
			float mag = 1.0F / getLength(); x *= mag; y *= mag; z *= mag;
		}
		
		const inline vec3f crossProd(const vec3f& v)
		{
			return vec3f((y * v.z - z * v.y), (z * v.x - x * v.z), (x * v.y - y * v.x));
		}
		
		const inline vec3f crossProd(const float* v)
		{
			return vec3f((y * v[2] - z * v[1]), (z * v[0] - x * v[2]), (x * v[1] - y * v[0]));
		}
		
		const inline float tripleScalar(const vec3f& v, const vec3f& w)
		{
			return ((y * v.z - z * v.y) * w.x + (z * v.x - x * v.z) * w.y + (x * v.y - y * v.x) * w.z);
		}
		
		const inline void	assignIfLess( const vec3f& v )
		{
			x = ( v.x < x ) ? v.x : x;
			y = ( v.y < y ) ? v.y : y;
			z = ( v.z < z ) ? v.z : z;
		}
		
		const inline void	assignIfGreater( const vec3f& v )
		{
			x = ( v.x > x ) ? v.x : x;
			y = ( v.y > y ) ? v.y : y;
			z = ( v.z > z ) ? v.z : z;
		}
	
	private:
};

class vec4f
{
	public:
		
		float x, y, z, w;
		
		vec4f();
		~vec4f(){}
		
		vec4f(const float* v);
		vec4f(const vec4f& v);
		vec4f(const float& nx, const float& ny, const float& nz, const float& nw);
		
		const inline void set(const float* v) { x = v[0]; y = v[1]; z = v[2]; w = v[3]; }
		const inline void set(const vec4f& v) { x = v.x; y = v.y; z = v.z; w = v.w; }
		const inline void set(const float& nx, const float& ny, const float& nz, const float& nw) { x = nx; y = ny; z = nz; w = nw; }
	
		const inline void operator=  (const vec4f& v) { x = v.x; y = v.y; z = v.z; w = v.w; }
		const inline void operator=  (const float* v) { x = v[0]; y = v[1]; z = v[2]; w = v[3]; }
		
		const inline void operator*= (const float& v) { x *= v; y *= v; z *= v; w *= w; }
	
	private:
};



static vec3f operator- ( const vec3f& l, const vec3f& r )
{
	return vec3f( l.x - r.x, l.y - r.y, l.z - r.z );
}


#endif
