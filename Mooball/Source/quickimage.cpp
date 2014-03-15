#include "quickimage.h"



const unsigned int DDPF_ALPHAPIXELS = 0x00000001;
const unsigned int DDPF_FOURCC      = 0x00000004;
const unsigned int DDPF_RGB         = 0x00000040;
const unsigned int DDSD_CAPS        = 0x00000001;
const unsigned int DDSD_HEIGHT      = 0x00000002;
const unsigned int DDSD_WIDTH       = 0x00000004;
const unsigned int DDSD_PITCH       = 0x00000008;
const unsigned int DDSD_PIXELFORMAT = 0x00001000;
const unsigned int DDSD_MIPMAPCOUNT = 0x00020000;
const unsigned int DDSD_LINEARSIZE  = 0x00080000;
const unsigned int DDSD_DEPTH       = 0x00800000;
const unsigned int DDSCAPS_COMPLEX  = 0x00000008;
const unsigned int DDSCAPS_TEXTURE  = 0x00001000;
const unsigned int DDSCAPS_MIPMAP   = 0x00400000;
const unsigned int DDSCAPS2_VOLUME  = 0x00200000;
const unsigned int DDSCAPS2_CUBEMAP = 0x00000200;
const unsigned int DDSCAPS2_CUBEMAP_POSITIVEX = 0x00000400;
const unsigned int DDSCAPS2_CUBEMAP_NEGATIVEX = 0x00000800;
const unsigned int DDSCAPS2_CUBEMAP_POSITIVEY = 0x00001000;
const unsigned int DDSCAPS2_CUBEMAP_NEGATIVEY = 0x00002000;
const unsigned int DDSCAPS2_CUBEMAP_POSITIVEZ = 0x00004000;
const unsigned int DDSCAPS2_CUBEMAP_NEGATIVEZ = 0x00008000;
#define DDSCAPS2_CUBEMAP_ALL_FACES (DDSCAPS2_CUBEMAP_POSITIVEX | DDSCAPS2_CUBEMAP_NEGATIVEX | DDSCAPS2_CUBEMAP_POSITIVEY | DDSCAPS2_CUBEMAP_NEGATIVEY | DDSCAPS2_CUBEMAP_POSITIVEZ | DDSCAPS2_CUBEMAP_NEGATIVEZ)


static GLint gl_internal_formats[] =
{
	0,
	GL_INTENSITY8,
	GL_LUMINANCE8_ALPHA8,
	GL_RGB8,
	GL_RGBA8,
	GL_INTENSITY16,
	GL_LUMINANCE16_ALPHA16,
	GL_RGB16,
	GL_RGBA16,
	GL_INTENSITY16F_ARB,
	GL_LUMINANCE_ALPHA16F_ARB,
	GL_RGB16F_ARB,
	GL_RGBA16F_ARB,
	GL_INTENSITY32F_ARB,
	GL_LUMINANCE_ALPHA32F_ARB,
	GL_RGB32F_ARB,
	GL_RGBA32F_ARB,
	GL_DEPTH_COMPONENT16,
	GL_DEPTH_COMPONENT32,
	GL_R3_G3_B2,
	GL_LUMINANCE8_ALPHA8,
	GL_INTENSITY8,
	GL_COMPRESSED_RGB_S3TC_DXT1_EXT,
	GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
	GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
	0,
	0,
};

static GLenum gl_formats[] =
{
	0,
	GL_LUMINANCE,
	GL_LUMINANCE_ALPHA,
	GL_RGB,
	GL_RGBA,
	GL_LUMINANCE,
	GL_LUMINANCE_ALPHA,
	GL_RGB,
	GL_RGBA,
	GL_LUMINANCE,
	GL_LUMINANCE_ALPHA,
	GL_RGB,
	GL_RGBA,
	GL_LUMINANCE,
	GL_LUMINANCE_ALPHA,
	GL_RGB,
	GL_RGBA,
	GL_DEPTH_COMPONENT,
	GL_DEPTH_COMPONENT,
	GL_RGB,
	GL_LUMINANCE_ALPHA,
	GL_LUMINANCE,
	0,
	0,
	0,
	0,
	0,
};

static GLenum gl_types[] =
{
	0,
	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_SHORT,
	GL_UNSIGNED_SHORT,
	GL_UNSIGNED_SHORT,
	GL_UNSIGNED_SHORT,
	GL_HALF_FLOAT_ARB,
	GL_HALF_FLOAT_ARB,
	GL_HALF_FLOAT_ARB,
	GL_HALF_FLOAT_ARB,
	GL_FLOAT,
	GL_FLOAT,
	GL_FLOAT,
	GL_FLOAT,
	GL_UNSIGNED_SHORT,
	GL_UNSIGNED_INT,
	GL_UNSIGNED_BYTE_3_3_2,
	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE,
	0,
	0,
	0,
	0,
	0,
};


static GLint gl_min_filters[] =
{
	GL_NEAREST,							// Nearest
	GL_LINEAR,							// Linear
	GL_LINEAR_MIPMAP_NEAREST,			// Bilinear
	GL_LINEAR_MIPMAP_LINEAR,			// Trilinear
	GL_LINEAR_MIPMAP_NEAREST,			// Bilinear Aniso
	GL_LINEAR_MIPMAP_LINEAR,			// Trilinear Aniso
};

static GLint gl_mag_filters[] =
{
	GL_NEAREST,
	GL_LINEAR,
	GL_LINEAR,
	GL_LINEAR,
	GL_LINEAR,
	GL_LINEAR,
};


CImage::CImage()
{
	m_imageType = IMG_UNDEFINED;
	m_imageWidth = 0;
	m_imageHeight = 0;
	m_imageBitDepth = 0;

	m_depth = 0;
	m_mipMapCount = 0;

	m_bIsLoaded = false;

	m_pPixels = NULL;

	m_pixelFormat = PIXEL_FORMAT_NONE;
}

CImage::~CImage()
{
	m_imageWidth = 0;
	m_imageHeight = 0;
	m_imageBitDepth = 0;

	m_depth = 0;
	m_mipMapCount = 0;

	m_bIsLoaded = false;

	m_pixelFormat = PIXEL_FORMAT_NONE;
}


unsigned short CImage::GetWidth(int level)
{
	int a = m_imageWidth >> level;
	return (a == 0) ? 1 : a;
}

unsigned short CImage::GetHeight(int level)
{
	int a = m_imageHeight >> level;
	return (a == 0) ? 1 : a;
}

unsigned short CImage::GetDepth(int level)
{
	int a = m_depth >> level;
	return (a == 0) ? 1 : a;
}


unsigned int CImage::GetSizeWithMipmaps(unsigned int first, unsigned int nLevels, unsigned char fmt)
{
	if(fmt == PIXEL_FORMAT_NONE)
		fmt = m_pixelFormat;

	if(nLevels == 127)
		nLevels = (m_mipMapCount == 0) ? 1 : m_mipMapCount;

	return get_image_size(fmt, GetWidth(first), GetHeight(first), (m_depth > 0) * GetDepth(first), nLevels);
}

unsigned char* CImage::GetPixels(int level) const
{
	int mipMappedSize;
	int w, h, d;
	w = m_imageWidth >> 0;
	if(w == 0) w = 1;
	h = m_imageHeight >> 0;
	if(h == 0) h = 1;
	d = m_depth >> 0;
	if(d == 0) d = 1;

	mipMappedSize = get_image_size(m_pixelFormat, w, h, (m_depth > 0) * d, level);
	return(level < m_mipMapCount) ? (unsigned char*)m_pPixels + mipMappedSize : NULL;
}

bool CImage::IsCubemap()
{
	return (m_depth == 0);
}

unsigned short CImage::GetGLFormat()
{
	switch(m_imageBitDepth)
	{
		case 8:
			return GL_LUMINANCE;

		case 16:
			return GL_RGBA;

		case 24:
			return GL_RGB;

		case 32:
			return GL_RGBA;

		default:
			return GL_RGBA;
	}
}

