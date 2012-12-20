/////////////////////////////////////////////////////////////////////////////////////
//
// EXTENSIONS.H
//
// Adapted for the Mooball project 12/09
// This file serves to provide extended OpenGL functionality for both windows
// and X systems.
//
// Provides functions which define function hooks from the driver.
//
// "initExtensions" shall be called one time prior to any dependent draw calls
//
// see: http://googlecode.com/p/mooball
//
/////////////////////////////////////////////////////////////////////////////////////



#ifndef _EXTENSIONS_H_
#define _EXTENSIONS_H_


#define INVALID_HANDLE		-1
#define DEFAULT_HANDLE		-2

static bool IS_HANDLE_VALID( int a )
{
	return ( a >= 0 ) ? true : false;
}


//mesa's gl.h 7.5 includes glext.h which defines all the GL_VERSIONs doing this stops gl.h including glext.h
#define GL_GLEXT_LEGACY

#ifdef WIN32
    #include <windows.h>
    #include <WindowsX.h>
    #include <gl/gl.h>
#else
    #include <GL/gl.h>
#endif

#ifndef GL_VERSION_1_2
	#define EXTEND_1_2 1
#else
	#warning "Have GL 1.2 support"
#endif

#ifndef GL_VERSION_1_3
	#define EXTEND_1_3 1
#else
	#warning "Have GL 1.3 support"
#endif

#ifndef GL_VERSION_1_4
	#define EXTEND_1_4 1
#else
	#warning "Have GL 1.4 support"
#endif

#ifndef GL_VERSION_1_5
	#define EXTEND_1_5 1
#else
	#warning "Have GL 1.5 support"
#endif

#ifndef GL_VERSION_2_0
	#define EXTEND_2_0 1
#else
	#warning "Have GL 2.0 support"
#endif

#ifdef WIN32
	#include <gl/wglext.h>
	#include <gl/glext.h>
#else
	#include <GL/glx.h>
	#include <GL/glxext.h>
	#include <GL/glext.h>
	#include <X11/extensions/xf86vmode.h>
	#include <X11/cursorfont.h>
#endif

#include <stdlib.h>
#include <string.h>


// GL 1.2 function hooks for Windows distribution //
//#ifdef WIN32
#ifdef EXTEND_1_2
   extern PFNGLTEXIMAGE3DPROC				glTexImage3D;
#endif

// GL 1.3 function hooks for Windows distribution //
#ifdef EXTEND_1_3
    extern PFNGLACTIVETEXTUREPROC			glActiveTexture;
    extern PFNGLCOMPRESSEDTEXIMAGE2DPROC	glCompressedTexImage2D;
    extern PFNGLCLIENTACTIVETEXTUREPROC		glClientActiveTexture;
#endif


// GL 1.5 function extensions //
#ifdef EXTEND_1_5
	extern PFNGLGENBUFFERSPROC				glGenBuffers;
	extern PFNGLBINDBUFFERPROC				glBindBuffer;
	extern PFNGLBUFFERDATAPROC				glBufferData;
	extern PFNGLDELETEBUFFERSPROC			glDeleteBuffers;
	extern PFNGLMAPBUFFERPROC				glMapBuffer;
	extern PFNGLUNMAPBUFFERPROC				glUnmapBuffer;
	extern PFNGLGETBUFFERPARAMETERIVPROC	glGetBufferParameteriv;
#endif


