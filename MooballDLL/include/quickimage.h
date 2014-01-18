////////////////////////////////////////////////////////////////////////////////////
//
// QUICKIMAGE.H
//
// This header was adapted for the Mooball project 12/09
//
// This header contains stubs for image loading. It also contains routines for
// generating random samples for both light scatter and SSAO casts.
//
// Anything related to image loading can also be placed into this header
//
// see: http://googlecode.com/p/mooball
//
/////////////////////////////////////////////////////////////////////////////////////


#include "extensions.h"

#ifndef __QUICKIMAGE_H_
#define __QUICKIMAGE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <math.h>
#include <time.h>
#include "quickerror.h"
#include "quickmath.h"
#include "windowsvc.h"
#include "extensions.h"
#include "quickdraw.h"



#define gmin(a, b) ((a) < (b) ? (a) : (b))
#define gmax(a, b) ((a) > (b) ? (a) : (b))

#define IMG_UNDEFINED	0xE0
#define IMG_TGA			0xE1
#define IMG_DDS			0xE2
#define IMG_BMP			0xE3
#define IMG_RAW			0xE4

//image type (broader then what's below)
#define IMG_STANDARD   0xFE
#define IMG_COMPRESSED 0xFF

#define PIXEL_FORMAT_NONE			0x00
#define PIXEL_FORMAT_I8				0x01
#define PIXEL_FORMAT_IA8			0x02
#define PIXEL_FORMAT_RGB8			0x03
#define PIXEL_FORMAT_RGBA8			0x04
#define PIXEL_FORMAT_I16			0x05
#define PIXEL_FORMAT_IA16			0x06
#define PIXEL_FORMAT_RGB16			0x07
#define PIXEL_FORMAT_RGBA16			0x08
#define PIXEL_FORMAT_I16F			0x09
#define PIXEL_FORMAT_IA16F			0x0A
#define PIXEL_FORMAT_RGB16F			0x0B
#define PIXEL_FORMAT_RGBA16F		0x0C
#define PIXEL_FORMAT_I32F			0x0D
#define PIXEL_FORMAT_IA32F			0x0E
#define PIXEL_FORMAT_RGB32F			0x0F
#define PIXEL_FORMAT_RGBA32F		0x10
#define PIXEL_FORMAT_DEPTH16		0x11
#define PIXEL_FORMAT_DEPTH32		0x12
#define PIXEL_FORMAT_RGB332			0x13
#define PIXEL_FORMAT_RE8			0x14
#define PIXEL_FORMAT_R8				0x15
#define PIXEL_FORMAT_DXT1			0x16
#define PIXEL_FORMAT_DXT3			0x17
#define PIXEL_FORMAT_DXT5			0x18
#define PIXEL_FORMAT_ATI2N			0x19
#define PIXEL_FORMAT_ATI1N			0x1A





#define SAMPLE_NEAREST				0x00000001
#define SAMPLE_LINEAR				0x00000002
#define SAMPLE_BILINEAR				0x00000004
#define SAMPLE_TRILINEAR			0x00000008
#define SAMPLE_BILINEAR_ANISO		0x00000010
#define SAMPLE_TRILINEAR_ANISO		0x00000020

#define SAMPLE_WRAP					0x00000040
#define SAMPLE_CLAMP				0x00000080


#define MSAA_2_SAMPLES				0x00000100
#define MSAA_4_SAMPLES				0x00000200
#define MSAA_8_SAMPLES				0x00000400
#define MSAA_16_SAMPLES				0x00000800
#define CSAA_8_SAMPLES_NV			0x00001000
#define CSAA_16_SAMPLES_NV			0x00002000
#define CSAA_8Q_SAMPLES_NV			0x00004000
#define CSAA_16Q_SAMPLES_NV			0x00008000

#define TEXTURE_WRITE_ONLY			0x00010000
#define TEXTURE_READ_WRITE			0x00020000