bool CImage::GenerateMipmaps(unsigned int nMipmaps)
{
	// Compressed formats should have mipmaps packed in //
	if(is_format_compressed(m_pixelFormat))
		return false;

	// We do not support non-square mipmapping at this time //
	if(!is_power_of_2(m_imageWidth, m_imageHeight))
		return false;


	m_mipMapCount = gmin(get_mipmap_count_from_dim(gmax(gmax(m_imageWidth, m_imageHeight), m_depth)), nMipmaps);
	m_pPixels = (unsigned char*)realloc(m_pPixels, GetSizeWithMipmaps());

	unsigned char *dest, *src = m_pPixels;
	int c = get_channel_count(m_pixelFormat);
	int cSize = get_bytes_per_channel(m_pixelFormat);
	if(IsCubemap())
	{
		int dim = m_imageWidth;
		for(int i = 1; i < m_mipMapCount; ++i)
		{
			int sliceSize = dim * dim * c * cSize;
			dest = src + 6 * sliceSize;

			for(unsigned int s = 0; s < 6; ++s)
			{
				if(cSize == 1)
					generate_mipmaps(src + s * sliceSize, dest + s * sliceSize / 4, dim, dim, 1, c);
				else if(cSize == 2)
					generate_mipmaps((unsigned short*)(src + s * sliceSize), (unsigned short*)(dest + s * sliceSize / 4), dim, dim, 1, c);
				else
					generate_mipmaps((float*)(src + s * sliceSize), (float*)(dest + s * sliceSize / 4), dim, dim, 1, c);
			}

			src = dest;
			dim >>= 1;
		}
	}

	else
	{
		int w = m_imageWidth;
		int h = m_imageHeight;
		int d = m_depth;
		for(int i = 1; i < m_mipMapCount; ++i)
		{
			dest = src + w * h * d * c * cSize;

			if(cSize == 1)
				generate_mipmaps(src, dest, w, h, d, c);
			else if(cSize == 2)
				generate_mipmaps((unsigned short*)src, (unsigned short*)dest, w, h, d, c);
			else
				generate_mipmaps((float*)src, (float*)dest, w, h, d, c);

			src = dest;
			if(w > 1) w >>= 1;
			if(h > 1) h >>= 1;
			if(d > 1) d >>= 1;
		}
	}

	return true;
}

bool CImage::GenerateColorTexture( const unsigned int& color, unsigned int srcFmt )
{
	m_imageWidth = 2;
	m_imageHeight = 2;
	m_imageType = IMG_RAW;
	m_imageBitDepth = 32;

	m_depth = 1;
	m_mipMapCount = 1;
	m_pixelFormat = PIXEL_FORMAT_RGBA8;
	m_fileName = "";

	m_pPixels = new unsigned char[16];
	for( unsigned i = 0; i < 16; i += 4 )
	{
		if( srcFmt == PIXEL_FORMAT_RGBA8 )
		{
			m_pPixels[i + 0] =  ( ( color & 0xFF000000 ) >> 24 );
			m_pPixels[i + 1] = ( ( color & 0x00FF0000 ) >> 16 );
			m_pPixels[i + 2] = ( ( color & 0x0000FF00 ) >> 8 );
			m_pPixels[i + 3] = 0xFF;
		}
	}

	m_bIsLoaded = true;
	return true;
}



CBitmap::CBitmap()
{
	m_imageType = IMG_BMP;
	m_pPixels = NULL;
}


CBitmap::~CBitmap()
{
	if(m_pPixels)
	{
		free(m_pPixels);
		m_pPixels = NULL;
	}
}



bool CBitmap::Load(const char* fName)
{
    // Read as binary file //
	FILE* bmpFile;
	if((bmpFile = fopen(fName, "rb")) == NULL)
		return false;

	// Read file header //
	s_bitmap_header fHeader;
	fread(&fHeader, 14, 1, bmpFile);

    // Verify header //
	if(fHeader.type != 19778)
	{
		fclose(bmpFile);
		return false;
	}

	// Read info header //
	s_bitmap_info_header fInfoHeader;
	fread(&fInfoHeader, 40, 1, bmpFile);

	m_imageWidth = fInfoHeader.bmpWidth;
	m_imageHeight = fInfoHeader.bmpHeight;
	m_imageBitDepth = fInfoHeader.bmpBitDepth;

	switch(m_imageBitDepth)
	{
		case 8:
			m_pixelFormat = PIXEL_FORMAT_R8;
			break;

		case 16:
			m_pixelFormat = PIXEL_FORMAT_NONE;
			fclose(bmpFile);
			return false;

		case 24:
			m_pixelFormat = PIXEL_FORMAT_RGB8;
			break;

		case 32:
			m_pixelFormat = PIXEL_FORMAT_RGBA8;
			break;

		default:
			break;
	}

	// Allocate pixel buffer //
	unsigned int totalSize = m_imageWidth * m_imageHeight * (m_imageBitDepth / 8);
	unsigned char* pix = (unsigned char*)malloc(totalSize);
	if(!pix)
	{
		fclose(bmpFile);
		return false;
	}

	// Read raw pix data //
	fread(pix, sizeof(unsigned char), totalSize, bmpFile);

	m_pPixels = (unsigned char*)malloc(totalSize);
	if(!m_pPixels)
	{
		free(pix); pix = NULL;
		fclose(bmpFile);
		return false;
	}

	fclose(bmpFile);
	memcpy(m_pPixels, pix, totalSize);

	// Swap to BGR/BGRA format //
	swap_channel(m_pPixels, m_imageWidth * m_imageHeight, get_channel_count(m_pixelFormat), 0, 2);

	// Cleanup and bail //
	free(pix); pix = NULL;

	m_fileName = fName;
	m_bIsLoaded = true;
	m_depth = 1;
	m_mipMapCount = 1;
	return true;
}

unsigned short CBitmap::GetGLFormat()
{
    // We will not be storing HDR data in bitmap form //
    // Format will not be GL_LUMINANCE //
    if(m_imageBitDepth == 8)
        return GL_RED;

     switch(m_imageBitDepth)
     {
        case 16:
            return GL_RGBA;
        case 24:
            return GL_BGR;
        case 32:
            return GL_BGRA;
		default:
			return GL_BGRA;
     };
}




CPNG::CPNG()
{
	m_imageType = IMG_PNG;
	m_pPixels = NULL;
}


CPNG::~CPNG()
{
	if(m_pPixels)
	{
		delete[] m_pPixels;
		m_pPixels = NULL;
	}
}


