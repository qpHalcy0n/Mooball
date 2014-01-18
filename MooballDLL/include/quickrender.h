#ifndef __QUICKRENDER_H_
#define __QUICKRENDER_H_




#include "extensions.h"
#include "quickimage.h"
#include "quickshader.h"
#include "quicklights.h"
#include "quickerror.h"
#include "quickmath.h"
#include "quickgeom.h"


#define		MAX_SHADERS		256


static void GetColorComponentsFromRGBA8( const unsigned int& color, unsigned char& r, unsigned char& g, unsigned char& b, unsigned char& a )
{
	r = ( unsigned char )( ( color & 0xFF000000 ) >> 24 );
	g = ( unsigned char )( ( color & 0x00FF0000 ) >> 16 );
	b = ( unsigned char )( ( color & 0x0000FF00 ) >> 8 );
	a = ( unsigned char )( ( color & 0x000000FF ) );
}


template <class T>
static int FindEmptySlot( const std::vector <T*>& vec )
{
	for( unsigned i = 0; i < vec.size(); ++i )
	{
		if( !vec[i] )
			return i;
	}

	return INVALID_HANDLE;
}



#endif