#ifdef EXTEND_2_0
	extern PFNGLATTACHSHADERPROC				glAttachShader;
	extern PFNGLBINDATTRIBLOCATIONPROC			glBindAttribLocation;
	extern PFNGLCOMPILESHADERPROC				glCompileShader;
	extern PFNGLCREATEPROGRAMPROC				glCreateProgram;
	extern PFNGLCREATESHADERPROC				glCreateShader;
	extern PFNGLDELETEPROGRAMPROC				glDeleteProgram;
	extern PFNGLDELETESHADERPROC				glDeleteShader;
	extern PFNGLDETACHSHADERPROC				glDetachShader;
	extern PFNGLENABLEVERTEXATTRIBARRAYPROC 	glEnableVertexAttribArray;
	extern PFNGLDISABLEVERTEXATTRIBARRAYPROC	glDisableVertexAttribArray;
	extern PFNGLGETACTIVEATTRIBPROC				glGetActiveAttrib;
	extern PFNGLGETACTIVEUNIFORMPROC			glGetActiveUniform;
	extern PFNGLGETATTRIBLOCATIONPROC			glGetAttribLocation;
	extern PFNGLGETATTACHEDSHADERSPROC			glGetAttachedShaders;
	extern PFNGLGETPROGRAMIVPROC				glGetProgramiv;
	extern PFNGLGETPROGRAMINFOLOGPROC			glGetProgramInfoLog;
	extern PFNGLGETSHADERIVPROC					glGetShaderiv;
	extern PFNGLGETSHADERINFOLOGPROC			glGetShaderInfoLog;
	extern PFNGLGETSHADERSOURCEPROC				glGetShaderSource;
	extern PFNGLGETUNIFORMLOCATIONPROC			glGetUniformLocation;
	extern PFNGLGETUNIFORMFVPROC				glGetUniformfv;
	extern PFNGLGETUNIFORMIVPROC				glGetUniformiv;
	extern PFNGLGETVERTEXATTRIBDVPROC			glGetVertexAttribdv;
	extern PFNGLGETVERTEXATTRIBFVPROC			glGetVertexAttribfv;
	extern PFNGLGETVERTEXATTRIBIVPROC			glGetVertexAttribiv;
	extern PFNGLISPROGRAMPROC					glIsProgram;
	extern PFNGLISSHADERPROC					glIsShader;
	extern PFNGLLINKPROGRAMPROC					glLinkProgram; 
	extern PFNGLSHADERSOURCEPROC				glShaderSource;
	extern PFNGLUSEPROGRAMPROC					glUseProgram;
	extern PFNGLUNIFORM1FPROC					glUniform1f; 
	extern PFNGLUNIFORM2FPROC					glUniform2f; 
	extern PFNGLUNIFORM3FPROC					glUniform3f; 
	extern PFNGLUNIFORM4FPROC					glUniform4f; 
	extern PFNGLUNIFORM1IPROC					glUniform1i; 
	extern PFNGLUNIFORM2IPROC					glUniform2i; 
	extern PFNGLUNIFORM3IPROC					glUniform3i; 
	extern PFNGLUNIFORM4IPROC					glUniform4i; 
	extern PFNGLUNIFORM1FVPROC					glUniform1fv;
	extern PFNGLUNIFORM2FVPROC					glUniform2fv;
	extern PFNGLUNIFORM3FVPROC					glUniform3fv;
	extern PFNGLUNIFORM4FVPROC					glUniform4fv;
	extern PFNGLUNIFORM1IVPROC					glUniform1iv;
	extern PFNGLUNIFORM2IVPROC					glUniform2iv;
	extern PFNGLUNIFORM3IVPROC					glUniform3iv;
	extern PFNGLUNIFORM4IVPROC					glUniform4iv;
	extern PFNGLUNIFORMMATRIX2FVPROC			glUniformMatrix2fv;
	extern PFNGLUNIFORMMATRIX3FVPROC			glUniformMatrix3fv;
	extern PFNGLUNIFORMMATRIX4FVPROC			glUniformMatrix4fv;
	extern PFNGLVALIDATEPROGRAMPROC				glValidateProgram; 
	extern PFNGLVERTEXATTRIB1DPROC				glVertexAttrib1d; 
	extern PFNGLVERTEXATTRIB1DVPROC				glVertexAttrib1dv;
	extern PFNGLVERTEXATTRIB1FPROC				glVertexAttrib1f; 
	extern PFNGLVERTEXATTRIB1FVPROC				glVertexAttrib1fv;
	extern PFNGLVERTEXATTRIB1SPROC				glVertexAttrib1s; 
	extern PFNGLVERTEXATTRIB1SVPROC				glVertexAttrib1sv;
	extern PFNGLVERTEXATTRIB2DPROC				glVertexAttrib2d; 
	extern PFNGLVERTEXATTRIB2DVPROC				glVertexAttrib2dv;
	extern PFNGLVERTEXATTRIB2FPROC				glVertexAttrib2f;
	extern PFNGLVERTEXATTRIB2FVPROC				glVertexAttrib2fv;
	extern PFNGLVERTEXATTRIB2SPROC				glVertexAttrib2s; 
	extern PFNGLVERTEXATTRIB2SVPROC				glVertexAttrib2sv;
	extern PFNGLVERTEXATTRIB3DPROC				glVertexAttrib3d; 
	extern PFNGLVERTEXATTRIB3DVPROC				glVertexAttrib3dv;
	extern PFNGLVERTEXATTRIB3FPROC				glVertexAttrib3f; 
	extern PFNGLVERTEXATTRIB3FVPROC				glVertexAttrib3fv;
	extern PFNGLVERTEXATTRIB3SPROC				glVertexAttrib3s; 
	extern PFNGLVERTEXATTRIB3SVPROC				glVertexAttrib3sv;
	extern PFNGLVERTEXATTRIB4NBVPROC			glVertexAttrib4Nbv;
	extern PFNGLVERTEXATTRIB4NIVPROC			glVertexAttrib4Niv; 
	extern PFNGLVERTEXATTRIB4NSVPROC			glVertexAttrib4Nsv; 
	extern PFNGLVERTEXATTRIB4NUBPROC			glVertexAttrib4Nub; 
	extern PFNGLVERTEXATTRIB4NUBVPROC 			glVertexAttrib4Nubv;
	extern PFNGLVERTEXATTRIB4NUIVPROC 			glVertexAttrib4Nuiv;
	extern PFNGLVERTEXATTRIB4NUSVPROC 			glVertexAttrib4Nusv;
	extern PFNGLVERTEXATTRIB4BVPROC				glVertexAttrib4bv;
	extern PFNGLVERTEXATTRIB4DPROC 				glVertexAttrib4d; 
	extern PFNGLVERTEXATTRIB4DVPROC				glVertexAttrib4dv;
	extern PFNGLVERTEXATTRIB4FPROC				glVertexAttrib4f; 
	extern PFNGLVERTEXATTRIB4FVPROC				glVertexAttrib4fv;
	extern PFNGLVERTEXATTRIB4IVPROC				glVertexAttrib4iv;
	extern PFNGLVERTEXATTRIB4SPROC 				glVertexAttrib4s; 
	extern PFNGLVERTEXATTRIB4SVPROC				glVertexAttrib4sv;
	extern PFNGLVERTEXATTRIB4UBVPROC			glVertexAttrib4ubv;
	extern PFNGLVERTEXATTRIB4UIVPROC			glVertexAttrib4uiv;
	extern PFNGLVERTEXATTRIB4USVPROC			glVertexAttrib4usv;
	extern PFNGLVERTEXATTRIBPOINTERPROC			glVertexAttribPointer;