bool CPNG::Load(const char* fName)
{
	std::vector<unsigned char> img;
	unsigned int width, height;
	unsigned int err = lodepng::decode(img, width, height, fName);
	if(err != 0)
		return false;

	m_pPixels = new unsigned char[img.size()];
	if(!m_pPixels)
		return false;

	for(int i = 0; i < img.size(); ++i)
		m_pPixels[i] = img[i];

	m_imageWidth = width;
	m_imageHeight = height;
	m_imageBitDepth = (img.size() / (width * height)) * 8;

	switch(m_imageBitDepth)
	{
		case 8:
			m_pixelFormat = PIXEL_FORMAT_I8;
			break;

		case 16:
			m_pixelFormat = PIXEL_FORMAT_RGBA8;
			break;

		case 24:
			m_pixelFormat = PIXEL_FORMAT_RGB8;
			break;

		case 32:
			m_pixelFormat = PIXEL_FORMAT_RGBA8;
			break;

		default:
			break;
	}

	m_depth = 1;
	m_mipMapCount = 1;
	return true;
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CTarga::CTarga()
{
	m_imageType = IMG_TGA;
	m_pPixels = NULL;
}


CTarga::~CTarga()
{
	if(m_pPixels)
	{
		delete[] m_pPixels;
		m_pPixels = NULL;
	}
}

bool CTarga::Load(const char* fName)
{
	s_tga_header header;
	FILE* file;
	if((file = fopen(fName, "rb")) == NULL)
		return false;

	// Read header in //
	fread(&header, sizeof(s_tga_header), 1, file);

	// If the header has a descriptor, move to the end of it //
	if(header.descLen)
		fseek(file, header.descLen, SEEK_CUR);

	m_imageWidth = header.width;
	m_imageHeight = header.height;
	m_imageBitDepth = header.bpp;

	unsigned int pixelSize = header.bpp / 8;
	unsigned int size = m_imageWidth * m_imageHeight * pixelSize;

	unsigned char* pix = new unsigned char[size];
	if(!pix)
	{
		fclose(file);
		return false;
	}

	fread(pix, size, 1, file);
	fclose(file);

//	unsigned char* dest, *src = pix + m_imageWidth * (m_imageHeight - 1) * pixelSize;
	unsigned char* dest, *src = pix;
	unsigned char* newPix;
	unsigned int x, y;

	switch(m_imageBitDepth)
	{
		case 8:
			m_pixelFormat = PIXEL_FORMAT_I8;
			dest = newPix = new unsigned char[m_imageWidth * m_imageHeight];
			for(y = 0; y < m_imageHeight; ++y)
			{
				memcpy(dest, src, m_imageWidth);
				dest += m_imageWidth;
				src += m_imageWidth;
//				src -= m_imageWidth;
			}
			break;

		case 16:
			m_pixelFormat = PIXEL_FORMAT_RGBA8;
			dest = newPix = new unsigned char[m_imageWidth * m_imageHeight * 4];
			for(y = 0; y < m_imageHeight; ++y)
			{
				for(x = 0; x < m_imageWidth; ++x)
				{
					unsigned short tmpPix = *((unsigned char*)src);
					dest[0] = ((tmpPix >> 10) & 0x1F) << 3;
					dest[1] = ((tmpPix >> 5) & 0x1F) << 3;
					dest[2] = ((tmpPix) & 0x1F) << 3;
					dest[3] = ((tmpPix >> 15) ? 0xFF : 0);
					dest += 4;
					src += 4;
				}

//				src -= 4 * m_imageWidth;
			}

			break;

		case 24:
			m_pixelFormat = PIXEL_FORMAT_RGB8;
			dest = newPix = new unsigned char[m_imageWidth * m_imageHeight * 3];
			for(y = 0; y < m_imageHeight; ++y)
			{
				for(x = 0; x < m_imageWidth; ++x)
				{
					*dest++ = src[2];
					*dest++ = src[1];
					*dest++ = src[0];
					src += 3;
				}

 //               src -= 6 * m_imageWidth;
			}

			break;

		case 32:
			m_pixelFormat = PIXEL_FORMAT_RGBA8;
			dest = newPix = new unsigned char[m_imageWidth * m_imageHeight * 4];
			for(y = 0; y < m_imageHeight; ++y)
			{
				for(x = 0; x < m_imageWidth; ++x)
				{
					*dest++ = src[2];
					*dest++ = src[1];
					*dest++ = src[0];
					*dest++ = src[3];
					src += 4;
				}

//				src -= 8 * m_imageWidth;
			}

			break;
	}

	if(header.attrib & 0x20)
	{
		// No texture compression support //
		if(is_format_compressed(m_pixelFormat))
		{
			delete[] newPix;
			newPix = NULL;
			return false;
		}

		unsigned int lineWidth = get_bytes_per_pixel(m_pixelFormat) * m_imageWidth;
		unsigned char* newPixels = new unsigned char[lineWidth * m_imageHeight];
		if(!newPixels)
		{
			delete[] newPix;
			newPix = NULL;
			return false;
		}

		for(unsigned int i = 0; i < m_imageHeight; ++i)
		{
			memcpy(newPixels + i * lineWidth, newPix + (m_imageHeight - 1 - i) * lineWidth, lineWidth);
		}

		m_pPixels = newPixels;
	}

	else
		m_pPixels = newPix;

	delete[] pix;

	m_depth = 1;
	m_bIsLoaded = true;
	m_mipMapCount = 1;
	m_fileName = fName;

	return true;
}


unsigned short CTarga::GetGLFormat()
{
	switch(m_imageBitDepth)
	{
		case 8:
			return GL_LUMINANCE;

		case 16:
			return GL_RGBA;

		case 24:
			return GL_RGB;

		case 32:
			return GL_RGBA;

		default: 
			return GL_RGBA;
	}
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CS3TC::CS3TC()
{
	m_imageType = IMG_DDS;
	m_pPixels = NULL;
}

CS3TC::~CS3TC()
{
	if(m_pPixels)
	{
		delete[] m_pPixels;
		m_pPixels = NULL;
	}
}


bool CS3TC::Load(const char* fName)
{
	s_dds_header header;
	FILE* file;
	if((file = fopen(fName, "rb")) == NULL)
		return false;


	fseek(file, 0, SEEK_END);
	int fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);

	fread(&header, sizeof(s_dds_header), 1, file);
	if(header.dwMagic != fourcc('D', 'D', 'S', ' '))
	{
		fclose(file);
		return false;
	}

	m_imageWidth = header.dwWidth;
	m_imageHeight = header.dwHeight;
	m_depth = header.dwDepth;
	m_mipMapCount = header.dwMipMapCount;


	if(header.s_dds_caps.dwCaps2 & DDSCAPS2_CUBEMAP)
		m_depth = 0;

	else
	{
		if(m_depth == 0)
			m_depth = 1;
	}

	if(m_mipMapCount <= 0)
		m_mipMapCount = 1;

	switch(header.s_ddpf_pixel_format.dwFourCC)
	{
		case 0x31545844:
			m_pixelFormat = PIXEL_FORMAT_DXT1;
			break;

		case 0x33545844:
			m_pixelFormat = PIXEL_FORMAT_DXT3;
			break;

		case 0x35545844:
			m_pixelFormat = PIXEL_FORMAT_DXT5;
			break;

		case 0x32495441:
			m_pixelFormat = PIXEL_FORMAT_ATI2N;
			break;

		case 0x31495441:
			m_pixelFormat = PIXEL_FORMAT_ATI1N;
			break;

		default:
			switch(header.s_ddpf_pixel_format.dwRGBBitCount)
			{
				case 8:
					if(header.s_ddpf_pixel_format.dwRBitMask == 0xE0)
						m_pixelFormat = PIXEL_FORMAT_RGB332;
					else
						m_pixelFormat = PIXEL_FORMAT_I16;
					break;

				case 16:
					if(header.s_ddpf_pixel_format.dwRGBAlphaBitMask)
						m_pixelFormat = PIXEL_FORMAT_IA8;
					else
						m_pixelFormat = PIXEL_FORMAT_I16;
					break;

				case 24:
					m_pixelFormat = PIXEL_FORMAT_RGB8;
					break;

				case 32:
					m_pixelFormat = PIXEL_FORMAT_RGBA8;
					break;

				default:
					fclose(file);
					return false;
			}
	}

	int size = GetSizeWithMipmaps(0, m_mipMapCount);
	m_pPixels = new unsigned char[size];



	if(IsCubemap())
	{
		for(unsigned int face = 0; face < 6; ++face)
		{
			for(unsigned int mipLevel = 0; mipLevel < m_mipMapCount; ++mipLevel)
			{
				int faceSize = GetSizeWithMipmaps(mipLevel, 1) / 6;
				unsigned char* src = GetPixels(mipLevel) + face * faceSize;
				fread(src, 1, faceSize, file);
			}
		}
	}


	else
	{
		fread(m_pPixels, 1, size, file);

		int lineWidth = ((GetWidth() + 3) >> 2);
		int lineHeight = ((GetHeight() + 3) >> 2);
		lineWidth *= get_bytes_per_block(m_pixelFormat);
		flip_image_vertical(m_pPixels, lineHeight, lineWidth, m_pixelFormat);
	}


	if((m_pixelFormat == PIXEL_FORMAT_RGB8 || m_pixelFormat == PIXEL_FORMAT_RGBA8) &&
	    header.s_ddpf_pixel_format.dwBBitMask == 0xFF)
	{
		int nChannels = get_channel_count(m_pixelFormat);
		swap_channel(m_pPixels, size / nChannels, nChannels, 0, 2);
	}

	fclose(file);

	m_fileName = fName;
	m_bIsLoaded = true;

	return true;
}



//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
static CImage *CreateImage(const char *filename)
{
	std::string ext;
	CImage *result = NULL;

	const std::string &fname = filename;
	ext.assign(fname, fname.find_last_of('.') + 1, std::string::npos);

	if(ext.compare("dds") == 0 || ext.compare("DDS") == 0)
		result = new CS3TC();

	else if(ext.compare("tga") == 0 || ext.compare("TGA") == 0)
		result = new CTarga();

	else if(ext.compare("bmp") == 0 || ext.compare("BMP") == 0)
		result = new CBitmap();

	else if(ext.compare("png") == 0 || ext.compare("PNG") == 0)
		result = new CPNG();

	else
	{
		result = NULL;
		return result;
	}
	if(!result->Load(filename))
	{
		printf("Error loading %s\n", filename);
		delete result;
		result = NULL;
	}

	return result;
};


s_gl_texture::s_gl_texture()
{
	id = 0;
	depthID = 0;
	stencilID = 0;
	width = 0;
	height = 0;
	depth = 0;
	height = 0;
	flags = 0;
	target = 0;
	colorSamples = 0;
	coverageSamples = 0;
	pixelFormat = 0;

	fileName = "";

	isCubemap = false;
	hasMipMaps = false;
}




s_gl_texture::~s_gl_texture()
{
	fileName.clear();

	id = 0;
	depthID = 0;
	stencilID = 0;

	width = 0;
	height = 0;
	depth = 0;
	isCubemap = false;
	hasMipMaps = false;
	flags = 0;
	target = 0;
	colorSamples = 0;
	coverageSamples = 0;
	pixelFormat = 0;
}


CTextureInterface::CTextureInterface()
{
	glGetIntegerv(GL_MAX_DRAW_BUFFERS, &m_maxMRT);
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &m_maxTextureUnits);
	glGetIntegerv(GL_MAX_SAMPLES_EXT, &m_maxMSAASamples);

	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &m_maxAnisotropy);
	m_globalAnisotropy = 1;
	m_curFBO = 0;
	m_scratchFBO = 0;
	m_scratchPBO = 0;
	m_defaultFBWidth = GetScreenWidth();
	m_defaultFBHeight = GetScreenHeight();
	m_curDSTarget = DEFAULT_HANDLE;

	m_texUnits = new int[m_maxTextureUnits];
	memset(m_texUnits, -1, sizeof(int) * m_maxTextureUnits);

	m_curColorTargets = new int[m_maxMRT];
	memset(m_curColorTargets, DEFAULT_HANDLE, sizeof(int) * m_maxMRT);

	m_nLatentMRT = 0;
}

