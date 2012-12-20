#include "quickmath.h"



static float FastISqrt( const float& a )
{
	float ha = 0.5F * a;
	float ret;
	int i = *(int*)&a;
	i = 0x5F3759D5 - ( i >> 1 );
	ret = *(float*)&i;
	ret = ret * ( 1.5F - ha * ret * ret );
	return ret;	
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
//					vec2f METHODS							  //
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
vec2f::vec2f()
{
	x = y = 0.0F;
}

vec2f::vec2f( const float* v )
{
	x = v[0];
	y = v[1];
}

vec2f::vec2f( const vec2f& v )
{
	x = v.x;
	y = v.y;
}

vec2f::vec2f( const float& nx, const float& ny )
{
	x = nx;
	y = ny;
}

float vec2f::getLength()
{
	return 1.0f / FastISqrt( x * x + y * y );
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
//					vec3f METHODS								  //
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
vec3f::vec3f()
{
	x = y = z = 0.0f;
}

vec3f::vec3f( const float* v )
{
	x = v[0];
	y = v[1];
	z = v[2];
}

vec3f::vec3f( const float& nx, const float& ny, const float& nz )
{
	x = nx;
	y = ny;
	z = nz;
}

vec3f::vec3f( const vec3f& v )
{
	x = v.x;
	y = v.y;
	z = v.z;
}

float vec3f::getLength()
{
	return 1.0f / FastISqrt( x * x + y * y + z * z );
}


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
//					vec4f METHODS								  //
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
vec4f::vec4f()
{
	x = y = z = w = 0.0f;
}

vec4f::vec4f( const float* v )
{
	x = v[0];
	y = v[1];
	z = v[2];
	w = v[3];
}

vec4f::vec4f( const vec4f& v )
{
	x = v.x;
	y = v.y;
	z = v.z;
	w = v.w;
}

vec4f::vec4f( const float& nx, const float& ny, const float& nz, const float& nw )
{
	x = nx;
	y = ny;
	z = nz;
	w = nw;
}