#endif


// extension hooks for FBO's //
#ifdef GL_ARB_framebuffer_object
	extern PFNGLISRENDERBUFFERPROC						glIsRenderbuffer;
	extern PFNGLBINDRENDERBUFFERPROC					glBindRenderbuffer;
	extern PFNGLDELETERENDERBUFFERSPROC					glDeleteRenderbuffers;
	extern PFNGLGENRENDERBUFFERSPROC					glGenRenderbuffers;
	extern PFNGLRENDERBUFFERSTORAGEPROC					glRenderbufferStorage;
	extern PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC		glRenderbufferStorageMultisample;
	extern PFNGLGETRENDERBUFFERPARAMETERIVPROC			glGetRenderbufferParameteriv;
	extern PFNGLISFRAMEBUFFERPROC						glIsFramebuffer;
	extern PFNGLBINDFRAMEBUFFERPROC						glBindFramebuffer;
	extern PFNGLDELETEFRAMEBUFFERSPROC					glDeleteFramebuffers;
	extern PFNGLGENFRAMEBUFFERSPROC						glGenFramebuffers;
	extern PFNGLCHECKFRAMEBUFFERSTATUSPROC				glCheckFramebufferStatus;
	extern PFNGLFRAMEBUFFERTEXTURE1DPROC				glFramebufferTexture1D;
	extern PFNGLFRAMEBUFFERTEXTURE2DPROC				glFramebufferTexture2D;
	extern PFNGLFRAMEBUFFERTEXTURE3DPROC				glFramebufferTexture3D;
	extern PFNGLFRAMEBUFFERRENDERBUFFERPROC				glFramebufferRenderbuffer;
	extern PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC	glGetFramebufferAttachmentParameteriv;
	extern PFNGLGENERATEMIPMAPPROC						glGenerateMipmap;
	extern PFNGLBLITFRAMEBUFFERPROC						glBlitFramebuffer;
#endif



// v-sync function hook
#ifdef WIN32
    extern PFNWGLSWAPINTERVALEXTPROC			wglSwapIntervalEXT;
#else
    extern PFNGLXSWAPINTERVALSGIPROC            glXSwapIntervalSGI;
#endif