CTextureInterface::~CTextureInterface()
{
	for(unsigned int i = 0; i < m_textureObjects.size(); ++i)
	{
		if(m_textureObjects[i].id > 0)
			glDeleteTextures(1, &m_textureObjects[i].id);
		else
		{
			if(m_textureObjects[i].depthID > 0)
				glDeleteRenderbuffers(1, &m_textureObjects[i].depthID);
			if(m_textureObjects[i].stencilID > 0)
				glDeleteRenderbuffers(1, &m_textureObjects[i].stencilID);
		}
	}

	if(m_texUnits)
	{
		delete[] m_texUnits;
		m_texUnits = NULL;
	}

	if(m_curColorTargets)
	{
		delete[] m_curColorTargets;
		m_curColorTargets = NULL;
	}

	if(m_curFBO > 0)
		glDeleteFramebuffers(1, &m_curFBO);

	if(m_scratchFBO > 0)
		glDeleteFramebuffers(1, &m_scratchFBO);

	if(m_scratchPBO > 0)
		glDeleteBuffers(1, &m_scratchPBO);


	m_scratchFBO = 0;
	m_curFBO = 0;
	m_nLatentMRT = 0;
	m_maxMSAASamples = 0;
}

void CTextureInterface::SetGlobalAnisotropy(GLint a)
{
	m_globalAnisotropy = gmin(a, m_maxAnisotropy);
}


/*
	Obtain non-mipmapped filter from one which requires mipmaps
*/
static void get_nonmipmap_filter(unsigned int& filter)
{
	if(filter & SAMPLE_BILINEAR) filter ^= SAMPLE_BILINEAR;
	else if(filter & SAMPLE_TRILINEAR) filter ^= SAMPLE_TRILINEAR;
	else if(filter & SAMPLE_BILINEAR_ANISO) filter ^= SAMPLE_BILINEAR_ANISO;
	else if(filter & SAMPLE_TRILINEAR_ANISO) filter ^= SAMPLE_TRILINEAR_ANISO;
	else
		return;

	filter |= SAMPLE_LINEAR;
}


/*
	From the image flags, obtain the number of coverage samples and or color samples
	for MSAA.

	If this is any NVIDIA implementation, coverage and color samples will be populated,
	otherwise coverageSamples will be 0 (thus not usable
*/
static void get_sample_count(unsigned int& flags, unsigned int& colorSamples, unsigned int& coverageSamples)
{
	if(flags < MSAA_2_SAMPLES)
		return;

	colorSamples = coverageSamples = 0;

	if(flags & MSAA_2_SAMPLES) colorSamples = 2;
	else if(flags & MSAA_4_SAMPLES) colorSamples = 4;
	else if(flags & MSAA_8_SAMPLES) colorSamples = 8;
	else if(flags & MSAA_16_SAMPLES) colorSamples = 16;

	else if(flags & CSAA_8_SAMPLES_NV)
	{
		colorSamples = 4;
		coverageSamples = 8;
	}

	else if(flags & CSAA_16_SAMPLES_NV)
	{
		colorSamples = 4;
		coverageSamples = 16;
	}

	else if(flags & CSAA_8Q_SAMPLES_NV)
	{
		colorSamples = 8;
		coverageSamples = 8;
	}

	else if(flags & CSAA_16Q_SAMPLES_NV)
	{
		colorSamples = 8;
		coverageSamples=  16;
	}

	else
		return;
}


static bool has_mipmap_flag(unsigned char flags)
{
	if((flags & SAMPLE_BILINEAR) || (flags & SAMPLE_TRILINEAR) || (flags & SAMPLE_BILINEAR_ANISO) ||
	   (flags & SAMPLE_TRILINEAR_ANISO))
	   return true;

	return false;
}