static unsigned int fourcc(unsigned char c0, unsigned char c1, unsigned char c2, unsigned char c3)
{
	return((unsigned int)(c0 | (c1 << 8) | (c2 << 16) | (c3 << 24)));
}

static bool is_format_compressed(unsigned char fmt)
{
	return (fmt >= PIXEL_FORMAT_DXT1);
}

static unsigned int get_bytes_per_pixel(unsigned char fmt)
{
	if(fmt > PIXEL_FORMAT_RGBA32F)
		return 0;
	static const unsigned int bpp[] = {0, 1, 2, 3, 4, 2, 4, 6, 8, 2, 4, 6, 8, 4, 8, 12, 16, 24, 1};
	return bpp[fmt];
}

static unsigned int get_bytes_per_channel(unsigned char fmt)
{
	if(fmt > PIXEL_FORMAT_RGBA32F)
		return 0;

	static const unsigned int bpc[] = {0, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4};
	return bpc[fmt];
}

static unsigned int get_channel_count(unsigned char fmt)
{
	static const unsigned int channels[] = {0, 1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4, 1, 1, 3, 3, 4, 4};
	return channels[fmt];
}

static unsigned int get_bytes_per_block(unsigned char fmt)
{
	return (fmt == PIXEL_FORMAT_DXT1 || fmt == PIXEL_FORMAT_ATI1N) ? 8 : 16;
}