// Shader object hooks //
#ifdef GL_ARB_shader_objects
	extern PFNGLDELETEOBJECTARBPROC				glDeleteObjectARB;
	extern PFNGLGETHANDLEARBPROC				glGetHandleARB;
	extern PFNGLDETACHOBJECTARBPROC				glDetachObjectARB;
	extern PFNGLCREATESHADEROBJECTARBPROC		glCreateShaderObjectARB;
	extern PFNGLSHADERSOURCEARBPROC				glShaderSourceARB;
	extern PFNGLCOMPILESHADERARBPROC			glCompileShaderARB;
	extern PFNGLCREATEPROGRAMOBJECTARBPROC		glCreateProgramObjectARB;
	extern PFNGLATTACHOBJECTARBPROC				glAttachObjectARB;
	extern PFNGLLINKPROGRAMARBPROC				glLinkProgramARB;
	extern PFNGLUSEPROGRAMOBJECTARBPROC			glUseProgramObjectARB;
	extern PFNGLVALIDATEPROGRAMARBPROC			glValidateProgramARB;
	extern PFNGLUNIFORM1FARBPROC				glUniform1fARB;
	extern PFNGLUNIFORM2FARBPROC				glUniform2fARB;
	extern PFNGLUNIFORM3FARBPROC				glUniform3fARB;
	extern PFNGLUNIFORM4FARBPROC				glUniform4fARB;
	extern PFNGLUNIFORM1IARBPROC				glUniform1iARB;
	extern PFNGLUNIFORM2IARBPROC				glUniform2iARB;
	extern PFNGLUNIFORM3IARBPROC				glUniform3iARB;
	extern PFNGLUNIFORM4IARBPROC				glUniform4iARB;
	extern PFNGLUNIFORM1FVARBPROC				glUniform1fvARB;
	extern PFNGLUNIFORM2FVARBPROC				glUniform2fvARB;
	extern PFNGLUNIFORM3FVARBPROC				glUniform3fvARB;
	extern PFNGLUNIFORM4FVARBPROC				glUniform4fvARB;
	extern PFNGLUNIFORM1IVARBPROC				glUniform1ivARB;
	extern PFNGLUNIFORM2IVARBPROC				glUniform2ivARB;
	extern PFNGLUNIFORM3IVARBPROC				glUniform3ivARB;
	extern PFNGLUNIFORM4IVARBPROC				glUniform4ivARB;
	extern PFNGLUNIFORMMATRIX2FVARBPROC			glUniformMatrix2fvARB;
	extern PFNGLUNIFORMMATRIX3FVARBPROC			glUniformMatrix3fvARB;
	extern PFNGLUNIFORMMATRIX4FVARBPROC			glUniformMatrix4fvARB;
	extern PFNGLGETOBJECTPARAMETERFVARBPROC		glGetObjectParameterfvARB;
	extern PFNGLGETOBJECTPARAMETERIVARBPROC		glGetObjectParameterivARB;
	extern PFNGLGETINFOLOGARBPROC				glGetInfoLogARB;
	extern PFNGLGETATTACHEDOBJECTSARBPROC		glGetAttachedObjectsARB;
	extern PFNGLGETUNIFORMLOCATIONARBPROC		glGetUniformLocationARB;
	extern PFNGLGETACTIVEUNIFORMARBPROC			glGetActiveUniformARB;
	extern PFNGLGETUNIFORMFVARBPROC				glGetUniformfvARB;
	extern PFNGLGETUNIFORMIVARBPROC				glGetUniformivARB;
	extern PFNGLGETSHADERSOURCEARBPROC			glGetShaderSourceARB;
#endif


#ifdef GL_ARB_vertex_array_object
	extern PFNGLBINDVERTEXARRAYPROC				glBindVertexArray;
	extern PFNGLDELETEVERTEXARRAYSPROC			glDeleteVertexArrays;
	extern PFNGLGENVERTEXARRAYSPROC				glGenVertexArrays;
	extern PFNGLISVERTEXARRAYPROC				glIsVertexArray;
#endif

// function hooks for GL_ARB_draw_buffers //
extern PFNGLDRAWBUFFERSARBPROC             		glDrawBuffersARB;



// queriable states //
extern bool EXT_VERTEX_BUFFER_OBJECT;
extern bool EXT_CUBEMAP;
extern bool EXT_FRAMEBUFFER_OBJECT;
extern bool EXT_VERTEX_PROGRAM;
extern bool EXT_VSYNC;
extern bool EXT_HALF_FLOAT;
extern bool EXT_SHADER_OBJECTS;
extern bool EXT_DRAW_BUFFERS;
extern bool EXT_SGIS_MIPMAP;
extern bool EXT_BGRA;
// Should be called upon OpenGL DC creation once only //
extern bool initExtensions();




#endif