int	CTextureInterface::AddTexture(const char* fName, unsigned int imgFlags)
{
	// First look for this texture's existance //
	for(unsigned int i = 0; i < m_textureObjects.size(); ++i)
	{
		// If the texture exists return the existing handle //
		if(m_textureObjects[i].fileName.compare(fName) == 0)
			return i;
	}

	CImage *img = CreateImage(fName);
	if(!img)
		return -1;
	s_gl_texture tex;

	// Set target type //
	if(img->GetWidth() <= 0 || img->GetHeight() <= 0)
		tex.target = GL_TEXTURE_1D;
	else if(img->GetDepth() > 1)
		tex.target = GL_TEXTURE_3D;
	else if(img->IsCubemap())
		tex.target = GL_TEXTURE_CUBE_MAP;
	else
		tex.target = GL_TEXTURE_2D;

	glGenTextures(1, &tex.id);
	glBindTexture(tex.target, tex.id);

	GLint internalFormat;
	GLenum format, type;
	unsigned char pixFmt = img->GetPixelFormat();
	internalFormat = gl_internal_formats[pixFmt];
	format = gl_formats[pixFmt];
	type = gl_types[pixFmt];
	int nMipMaps = img->NumMipmaps();
	bool genMips = false;


	// Check filter for mipmap requirement and generate if necessary //
	// If generation failed, then fallback to linear sampling //
	if(has_mipmap_flag(imgFlags))
	{
		if(nMipMaps <= 1)
		{
			if(!img->GenerateMipmaps())
				genMips = true;
//				get_nonmipmap_filter(imgFlags);

			else
				nMipMaps = img->NumMipmaps();
		}
	}

	// Sort out minification and magnification flags //
	GLint minFilter = (imgFlags & SAMPLE_NEAREST) ? GL_NEAREST : (imgFlags & SAMPLE_LINEAR) ? GL_LINEAR : (imgFlags & SAMPLE_BILINEAR) ?
					  GL_LINEAR_MIPMAP_NEAREST : (imgFlags & SAMPLE_TRILINEAR) ? GL_LINEAR_MIPMAP_LINEAR : (imgFlags & SAMPLE_BILINEAR_ANISO) ?
					  GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_LINEAR;

	GLint magFilter = (imgFlags & SAMPLE_NEAREST) ? GL_NEAREST : GL_LINEAR;


	// Specify default behaviour //
	glTexParameteri(tex.target, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(tex.target, GL_TEXTURE_MAG_FILTER, magFilter);
	glTexParameteri(tex.target, GL_TEXTURE_WRAP_S, (imgFlags & SAMPLE_WRAP) ? GL_REPEAT : GL_CLAMP_TO_EDGE);
	glTexParameteri(tex.target, GL_TEXTURE_WRAP_T, (imgFlags & SAMPLE_WRAP) ? GL_REPEAT : GL_CLAMP_TO_EDGE);
	if(tex.target == GL_TEXTURE_3D)
		glTexParameteri(tex.target, GL_TEXTURE_WRAP_R, (imgFlags & SAMPLE_WRAP) ? GL_REPEAT : GL_CLAMP_TO_EDGE);

	// Apply anisotropic filtering if desired //
	if(imgFlags & SAMPLE_BILINEAR_ANISO || imgFlags & SAMPLE_TRILINEAR_ANISO)
		glTexParameteri(tex.target, GL_TEXTURE_MAX_ANISOTROPY_EXT, m_globalAnisotropy);


	// Upload compressed image via glCompressedTexImage2D //
	if(is_format_compressed(pixFmt))
	{

		unsigned int mipWidth = 0;
		unsigned int mipHeight = 0;
		unsigned int mipSize = 0;

		for(GLint mipLevel = 0; mipLevel < nMipMaps; mipLevel++)
		{
			mipWidth = img->GetWidth(mipLevel);
			mipHeight = img->GetHeight(mipLevel);
			mipSize = img->GetSizeWithMipmaps(mipLevel, 1, pixFmt);

			glCompressedTexImage2D(tex.target, mipLevel, internalFormat, mipWidth, mipHeight, 0, mipSize, img->GetPixels(mipLevel));

			post_gl_errors();
		}
	}

	// Otherwise the image is uncompressed //
	else
	{
		unsigned int mipWidth = 0;
		unsigned int mipHeight = 0;
		unsigned int mipSize = 0;

		for(GLint mipLevel = 0; mipLevel < nMipMaps; ++mipLevel)
		{
			mipWidth = img->GetWidth(mipLevel);
			mipHeight = img->GetHeight(mipLevel);
			mipSize = img->GetSizeWithMipmaps(mipLevel, 1, pixFmt);

			glTexImage2D(tex.target, mipLevel, internalFormat, mipWidth, mipHeight, 0, format, type, img->GetPixels(mipLevel));
		}
	}

	// Pack the texture object and push it onto the vector //
	tex.width = img->GetWidth();
	tex.height = img->GetHeight();
	tex.depth = img->GetDepth();
	tex.flags = imgFlags;
	tex.isCubemap = img->IsCubemap();
	tex.hasMipMaps = (nMipMaps >= 1) ? true : false;
	tex.fileName = fName;

	for(unsigned int i = 0; i < m_textureObjects.size(); ++i)
	{
		if(m_textureObjects[i].id <= 0 && m_textureObjects[i].depthID <= 0 && m_textureObjects[i].stencilID <= 0)
		{
			m_textureObjects[i] = tex;

			delete img;
			glBindTexture(tex.target, 0);

			return i;
		}
	}

	// Generate mipmaps automatically if we were not able to before //
	if( genMips )
		glGenerateMipmap( GL_TEXTURE_2D );


	delete img;
	glBindTexture(tex.target, 0);

	tex.pixelFormat = img->GetPixelFormat();
	m_textureObjects.push_back(tex);

	return m_textureObjects.size() - 1;
}


int CTextureInterface::AddTexture(const unsigned int width, const unsigned int height, const unsigned int pixelFormat, void* buf, unsigned int imgFlags)
{
	s_gl_texture tex;

	// Set target type //
	if(width <= 0 || height <= 0)
		tex.target = GL_TEXTURE_1D;
	else
		tex.target = GL_TEXTURE_2D;

	glGenTextures(1, &tex.id);
	glBindTexture(tex.target, tex.id);

	GLint internalFormat;
	GLenum format, type;
	unsigned char pixFmt = pixelFormat;
	internalFormat = gl_internal_formats[pixFmt];
	format = gl_formats[pixFmt];
	type = gl_types[pixFmt];
	int nMipMaps = 0;

	// Sort out minification and magnification flags //
	GLint minFilter = (imgFlags & SAMPLE_NEAREST) ? GL_NEAREST : (imgFlags & SAMPLE_LINEAR) ? GL_LINEAR : (imgFlags & SAMPLE_BILINEAR) ?
					  GL_LINEAR_MIPMAP_NEAREST : (imgFlags & SAMPLE_TRILINEAR) ? GL_LINEAR_MIPMAP_LINEAR : (imgFlags & SAMPLE_BILINEAR_ANISO) ?
					  GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_LINEAR;

	GLint magFilter = (imgFlags & SAMPLE_NEAREST) ? GL_NEAREST : GL_LINEAR;


	// Specify default behaviour //
	glTexParameteri(tex.target, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(tex.target, GL_TEXTURE_MAG_FILTER, magFilter);
	glTexParameteri(tex.target, GL_TEXTURE_WRAP_S, (imgFlags & SAMPLE_WRAP) ? GL_REPEAT : GL_CLAMP_TO_EDGE);
	glTexParameteri(tex.target, GL_TEXTURE_WRAP_T, (imgFlags & SAMPLE_WRAP) ? GL_REPEAT : GL_CLAMP_TO_EDGE);
	if(tex.target == GL_TEXTURE_3D)
		glTexParameteri(tex.target, GL_TEXTURE_WRAP_R, (imgFlags & SAMPLE_WRAP) ? GL_REPEAT : GL_CLAMP_TO_EDGE);

	// Apply anisotropic filtering if desired //
	if(imgFlags & SAMPLE_BILINEAR_ANISO || imgFlags & SAMPLE_TRILINEAR_ANISO)
		glTexParameteri(tex.target, GL_TEXTURE_MAX_ANISOTROPY_EXT, m_globalAnisotropy);

	glTexImage2D(tex.target, 0, internalFormat, width, height, 0, format, type, buf);


	// Pack the texture object and push it onto the vector //
	tex.width = width;
	tex.height = height;
	tex.depth = 0;
	tex.flags = imgFlags;
	tex.isCubemap = false;
	tex.hasMipMaps = false;
	tex.fileName = "";

	for(size_t i = 0; i < m_textureObjects.size(); ++i)
	{
		if(m_textureObjects[i].id <= 0 && m_textureObjects[i].depthID <= 0 && m_textureObjects[i].stencilID <= 0)
		{
			m_textureObjects[i] = tex;
			glBindTexture(tex.target, 0);

			return i;
		}
	}

	glBindTexture(tex.target, 0);

	tex.pixelFormat = pixelFormat;
	m_textureObjects.push_back(tex);
	return m_textureObjects.size() - 1;
}



int CTextureInterface::AddTexture( CImage* img, unsigned int imgFlags )
{
	s_gl_texture tex;

	// Set target type //
	if( img->GetWidth() <= 0 || img->GetHeight() <= 0)
		tex.target = GL_TEXTURE_1D;
	else
		tex.target = GL_TEXTURE_2D;

	glGenTextures(1, &tex.id);
	glBindTexture(tex.target, tex.id);

	GLint internalFormat;
	GLenum format, type;
	unsigned char pixFmt = img->GetPixelFormat();
	internalFormat = gl_internal_formats[pixFmt];
	format = gl_formats[pixFmt];
	type = gl_types[pixFmt];
	int nMipMaps = 0;

	// Sort out minification and magnification flags //
	GLint minFilter = (imgFlags & SAMPLE_NEAREST) ? GL_NEAREST : (imgFlags & SAMPLE_LINEAR) ? GL_LINEAR : (imgFlags & SAMPLE_BILINEAR) ?
					  GL_LINEAR_MIPMAP_NEAREST : (imgFlags & SAMPLE_TRILINEAR) ? GL_LINEAR_MIPMAP_LINEAR : (imgFlags & SAMPLE_BILINEAR_ANISO) ?
					  GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_LINEAR;

	GLint magFilter = (imgFlags & SAMPLE_NEAREST) ? GL_NEAREST : GL_LINEAR;


	// Specify default behaviour //
	glTexParameteri(tex.target, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(tex.target, GL_TEXTURE_MAG_FILTER, magFilter);
	glTexParameteri(tex.target, GL_TEXTURE_WRAP_S, (imgFlags & SAMPLE_WRAP) ? GL_REPEAT : GL_CLAMP_TO_EDGE);
	glTexParameteri(tex.target, GL_TEXTURE_WRAP_T, (imgFlags & SAMPLE_WRAP) ? GL_REPEAT : GL_CLAMP_TO_EDGE);
	if(tex.target == GL_TEXTURE_3D)
		glTexParameteri(tex.target, GL_TEXTURE_WRAP_R, (imgFlags & SAMPLE_WRAP) ? GL_REPEAT : GL_CLAMP_TO_EDGE);

	// Apply anisotropic filtering if desired //
	if(imgFlags & SAMPLE_BILINEAR_ANISO || imgFlags & SAMPLE_TRILINEAR_ANISO)
		glTexParameteri(tex.target, GL_TEXTURE_MAX_ANISOTROPY_EXT, m_globalAnisotropy);

	glTexImage2D(tex.target, 0, internalFormat, img->GetWidth(), img->GetHeight(), 0, format, type, img->GetPixels());


	// Pack the texture object and push it onto the vector //
	tex.width = img->GetWidth();
	tex.height = img->GetHeight();
	tex.depth = 0;
	tex.flags = imgFlags;
	tex.isCubemap = false;
	tex.hasMipMaps = false;
	tex.fileName = "";

	for(size_t i = 0; i < m_textureObjects.size(); ++i)
	{
		if(m_textureObjects[i].id <= 0 && m_textureObjects[i].depthID <= 0 && m_textureObjects[i].stencilID <= 0)
		{
			m_textureObjects[i] = tex;
			glBindTexture(tex.target, 0);

			return i;
		}
	}

	glBindTexture(tex.target, 0);

	tex.pixelFormat = pixFmt;
	m_textureObjects.push_back(tex);
	return m_textureObjects.size() - 1;
}



static s_gl_texture add_msaa_render_target(unsigned int w, unsigned int h, unsigned char fmt, unsigned int flags)
{
	s_gl_texture rt;
	rt.width = w;
	rt.height = h;
	rt.depth = 1;
	rt.fileName = "";
	rt.hasMipMaps = false;
	rt.isCubemap = false;
	rt.target = GL_TEXTURE_2D;
	rt.flags = flags;

	get_sample_count(flags, rt.colorSamples, rt.coverageSamples);

	GLuint iFmt = gl_internal_formats[fmt];

	glGenRenderbuffers(1, &rt.id);
	glBindRenderbuffer(GL_RENDERBUFFER_EXT, rt.id);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER_EXT, rt.colorSamples, iFmt, w, h);
	glBindRenderbuffer(GL_RENDERBUFFER_EXT, 0);

	return rt;
}


static s_gl_texture add_msaa_ds_target(unsigned int w, unsigned int h, unsigned int d, unsigned int s, unsigned int flags)
{
	s_gl_texture ds;
	ds.width = w;
	ds.height = h;
	ds.target = GL_TEXTURE_2D;
	ds.flags = flags;

	get_sample_count(flags, ds.colorSamples, ds.coverageSamples);

	if(d > 0)
	{
		glGenRenderbuffers(1, &ds.depthID);
	    glBindRenderbuffer(GL_RENDERBUFFER_EXT, ds.depthID);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER_EXT, ds.colorSamples, (d == 16) ? GL_DEPTH_COMPONENT16 : (d == 24) ? GL_DEPTH_COMPONENT24 : GL_DEPTH_COMPONENT32, w, h);
		glBindRenderbuffer(GL_RENDERBUFFER_EXT, 0);
	}

	// Check that D/s space is not already completely consumed by depth resolution //
	if(s > 0 && (d != 32))
	{
		glGenRenderbuffers(1, &ds.stencilID);
		glBindRenderbuffer(GL_RENDERBUFFER_EXT, ds.stencilID);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER_EXT, ds.colorSamples, GL_STENCIL_INDEX8, w, h);
		glBindRenderbuffer(GL_RENDERBUFFER_EXT, 0);
	}

	return ds;
}