static bool is_float_format(unsigned char fmt)
{
	static const bool fl[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	return fl[fmt];
}



static unsigned int get_image_size(unsigned char fmt, unsigned int w, unsigned int h, unsigned int d, unsigned int nLevels)
{
	unsigned int width = w;
	unsigned int height = h;
	unsigned int depth = d;
	unsigned int levels = nLevels;

	bool isCubemap = (d == 0);
	if(isCubemap)
		depth = 1;

	int size = 0;
	while(levels)
	{
		if(is_format_compressed(fmt))
			size += ((width + 3) >> 2) * ((height + 3) >> 2) * depth;
		else
			size += width * height * depth;

		if(width == 1 && height == 1 && depth == 1)
			break;

		if(width > 1) width >>= 1;
		if(height > 1) height >>= 1;
		if(depth > 1) depth >>= 1;
		--levels;
	}

	if(is_format_compressed(fmt))
		size *= get_bytes_per_block(fmt);
	else
		size *= get_bytes_per_pixel(fmt);

	if(isCubemap)
		size *= 6;

	return size;
}


// Swap channel for pixel ordering
// "pixels" - src image
// "nPixels" - number of pixels in src
// "nChannels" - number of channels in source image
// "ch0" - left operand for channel swap
// "ch1" - right operand for channel swap
template <typename T>
static void swap_channel(T* pixels, int nPixels, int nChannels, int ch0, int ch1)
{
	printf("\nsizeof(T) == %d\n", sizeof(T));
	int pixelCount = nPixels;

	for(int i = nPixels; i != 0; i--)
	//do
	{
		T tmp = pixels[ch1];
		pixels[ch1] = pixels[ch0];
		pixels[ch0] = tmp;
		pixels += nChannels;
	}//while(--pixelCount);
}


// Mirror image over horizontal axis
// "pixels" - src image
// "height" - height (in pels)
// "lineWidthInBytes" - width (in bytes)
// "fmt" - source image pixel format as an unsigned char flag
template <typename T>
static void flip_image_vertical(T* pixels, int height, int lineWidthInBytes, unsigned char fmt)
{
    int topLine = 0;
    int bottomLine = height - 1;

    T* tmp = new T[lineWidthInBytes];
    for(topLine; topLine < bottomLine; topLine++, bottomLine--)
    {
        memcpy(tmp, pixels + topLine * lineWidthInBytes, lineWidthInBytes);
        memcpy(pixels + topLine * lineWidthInBytes, pixels + bottomLine * lineWidthInBytes, lineWidthInBytes);
        memcpy(pixels + bottomLine * lineWidthInBytes, tmp, lineWidthInBytes);
        //topLine++;
        //bottomLine--;
    }//while(topLine < bottomLine);

    if(is_format_compressed(fmt))
    {
		int bPerBlock = get_bytes_per_block(fmt);
		int lineWidth = lineWidthInBytes / bPerBlock;
		T* pBuf = pixels + (bPerBlock / 2);

		for(int i = 0; i < height; ++i)
		{
			for(int j = 0; j < lineWidth; ++j)
			{
				T tmp = *(pBuf);
				*(pBuf) = *(pBuf + 3);
				*(pBuf + 3) = tmp;

				tmp = *(pBuf + 1);
				*(pBuf + 1) = *(pBuf + 2);
				*(pBuf + 2) = tmp;

				pBuf += get_bytes_per_block(fmt);
			}
		}
    }
}

static bool is_power_of_2(unsigned int width, unsigned int height)
{
	unsigned int m;
    for(m = 1; m < width; m *= 2);

    if(m != width)
       return false;

    if(m != height)
       return false;

    return true;
};



// Generates mipmaps from source file.
// "w" - image width (in pels)
// "h" - image height (in pels)
// "d" - image depth (in pels)
// "c" - image channel count
template <typename T>
static void generate_mipmaps(T* src, T* dest, unsigned int w, unsigned int h, unsigned int d, unsigned int c)
{
//	if(w != h)
//		return;

	unsigned int xOff = (w < 2) ? 0 : c;
	unsigned int yOff = (h < 2) ? 0 : c * w;
	unsigned int zOff = (d < 2) ? 0 : c * w * h;

	for(unsigned int z = 0; z < d; z += 2)
	{
		for(unsigned int y = 0; y < h; y += 2)
		{
			for(unsigned int x = 0; x < w; x += 2)
			{
				for(unsigned int i = 0; i < c; ++i)
				{
					*dest++ = (src[0] + src[xOff] + src[yOff] + src[yOff + xOff] + src[zOff] +
							   src[zOff + xOff] + src[zOff + yOff] + src[zOff + yOff + xOff]) / 8;

					++src;
				}

				src += xOff;
			}

			src += yOff;
		}

		src += zOff;
	}
}

// Obtain mip map count from largest dimension
// "max" - Maximum texture dimension in pixels
static int get_mipmap_count_from_dim(int max)
{
	int i = 0;
	while (max > 0)
	{
		max >>= 1;
		i++;
	}

	return i;
}



////////////////////////////////////////////////////////////////////////////////////
//
// CImage
//
// Base class for all image file types. Encapsulates some of the more common
// functionality and queries for image formats
//
// The programmer's primary point of interaction will be with CTextureInterface,
// which is an abstraction layer above the GL
//
////////////////////////////////////////////////////////////////////////////////////


class CImage
{
	public:


		CImage();
		virtual ~CImage();

		/*
			Load

			Load image from file. fName should have extension and any directory paths
		*/
		virtual bool Load(const char* fName) { return true; }


		/*
			GetWidth/GetHeight

			Obtain image width and height (in pixels)
		*/
		const inline unsigned short GetWidth() { return m_imageWidth; }
		const inline unsigned short GetHeight() { return m_imageHeight; }
		const inline unsigned short GetDepth() { return m_depth; }


		/*
			GetWidth/GetHeight/GetDepth(int level)

			Obtain image width/height/depth at a mipmap level "level"
		*/
		unsigned short GetWidth(int level);
		unsigned short GetHeight(int level);
		unsigned short GetDepth(int level);


		/*
			GetSizeWithMipmaps

			Return the image size complete with all mipmap levels
		*/
		unsigned int GetSizeWithMipmaps(unsigned int first = 0, unsigned int nLevels = 127, unsigned char fmt = PIXEL_FORMAT_NONE);

		/*
			GenerateMipmaps

			Generates the desired number of mipmaps. By default this will generate all mipmaps for an image.
			This will only work for non-compressed and square textures. Returns false on fail.
		*/
		bool		GenerateMipmaps(unsigned int nMipmaps = 127);


		/*
			GenerateColorTexture

			Generates a 2x2 color texture with the color specified
			The color should be in RGBA order.
		*/
		bool		GenerateColorTexture( const unsigned int& color, unsigned int srcFmt );


		/*
			GetBitDepth

			Obtain the image bit depth (in bits)
		*/
		const inline unsigned short GetBitDepth() { return m_imageBitDepth; }

		/*
			GetPixelFormat

			Obtain the image pixel format.
			Defined in the macros above this class def
		*/
		const inline unsigned char GetPixelFormat() { return m_pixelFormat; }

		/*
			GetFileName

			Obtain the original image file name
		*/
		const inline std::string GetFileName() { return m_fileName; }

		/*
			GetPixels

			Obtain raw pixel buffer. Can be used to pass to glTexImage
			returns raw byte buffer.
		*/
		const inline unsigned char*	GetPixels() { return m_pPixels; }


		/*
			GetPixels(int level)

			Obtain the raw pixel buffer at a given mip map level.
			This can be passed to glTexImage, minding the level parameter
		*/
		unsigned char* GetPixels(int level) const;

		/*
			IsLoaded

			Query the status of the image.
			True if ready for the GL, false if not
		*/
		const inline bool IsLoaded() { return m_bIsLoaded; }

		const inline short ImageType() { return m_imageType; }

		/*
			NumMipmaps

			Get the number of mipmaps in the image
		*/
		const inline int NumMipmaps() { return m_mipMapCount; }

		/*
			IsCubemap

			Query that the texture is a cubemap
		*/
		bool	IsCubemap();

		virtual unsigned short GetGLFormat();

	protected:
	
		unsigned short		m_imageType;				//TGA, DDS, etc
		unsigned short		m_imageWidth;				// Image width (in pixels)
		unsigned short		m_imageHeight;				// Image height (in pixels)
		unsigned short		m_imageBitDepth;			// Image bit depth (in bits per pel)

		unsigned short		m_depth;					// Layer depth
		unsigned short		m_mipMapCount;				// Number of mip maps present

		unsigned char		m_pixelFormat;				// Image pixel format

		std::string			m_fileName;					// Image file name w/ path

		bool				m_bIsLoaded;				// Is the image ready?

		unsigned char*		m_pPixels;
	private:
};


///////////////////////////////////////////////////////////////////////////////
//
// CBitmap
//
// Bitmap (.BMP) format file loader.
// Loads .BMP's of raw format into a pixel buffer which can be queried and
// passed to the GL.
//
////////////////////////////////////////////////////////////////////////////////



// BMP file format header //
typedef struct
{
	unsigned short		type;
	unsigned int		size;           // ulong
	unsigned short		res1;
	unsigned short		res2;
	unsigned int		offBits;        // ulong
} s_bitmap_header;


// BMP info sub-header //
typedef struct
{
	unsigned int		bmpSize;                    // ulong
	int 				bmpWidth;                   // long
	int 				bmpHeight;                  // long
	unsigned short		bmpPlanes;
	unsigned short		bmpBitDepth;
	unsigned int		bmpCompression;             // ulong
	unsigned int		bmpImageSize;               // ulong
	int 				bmpXPixelsPerMeter;         // long
	int 				bmpYPixelsPerMeter;         // long
	unsigned int		bmpClrUsed;                 // ulong
	unsigned int		bmpClrImportant;            // ulong
} s_bitmap_info_header;


// BMP pixel encapsulation //
typedef struct
{
	unsigned char	b;
	unsigned char	g;
	unsigned char	r;
	unsigned char	reserved;
} s_bitmap_rgb;



/*
    CBitmap

    BMP File object loader.

    Facilitates essential bitmap loading, unloading, as well as
    pixel, bpp, and size queries
*/
class CBitmap : public CImage
{
	public:

		CBitmap();
		~CBitmap();


		/*
			Load

			Load the image specified by "fName". This should include extension
			and any path information. Must have .BMP extension

			Returns true on success false on fail
		*/
		bool	Load(const char* fName);




        /*
            GetGLFormat

			Obtains the OpenGL format specifier for glTexImage
        */
		unsigned short      GetGLFormat();

	private:


};



//////////////////////////////////////////////////////////////////////////////////////////
//
// CTarga
//
// Supports loading of uncompressed .TGA file formats
//
//////////////////////////////////////////////////////////////////////////////////////////



/*
	TGA File Header
*/

// Pack header tightly //
#pragma pack(push, 1)

typedef struct
{
	unsigned char		descLen;
	unsigned char		cmapType;
	unsigned char		imgType;
	unsigned short		cmapStart;
	unsigned short		cmapEntries;
	unsigned char		cmapBits;
	unsigned short		xOffset;
	unsigned short		yOffset;
	unsigned short		width;
	unsigned short		height;
	unsigned char		bpp;
	unsigned char		attrib;
} s_tga_header;

#pragma pack(pop)


/*
	CTarga

	Child of CImage which is responsible for loading and passing information
	to CImage about a .TGA image format
*/

class CTarga : public CImage
{
	public:

		CTarga();
		~CTarga();


		/*
			Load

			Load the image specified by "fName". This should include extension
			and any path information. Must have .TGA extension

			Returns true on success false on fail
		*/
		bool	Load(const char* fName);




		/*
			GetGLFormat

			Get the GL internal format for the image
		*/
		unsigned short GetGLFormat();

	private:

};



//////////////////////////////////////////////////////////////////////////////////////
//
// CS3TC
//
// Supports loading of compressed DXT1, DXT3, and DXT5 .DDS encoded
// image file formats
//
//////////////////////////////////////////////////////////////////////////////////////



/*
	DDS File header
*/
struct s_dds_header
{
	unsigned int		dwMagic;
	unsigned int		dwSize;
	unsigned int		dwFlags;
	unsigned int		dwHeight;
	unsigned int		dwWidth;
	unsigned int		dwPitchOrLinearSize;
	unsigned int		dwDepth;
	unsigned int		dwMipMapCount;
	unsigned int		dwReserved[11];

	struct
	{
		unsigned int		dwSize;
		unsigned int		dwFlags;
		unsigned int		dwFourCC;
		unsigned int		dwRGBBitCount;
		unsigned int		dwRBitMask;
		unsigned int		dwGBitMask;
		unsigned int		dwBBitMask;
		unsigned int		dwRGBAlphaBitMask;
	} s_ddpf_pixel_format;

	struct
	{
		unsigned int		dwCaps1;
		unsigned int		dwCaps2;
		unsigned int		reserved[2];
	} s_dds_caps;

	unsigned int		dwReserved2;

};


class CS3TC : public CImage
{
	public:

		CS3TC();
		~CS3TC();

		/*
			Load

			Load the image specified by "fName". This should include extension
			and any path information. Must have .DDS extension.

			Returns true on success false on fail
		*/
		bool	Load(const char* fName);
};


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

struct s_gl_texture
{
	s_gl_texture();
	~s_gl_texture();

	GLuint			id;
	GLuint			depthID;
	GLuint			stencilID;
	GLenum			target;
	unsigned int	width;
	unsigned int	height;
	unsigned int	depth;
	unsigned int	flags;
	bool			isCubemap;
	bool			hasMipMaps;
	unsigned int	colorSamples;
	unsigned int	coverageSamples;
	unsigned char   pixelFormat;
	std::string		fileName;
};


class CTextureInterface
{
	public:

		CTextureInterface();
		~CTextureInterface();

		/*
			AddTexture

			Add a texture to the GL via file name.
			Texture persists in memory until explicitly deleted with DeleteTexture.

			Returns local handle to texture. This handle WILL NOT BE VALID for GL calls.

			"fName" - file name of texture with path and extension
			"imgFlags" (optional) - specify sampler filtering and wrap modes
		*/
		int			AddTexture(const char* fName, unsigned int imgFlags = 0);
		
		
		/* 
			AddTexture
			
			Add a texture to the GL via raw data. 
			Same assumptions apply as the other AddTexture call.
		*/
		int			AddTexture(const unsigned int width, const unsigned int height, const unsigned int pixelFormat, void* buf, unsigned int imgFlags = 0);
		


		/*
			AddTexture

			Add a texture to the GL via an already created CImage object
		*/
		int			AddTexture( CImage* img, unsigned int imgFlags = 0 );


		/*
			AddRenderTarget

			Add a renderable texture to the GL.
			Render target persists until the memory is explicitly deleted with DeleteTexture.

			Returns local handle to render target. This handle WILL NOT be valid for GL calls

			"width" - width of render target (pixels)
			"height"- height of render target (pixels)
			"fmt" - pixel format of render target
			"imgFlags" (optional) - specify sampler filtering and wrap modes
		*/
		int			AddRenderTarget(unsigned int width, unsigned int height, unsigned char fmt, unsigned int imgFlags = 0);



		/*
			AddDepthStencilTarget

			Add a renderable Depth/Stencil surface to the GL.
			Same rules of persistence and handles that apply to "AddRenderTarget" apply to AddDepthStencilTarget

			"width" - width of d/s target (pixels)
			"height" - width of d/s target (pixels)
			"depthBits" - depth buffer resolution (16, 24, or 32)
			"stencilBits" - stencil buffer resolution (8 by default, 0 means no stencil buffer)
			"imgFlags" (optional) - Not currently used
		*/
		int			AddDepthStencilTarget(unsigned int width, unsigned int height, unsigned int depthBits, unsigned int stencilBits, unsigned int imgFlags = 0);


		/*
			BindRenderTarget

			Given handles to either/or color targets or depth stencil surfaces created from "AddRenderTarget" and/or "AddDepthStencilTarget"
			will bind those targets as the current render target.

			If more color targets are passed than the device supports, it will use the most that it can support. Use GetMaxMRT()

			"colorTargets" - list of color target handles (up to GetMaxMRT())
			"nColorTargets" - number of handles in "colorTargets"
			"dsTarget" - single depth/stencil target (can be INVALID_HANDLE or DEFAULT_HANDLE to use the default Depth/Stencil buffer)
		*/
		bool		BindRenderTarget(int* colorTargets, int nColorTargets, int dsTarget);



		/*
			DeleteTexture

			Explicitly deletes a texture by handle returned from "AddTexture".

			"handle" - local handle returned from "AddTexture"
		*/
		void		DeleteTexture(int handle);

		/*
			BindTexture

			Binds texture to the current state until forcefully evicted.

			"handle" - local handle returned from "AddTexture"
			"texUnit" (optional) - texture unit to be bound to
		*/
		void		BindTexture(int handle, unsigned int texUnit = 0);
		
		
		/* 
			ReadTexture
			
			Reads a texture back from memory into client memory.
			
			"handle" - local handle returned from "AddTexture" or "AddRenderTarget"
			"buf" - pointer to output buffer. This buffer must be appropriately sized
		*/
		void		ReadTexture(int handle, void* buf);
		
		
		/*
			ResolveMSAATarget
			
			Resolves an MSAA'd render target to another buffer so that it can be either blitted to the screen or
			sampled from a shader. This uses older functionality (glBlitFramebuffer)
			
			
			"msaaColor" - local handle to MSAA color target (source buffer)
			"msaaDepth" - local handle to MSAA depth/stencil surface (source buffer)
			"destHandle" - local handle to output (must be non-msaa) (dest buffer)
		*/
		void		ResolveMSAATarget(const int destHandle);
		

		/*
			EvictTextures

			Forcefully evict all textures
		*/
		void		EvictTextures();


		/*
			SetGlobalAnisotropy

			Sets the global anisotropic filter level. This applies to all textures specified
			by SAMPLE_BILINEAR_ANISO or SAMPLE_TRILINEAR_ANISO

			"a" - new level
		*/
		void		SetGlobalAnisotropy(GLint a);



		/*
			GetSamplerResidency

			Obtain the closest texture unit that is available for "handle". If "handle" is
			already bound to a texture unit, that one is returned, otherwise the next available is returned

			If all texture units are full, it WILL forcefully evict a texture and return a good tex unit.

			"handle" - texture handle
		*/
		int 		GetSamplerResidency(int handle);


		/*
			BroadcastSamplerOccupation

			Sets the current texture sampler occupation to be that of "handle"

			"sampler" - which sampler is to be set
			"handle" - handle which occupies "sampler"
		*/
		const inline void		BroadcastSamplerOccupation(int sampler, int handle) { m_texUnits[sampler] = handle; }

		/*
			GetSamplerOccupation

			Obtain the handle that occupies a particular sampler index.

			"sampler" - index of the sampler to query (should be between 0 and GetMaxMRT())
		*/
		const inline int		GetSamplerOccupation(int sampler) { return m_texUnits[sampler]; }


		/*
			GetGLHandle
			
			Obtain the GL handle for the client given handle from AddTextureObject/AddRenderTarget/AddDepthStencilTarget
		
			"handle" - handle given from any of the above functions
		*/
		GLuint			GetGLHandle(int handle);

		/*
			GetMaxMRT

			Obtain the maximum number of multiple render targets bound at a time
		*/
		const inline int		GetMaxMRT() { return m_maxMRT; }
		
		
		/*
			GetMaxMSAA
			
			Obtain the maximum amount of MSAA samples supported by hardware.
		*/
		const inline GLint		GetMaxMSAA() { return m_maxMSAASamples; }


		/*
			DebugRender

			Renders the image fullscreen. Must be called within an appropriate render state.
		*/
		bool			DebugRender( int tex );


	private:

		unsigned int			m_nLatentMRT;					// Number of latent MRT residence for drivers that enforce
																// strict FBO format compliance
		
		int			m_maxMSAASamples;							// Maximum number of samples for MSAA targets

		int			m_noShaderHandle;							// Default no shader/no texture texture

		int			m_defaultFBWidth;
		int			m_defaultFBHeight;
		int			m_curDSTarget;
		int*		m_curColorTargets;

		int				m_maxMRT;									// Maximum number of render targets bound at a time
		GLint			m_maxTextureUnits;							// Maximum system allowable texture units
		GLfloat			m_maxAnisotropy;							// Maximum system supported anisotropic level
		GLint			m_globalAnisotropy;							// Current anisotropic level

		int*		m_texUnits;									// Current texture unit state
		std::vector <s_gl_texture>		m_textureObjects;		// Texture object list

		GLuint		m_curFBO;									// Current FBO object for rt's
		GLuint		m_scratchFBO;								// Scratch buffer object for async reads
		GLuint		m_scratchPBO;								// Pixel buffer object for async reads
};

extern CTextureInterface*	g_pTexInterface;



// Handy dandy fun time //
static int generate_random_ray_texture(int nRaysX, int nRaysY)
{
	unsigned char* buf = (unsigned char*)malloc(nRaysX * nRaysY * 3);
	unsigned char* pBuf = buf;

	vec3 rVec;
	
	int scrWidth = GetScreenWidth();
	int scrHeight = GetScreenHeight();

	for(int i = 0; i < nRaysX; ++i)
	{
		for(int j = 0; j < nRaysY; ++j)
		{
			rVec[0] = rand() % scrWidth / (float)scrWidth;
			rVec[1] = rand() % scrHeight / (float)scrHeight;
			rVec[2] = rand() % 2;
			
			*pBuf++ = (unsigned char)(rVec[0] * 255);
			*pBuf++ = (unsigned char)(rVec[1] * 255);
			*pBuf++ = (unsigned char)(rVec[2] * 255);
		}
	}
	
	return g_pTexInterface->AddTexture(nRaysX, nRaysY, PIXEL_FORMAT_RGB8, buf, SAMPLE_NEAREST);
	free(buf);
}


#endif