int	CTextureInterface::AddRenderTarget(unsigned int width, unsigned int height, unsigned char fmt, unsigned int imgFlags)
{
	// No 1D render targets //
	if(width <= 0 || height <= 0)
		return INVALID_HANDLE;

	s_gl_texture rt;

	// Handle MSAA'd buffers first //
	if(imgFlags >= MSAA_2_SAMPLES)
	{
		unsigned int colorSamps, coverageSamps;
		colorSamps = coverageSamps = 0;
		get_sample_count(imgFlags, colorSamps, coverageSamps);

		if(!(colorSamps == 0 && coverageSamps == 0))
			rt = add_msaa_render_target(width, height, fmt, imgFlags);

		for(size_t i = 0; i < m_textureObjects.size(); ++i)
		{
			if(m_textureObjects[i].id <= 0 && m_textureObjects[i].depthID <= 0 && m_textureObjects[i].stencilID <= 0)
			{
				m_textureObjects[i] = rt;
				return i;
			}
		}

		m_textureObjects.push_back(rt);

		return m_textureObjects.size() - 1;
	}


	rt.width = width;
	rt.height = height;
	rt.depth = 1;
	rt.fileName = "";
	rt.hasMipMaps = false;
	rt.isCubemap = false;
	rt.target = GL_TEXTURE_2D;
	rt.pixelFormat = fmt;

	glGenTextures(1, &rt.id);
	glBindTexture(rt.target, rt.id);

	// Sort out minification and magnification flags //
	GLint minFilter = (imgFlags & SAMPLE_NEAREST) ? GL_NEAREST : (imgFlags & SAMPLE_LINEAR) ? GL_LINEAR : (imgFlags & SAMPLE_BILINEAR) ?
					  GL_LINEAR_MIPMAP_NEAREST : (imgFlags & SAMPLE_TRILINEAR) ? GL_LINEAR_MIPMAP_LINEAR : (imgFlags & SAMPLE_BILINEAR_ANISO) ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR;

	GLint magFilter = (imgFlags & SAMPLE_NEAREST) ? GL_NEAREST : GL_LINEAR;


	// Specify default behaviour //
	glTexParameteri(rt.target, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(rt.target, GL_TEXTURE_MAG_FILTER, magFilter);
	glTexParameteri(rt.target, GL_TEXTURE_WRAP_S, (imgFlags & SAMPLE_WRAP) ? GL_REPEAT : GL_CLAMP_TO_EDGE);
	glTexParameteri(rt.target, GL_TEXTURE_WRAP_T, (imgFlags & SAMPLE_WRAP) ? GL_REPEAT : GL_CLAMP_TO_EDGE);

	// Apply anisotropic filtering if desired //
	if(imgFlags & SAMPLE_BILINEAR_ANISO || imgFlags & SAMPLE_TRILINEAR_ANISO)
		glTexParameteri(rt.target, GL_TEXTURE_MAX_ANISOTROPY_EXT, m_globalAnisotropy);

	GLint internalFormat;
	GLenum format, type;
	internalFormat = gl_internal_formats[fmt];
	format = gl_formats[fmt];
	type = gl_types[fmt];

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, 0);
	if(has_mipmap_flag(imgFlags))
		glGenerateMipmap(GL_TEXTURE_2D);

	for(size_t i = 0; i < m_textureObjects.size(); ++i)
	{
		if(m_textureObjects[i].id <= 0 && m_textureObjects[i].depthID <= 0 && m_textureObjects[i].stencilID <= 0)
		{
			m_textureObjects[i] = rt;
			glBindTexture(rt.target, 0);
			return i;
		}
	}

	glBindTexture(rt.target, 0);
	m_textureObjects.push_back(rt);

	return m_textureObjects.size() - 1;
}

int	CTextureInterface::AddDepthStencilTarget(unsigned int width, unsigned int height, unsigned int depthBits, unsigned int stencilBits, unsigned int imgFlags)
{
	if(width <= 0 || height <= 0)
		return INVALID_HANDLE;

	s_gl_texture ds;

	if(imgFlags >= MSAA_2_SAMPLES)
	{
		unsigned int colorSamps, coverageSamps;
		colorSamps = coverageSamps = 0;
		get_sample_count(imgFlags, colorSamps, coverageSamps);

		if(!(colorSamps == 0 && coverageSamps == 0))
			ds = add_msaa_ds_target(width, height, depthBits, stencilBits, imgFlags);

		for(size_t i = 0; i < m_textureObjects.size(); ++i)
		{
			if(m_textureObjects[i].id <= 0 && m_textureObjects[i].depthID <= 0 && m_textureObjects[i].stencilID <= 0)
			{
				m_textureObjects[i] = ds;
				return i;
			}
		}

		m_textureObjects.push_back(ds);

		return m_textureObjects.size() - 1;
	}

	ds.target = GL_TEXTURE_2D;

	if(depthBits > 0)
	{
		glGenRenderbuffers(1, &ds.depthID);
	    glBindRenderbuffer(GL_RENDERBUFFER_EXT, ds.depthID);
		glRenderbufferStorage(GL_RENDERBUFFER_EXT, (depthBits == 16) ? GL_DEPTH_COMPONENT16 : (depthBits == 24) ? GL_DEPTH_COMPONENT24 : GL_DEPTH_COMPONENT32, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER_EXT, 0);
	}

	// Check that D/s space is not already completely consumed by depth resolution //
	if(stencilBits > 0 && (depthBits != 32))
	{
		glGenRenderbuffers(1, &ds.stencilID);
		glBindRenderbuffer(GL_RENDERBUFFER_EXT, ds.stencilID);
		glRenderbufferStorage(GL_RENDERBUFFER_EXT, GL_STENCIL_INDEX8, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER_EXT, 0);
	}

	ds.pixelFormat = (depthBits == 16) ? PIXEL_FORMAT_DEPTH16 : (depthBits > 0) ? PIXEL_FORMAT_DEPTH32 : 0;


	for(size_t i = 0; i < m_textureObjects.size(); ++i)
	{
		if(m_textureObjects[i].id <= 0 && m_textureObjects[i].depthID <= 0 && m_textureObjects[i].stencilID <= 0)
		{
			m_textureObjects[i] = ds;
			return i;
		}
	}

	ds.width = width;
	ds.height = height;
	m_textureObjects.push_back(ds);
	return m_textureObjects.size() - 1;
}

void CTextureInterface::BindTexture(int handle, unsigned int texUnit)
{
	if(handle == INVALID_HANDLE)
		return;

	s_gl_texture tex = m_textureObjects[handle];

	glEnable(tex.target);

	if(texUnit >= m_maxTextureUnits)
		texUnit = m_maxTextureUnits - 1;

	// Bind texture to GL state //
	glActiveTexture(GL_TEXTURE0 + texUnit);
	glBindTexture(tex.target, tex.id);


	// Setup texture sampler //
	unsigned char imgFlags = tex.flags;
	GLint minFilter = (imgFlags & SAMPLE_NEAREST) ? GL_NEAREST : (imgFlags & SAMPLE_LINEAR) ? GL_LINEAR : (imgFlags & SAMPLE_BILINEAR) ?
					  GL_LINEAR_MIPMAP_NEAREST : (imgFlags & SAMPLE_TRILINEAR) ? GL_LINEAR_MIPMAP_LINEAR : (imgFlags & SAMPLE_BILINEAR_ANISO) ?
					  GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_LINEAR;

	GLint magFilter = (imgFlags & SAMPLE_NEAREST) ? GL_NEAREST : GL_LINEAR;

	// Specify sampler behaviour //
//	glTexParameteri(tex.target, GL_TEXTURE_WRAP_S, (imgFlags & SAMPLE_WRAP) ? GL_REPEAT : GL_CLAMP_TO_EDGE);
//	glTexParameteri(tex.target, GL_TEXTURE_WRAP_T, (imgFlags & SAMPLE_WRAP) ? GL_REPEAT : GL_CLAMP_TO_EDGE);
//	if(tex.target == GL_TEXTURE_3D)
//		glTexParameteri(tex.target, GL_TEXTURE_WRAP_R, (imgFlags & SAMPLE_WRAP) ? GL_REPEAT : GL_CLAMP_TO_EDGE);

	// Apply anisotropic filtering if desired //
//	if(imgFlags & SAMPLE_BILINEAR_ANISO || imgFlags & SAMPLE_TRILINEAR_ANISO)
//		glTexParameteri(tex.target, GL_TEXTURE_MAX_ANISOTROPY_EXT, m_globalAnisotropy);

	// This texture unit is now in use //
	m_texUnits[texUnit] = handle;
}



bool CTextureInterface::BindRenderTarget(int* colorTargets, int nColorTargets, int dsTarget)
{
	// First check to see that we're binding the default framebuffer //
	if(nColorTargets == 1 && colorTargets[0] < 0 && dsTarget < 0)
	{
		if(m_curColorTargets[0] == DEFAULT_HANDLE && m_curDSTarget == DEFAULT_HANDLE)
			return true;

		for(int i = 0; i < m_maxMRT; ++i)
			if(m_curColorTargets[i] >= 0)
				++m_nLatentMRT;

		glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
		glViewport(0, 0, m_defaultFBWidth, m_defaultFBHeight);

		memset(m_curColorTargets, DEFAULT_HANDLE, sizeof(int) * m_maxMRT);
		m_curDSTarget = DEFAULT_HANDLE;

		glDeleteFramebuffers(1, &m_curFBO);
		m_curFBO = 0;

		return true;
	}

	// Otherwise we're binding some combination of something //
	else
	{
		// First check if we have a framebuffer object generated //
		if(m_curFBO <= 0)
			glGenFramebuffers(1, &m_curFBO);
		glBindFramebuffer(GL_FRAMEBUFFER_EXT, m_curFBO);

		// Check for depth stencil target bind //
		if(dsTarget != m_curDSTarget)
		{
			// We're binding the default DS target back //
			if(dsTarget < 0)
				glFramebufferRenderbuffer(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, 0);

			// Otherwise its a new bind //
			else
			{
				if(m_textureObjects[dsTarget].id > 0)
					glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, m_textureObjects[dsTarget].id, m_textureObjects[dsTarget].target, 0);
				else
					glFramebufferRenderbuffer(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, m_textureObjects[dsTarget].depthID);
			}

			m_curDSTarget = dsTarget;
		}

		// If no color targets are to be bound we can safely disable reads and writes to the color target //
		if(nColorTargets <= 0 || !colorTargets)
		{
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		}

		// Otherwise we have color targets to bind //
		else
		{
			GLuint* curDrawBuffers = new GLuint[m_maxMRT];
			int i = 0;
			for(i = 0; i < nColorTargets; ++i)
			{
				if(m_curColorTargets[i] != colorTargets[i])
				{
					if(m_textureObjects[colorTargets[i]].colorSamples <= 0 && m_textureObjects[colorTargets[i]].coverageSamples <= 0)
						glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + i, GL_TEXTURE_2D, m_textureObjects[colorTargets[i]].id, 0);
					else
					{
						if(m_textureObjects[colorTargets[i]].flags & TEXTURE_WRITE_ONLY)
							glFramebufferRenderbuffer(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + i, GL_RENDERBUFFER_EXT, m_textureObjects[colorTargets[i]].id);
						else
							glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + i, GL_TEXTURE_2D, m_textureObjects[colorTargets[i]].id, 0);
					}
					m_curColorTargets[i] = colorTargets[i];
				}

				curDrawBuffers[i] = GL_COLOR_ATTACHMENT0_EXT + i;
			}

			glDrawBuffersARB(i, curDrawBuffers);
			delete[] curDrawBuffers;
		}

		int id = (colorTargets) ? colorTargets[0] : dsTarget;

		// Get the current viewport //
		GLint vpDims[4];
		glGetIntegerv(GL_VIEWPORT, vpDims);
		m_defaultFBWidth = vpDims[2];
		m_defaultFBHeight = vpDims[3];

		glViewport(0, 0, m_textureObjects[id].width, m_textureObjects[id].height);
	}

	fbo_status completion = check_fbo_status();
	if(completion > COMPLETE)
	{
		// Check that we're not in one of those strict drivers that checks for
		// FBO format completeness before the actual draw call
		if(completion == INCOMPLETE_FORMATS)
		{
			for(unsigned int i = nColorTargets + 1; i < m_nLatentMRT; ++i)
				glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + i, GL_TEXTURE_2D, 0, 0);

			m_nLatentMRT = 0;
		}

		// Check again to make sure //
		completion = check_fbo_status();
		if(completion > COMPLETE)
		{
			glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
			return false;
		}
	}

	return true;
}


void CTextureInterface::ReadTexture(int handle, void* buf)
{
	if(m_scratchFBO == 0)
		glGenFramebuffers(1, &m_scratchFBO);
	glBindFramebuffer(GL_FRAMEBUFFER_EXT, m_scratchFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, m_textureObjects[handle].id, 0);

	if(check_fbo_status() > COMPLETE)
	{
		glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
		return;
	}

	if(m_scratchPBO == 0)
		glGenBuffers(1, &m_scratchPBO);

	post_gl_errors();

	unsigned char pixelFormat = m_textureObjects[handle].pixelFormat;
	int imgWidth = m_textureObjects[handle].width;
	int imgHeight = m_textureObjects[handle].height;
	int imgBitDepth = get_bytes_per_pixel(pixelFormat);
	int imgSize = imgWidth * imgHeight * imgBitDepth;

	glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
	glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, m_scratchPBO);
	glBufferData(GL_PIXEL_PACK_BUFFER_ARB, imgSize, NULL, GL_DYNAMIC_READ);
	glReadPixels(0, 0, imgWidth, imgHeight, GL_RGBA, is_float_format(pixelFormat) ? GL_FLOAT : GL_UNSIGNED_BYTE, (char*)NULL + 0);

	void* scratchMem = glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
	memcpy(buf, scratchMem, imgSize);

	glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
}


void CTextureInterface::ResolveMSAATarget(const int destHandle)
{
	// Lets do some sanity checks here //
//	if(!IS_HANDLE_VALID(destHandle) || !IS_HANDLE_VALID(m_curFBO))
//		return;

	if(m_textureObjects[destHandle].flags >= MSAA_2_SAMPLES)
		return;

	int w = m_textureObjects[destHandle].width;
	int h = m_textureObjects[destHandle].height;

	GLuint tmpFBO;
	if(destHandle >= 0)
	{

		glGenFramebuffers(1, &tmpFBO);
		glBindFramebuffer(GL_FRAMEBUFFER_EXT, tmpFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, m_textureObjects[destHandle].id, 0);

		fbo_status completion = check_fbo_status();

		glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
	}

	glBindFramebuffer(GL_READ_FRAMEBUFFER_EXT, m_curFBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER_EXT, (destHandle == DEFAULT_HANDLE) ? 0 : tmpFBO);
	glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_READ_FRAMEBUFFER_EXT, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER_EXT, 0);

	if(destHandle >= 0)
		glDeleteFramebuffers(1, &tmpFBO);
}


void CTextureInterface::EvictTextures()
{
	for(int i = 0; i < m_maxTextureUnits; ++i)
	{
		if(m_texUnits[i] >= 0)
		{
			glBindTexture(m_textureObjects[m_texUnits[i]].target, 0);
			glDisable(m_textureObjects[m_texUnits[i]].target);

			m_texUnits[i] = -1;
		}
	}

	glActiveTexture(GL_TEXTURE0);
}


void CTextureInterface::DeleteTexture(int handle)
{
	if(handle == INVALID_HANDLE)
		return;

	if(m_textureObjects[handle].id > 0)
		glDeleteTextures(1, &m_textureObjects[handle].id);
	else if(m_textureObjects[handle].depthID > 0)
		glDeleteRenderbuffers(1, &m_textureObjects[handle].depthID);
	else
		return;

	m_textureObjects[handle].id = INVALID_HANDLE;
}

int CTextureInterface::GetSamplerResidency(int handle)
{
	int texUnit = 0;

	for(int i = 0; i < m_maxTextureUnits; ++i)
	{
		// If this texture is already bound, pass the parameter //
		if(m_texUnits[i] == handle)
			return i;

		// Otherwise look for an open slot //
		else
		{
			if(m_texUnits[i] <= 0)
			{
				texUnit = i;
				m_texUnits[i] = handle;
				break;
			}
		}

		// If we've gotten here, we need to forcefully evict textures //
		texUnit = 0;
	}

	return texUnit;
}



bool CTextureInterface::DebugRender( int tex )
{
	if( !IS_HANDLE_VALID( tex ) )
		return false;

	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, m_textureObjects[tex].id );

	RenderTexturedFullscreenQuad();

	glBindTexture( GL_TEXTURE_2D, 0 );

	return true;
}

GLuint CTextureInterface::GetGLHandle(int handle)
{
	if(m_textureObjects[handle].id > 0)
		return m_textureObjects[handle].id;
	else if(m_textureObjects[handle].stencilID > 0)
		return m_textureObjects[handle].stencilID;
	else if(m_textureObjects[handle].depthID > 0)
		return m_textureObjects[handle].depthID;
	else
		return 0;
}
