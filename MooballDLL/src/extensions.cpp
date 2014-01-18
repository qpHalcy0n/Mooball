#include "extensions.h"


bool EXT_VERTEX_BUFFER_OBJECT = false;
bool EXT_CUBEMAP = false;
bool EXT_FRAMEBUFFER_OBJECT = false;
bool EXT_VERTEX_PROGRAM = false;
bool EXT_VSYNC = false;
bool EXT_HALF_FLOAT = false;
bool EXT_SHADER_OBJECTS = false;
bool EXT_DRAW_BUFFERS = false;
bool EXT_SGIS_MIPMAP = false;
bool EXT_BGRA = false;

bool OGL_VER_1_3 = false;


#ifdef EXTEND_1_5
	PFNGLGENBUFFERSPROC					glGenBuffers = NULL;
	PFNGLBINDBUFFERPROC					glBindBuffer = NULL;
	PFNGLBUFFERDATAPROC					glBufferData = NULL;
	PFNGLDELETEBUFFERSPROC				glDeleteBuffers = NULL;
	PFNGLMAPBUFFERPROC					glMapBuffer = NULL;
	PFNGLUNMAPBUFFERPROC				glUnmapBuffer = NULL;
	PFNGLGETBUFFERPARAMETERIVPROC		glGetBufferParameteriv = NULL;
#endif


#ifdef EXTEND_2_0
	PFNGLATTACHSHADERPROC				glAttachShader = NULL;
	PFNGLBINDATTRIBLOCATIONPROC			glBindAttribLocation = NULL;
	PFNGLCOMPILESHADERPROC				glCompileShader	= NULL;
	PFNGLCREATEPROGRAMPROC				glCreateProgram	= NULL;
	PFNGLCREATESHADERPROC				glCreateShader	= NULL;
	PFNGLDELETEPROGRAMPROC				glDeleteProgram	= NULL;
	PFNGLDELETESHADERPROC				glDeleteShader	= NULL;
	PFNGLDETACHSHADERPROC				glDetachShader	= NULL;
	PFNGLENABLEVERTEXATTRIBARRAYPROC 	glEnableVertexAttribArray	= NULL;
	PFNGLDISABLEVERTEXATTRIBARRAYPROC	glDisableVertexAttribArray	= NULL;
	PFNGLGETACTIVEATTRIBPROC			glGetActiveAttrib = NULL;
	PFNGLGETACTIVEUNIFORMPROC			glGetActiveUniform	= NULL;
	PFNGLGETATTRIBLOCATIONPROC			glGetAttribLocation	= NULL;
	PFNGLGETATTACHEDSHADERSPROC			glGetAttachedShaders = NULL;
	PFNGLGETPROGRAMIVPROC				glGetProgramiv = NULL;
	PFNGLGETPROGRAMINFOLOGPROC			glGetProgramInfoLog	= NULL;
	PFNGLGETSHADERIVPROC				glGetShaderiv= NULL;
	PFNGLGETSHADERINFOLOGPROC			glGetShaderInfoLog = NULL;
	PFNGLGETSHADERSOURCEPROC			glGetShaderSource = NULL;
	PFNGLGETUNIFORMLOCATIONPROC			glGetUniformLocation = NULL;
	PFNGLGETUNIFORMFVPROC				glGetUniformfv = NULL;
	PFNGLGETUNIFORMIVPROC				glGetUniformiv = NULL;
	PFNGLGETVERTEXATTRIBDVPROC			glGetVertexAttribdv = NULL;
	PFNGLGETVERTEXATTRIBFVPROC			glGetVertexAttribfv = NULL;
	PFNGLGETVERTEXATTRIBIVPROC			glGetVertexAttribiv = NULL;
	PFNGLISPROGRAMPROC					glIsProgram = NULL;
	PFNGLISSHADERPROC					glIsShader = NULL;
	PFNGLLINKPROGRAMPROC				glLinkProgram = NULL; 
	PFNGLSHADERSOURCEPROC				glShaderSource = NULL;
	PFNGLUSEPROGRAMPROC					glUseProgram = NULL;
	PFNGLUNIFORM1FPROC					glUniform1f = NULL; 
	PFNGLUNIFORM2FPROC					glUniform2f = NULL; 
	PFNGLUNIFORM3FPROC					glUniform3f = NULL; 
	PFNGLUNIFORM4FPROC					glUniform4f = NULL; 
	PFNGLUNIFORM1IPROC					glUniform1i = NULL; 
	PFNGLUNIFORM2IPROC					glUniform2i = NULL; 
	PFNGLUNIFORM3IPROC					glUniform3i = NULL; 
	PFNGLUNIFORM4IPROC					glUniform4i = NULL; 
	PFNGLUNIFORM1FVPROC					glUniform1fv = NULL;
	PFNGLUNIFORM2FVPROC					glUniform2fv = NULL;
	PFNGLUNIFORM3FVPROC					glUniform3fv = NULL;
	PFNGLUNIFORM4FVPROC					glUniform4fv = NULL;
	PFNGLUNIFORM1IVPROC					glUniform1iv = NULL;
	PFNGLUNIFORM2IVPROC					glUniform2iv = NULL;
	PFNGLUNIFORM3IVPROC					glUniform3iv = NULL;
	PFNGLUNIFORM4IVPROC					glUniform4iv = NULL;
	PFNGLUNIFORMMATRIX2FVPROC			glUniformMatrix2fv = NULL;
	PFNGLUNIFORMMATRIX3FVPROC			glUniformMatrix3fv = NULL;
	PFNGLUNIFORMMATRIX4FVPROC			glUniformMatrix4fv = NULL;
	PFNGLVALIDATEPROGRAMPROC			glValidateProgram = NULL; 
	PFNGLVERTEXATTRIB1DPROC				glVertexAttrib1d = NULL; 
	PFNGLVERTEXATTRIB1DVPROC			glVertexAttrib1dv = NULL;
	PFNGLVERTEXATTRIB1FPROC				glVertexAttrib1f = NULL; 
	PFNGLVERTEXATTRIB1FVPROC			glVertexAttrib1fv = NULL;
	PFNGLVERTEXATTRIB1SPROC				glVertexAttrib1s = NULL; 
	PFNGLVERTEXATTRIB1SVPROC			glVertexAttrib1sv = NULL;
	PFNGLVERTEXATTRIB2DPROC				glVertexAttrib2d = NULL; 
	PFNGLVERTEXATTRIB2DVPROC			glVertexAttrib2dv = NULL;
	PFNGLVERTEXATTRIB2FPROC				glVertexAttrib2f = NULL;
	PFNGLVERTEXATTRIB2FVPROC			glVertexAttrib2fv = NULL;
	PFNGLVERTEXATTRIB2SPROC				glVertexAttrib2s = NULL; 
	PFNGLVERTEXATTRIB2SVPROC			glVertexAttrib2sv = NULL;
	PFNGLVERTEXATTRIB3DPROC				glVertexAttrib3d = NULL; 
	PFNGLVERTEXATTRIB3DVPROC			glVertexAttrib3dv = NULL;
	PFNGLVERTEXATTRIB3FPROC				glVertexAttrib3f = NULL; 
	PFNGLVERTEXATTRIB3FVPROC			glVertexAttrib3fv = NULL;
	PFNGLVERTEXATTRIB3SPROC				glVertexAttrib3s = NULL; 
	PFNGLVERTEXATTRIB3SVPROC			glVertexAttrib3sv = NULL;
	PFNGLVERTEXATTRIB4NBVPROC			glVertexAttrib4Nbv = NULL;
	PFNGLVERTEXATTRIB4NIVPROC			glVertexAttrib4Niv = NULL; 
	PFNGLVERTEXATTRIB4NSVPROC			glVertexAttrib4Nsv = NULL; 
	PFNGLVERTEXATTRIB4NUBPROC			glVertexAttrib4Nub = NULL; 
	PFNGLVERTEXATTRIB4NUBVPROC 			glVertexAttrib4Nubv = NULL;
	PFNGLVERTEXATTRIB4NUIVPROC 			glVertexAttrib4Nuiv = NULL;
	PFNGLVERTEXATTRIB4NUSVPROC 			glVertexAttrib4Nusv = NULL;
	PFNGLVERTEXATTRIB4BVPROC			glVertexAttrib4bv = NULL;
	PFNGLVERTEXATTRIB4DPROC 			glVertexAttrib4d = NULL; 
	PFNGLVERTEXATTRIB4DVPROC			glVertexAttrib4dv = NULL;
	PFNGLVERTEXATTRIB4FPROC				glVertexAttrib4f = NULL; 
	PFNGLVERTEXATTRIB4FVPROC			glVertexAttrib4fv = NULL;
	PFNGLVERTEXATTRIB4IVPROC			glVertexAttrib4iv = NULL;
	PFNGLVERTEXATTRIB4SPROC 			glVertexAttrib4s = NULL; 
	PFNGLVERTEXATTRIB4SVPROC			glVertexAttrib4sv = NULL;
	PFNGLVERTEXATTRIB4UBVPROC			glVertexAttrib4ubv = NULL;
	PFNGLVERTEXATTRIB4UIVPROC			glVertexAttrib4uiv = NULL;
	PFNGLVERTEXATTRIB4USVPROC			glVertexAttrib4usv = NULL;
	PFNGLVERTEXATTRIBPOINTERPROC		glVertexAttribPointer = NULL;
#endif

#ifdef GL_ARB_framebuffer_object
	PFNGLISRENDERBUFFERPROC							glIsRenderbuffer = NULL;
	PFNGLBINDRENDERBUFFERPROC						glBindRenderbuffer = NULL;
	PFNGLDELETERENDERBUFFERSPROC					glDeleteRenderbuffers = NULL;
	PFNGLGENRENDERBUFFERSPROC						glGenRenderbuffers = NULL;
	PFNGLRENDERBUFFERSTORAGEPROC					glRenderbufferStorage = NULL;
	PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC			glRenderbufferStorageMultisample = NULL;
	PFNGLGETRENDERBUFFERPARAMETERIVPROC				glGetRenderbufferParameteriv = NULL;
	PFNGLISFRAMEBUFFERPROC							glIsFramebuffer = NULL;
	PFNGLBINDFRAMEBUFFERPROC						glBindFramebuffer = NULL;
	PFNGLDELETEFRAMEBUFFERSPROC						glDeleteFramebuffers = NULL;
	PFNGLGENFRAMEBUFFERSPROC						glGenFramebuffers = NULL;
	PFNGLCHECKFRAMEBUFFERSTATUSPROC					glCheckFramebufferStatus = NULL;
	PFNGLFRAMEBUFFERTEXTURE1DPROC					glFramebufferTexture1D = NULL;
	PFNGLFRAMEBUFFERTEXTURE2DPROC					glFramebufferTexture2D = NULL;
	PFNGLFRAMEBUFFERTEXTURE3DPROC					glFramebufferTexture3D = NULL;
	PFNGLFRAMEBUFFERRENDERBUFFERPROC				glFramebufferRenderbuffer = NULL;
	PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC	glGetFramebufferAttachmentParameteriv = NULL;
	PFNGLGENERATEMIPMAPPROC							glGenerateMipmap = NULL;
	PFNGLBLITFRAMEBUFFERPROC						glBlitFramebuffer = NULL;
#endif

#ifdef GL_ARB_vertex_array_object
	PFNGLBINDVERTEXARRAYPROC				glBindVertexArray = NULL;
	PFNGLDELETEVERTEXARRAYSPROC				glDeleteVertexArrays = NULL;
	PFNGLGENVERTEXARRAYSPROC				glGenVertexArrays = NULL;
	PFNGLISVERTEXARRAYPROC					glIsVertexArray = NULL;
#endif



#ifdef WIN32
    PFNWGLSWAPINTERVALEXTPROC			wglSwapIntervalEXT = NULL;
#else
    PFNGLXSWAPINTERVALSGIPROC			glXSwapIntervalSGI = NULL;
#endif

// The Microsoft GL Distribution has not made glTexImage3D part of the GL.H core specification
// yet. However I know for certain nVidia distributes GL.H for linux with glTexImage3D as part of
// core spec, so for now this is how this must be done.
#ifdef EXTEND_1_2
	PFNGLTEXIMAGE3DPROC				glTexImage3D = NULL;
#endif

#ifdef EXTEND_1_3
	PFNGLACTIVETEXTUREPROC			glActiveTexture = NULL;
	PFNGLCOMPRESSEDTEXIMAGE2DPROC   glCompressedTexImage2D = NULL;
	PFNGLCLIENTACTIVETEXTUREPROC	glClientActiveTexture = NULL;
#endif


#ifdef GL_ARB_shader_objects
	PFNGLDELETEOBJECTARBPROC			glDeleteObjectARB = NULL;
	PFNGLGETHANDLEARBPROC				glGetHandleARB = NULL;
	PFNGLDETACHOBJECTARBPROC			glDetachObjectARB = NULL;
	PFNGLCREATESHADEROBJECTARBPROC		glCreateShaderObjectARB = NULL;
	PFNGLSHADERSOURCEARBPROC			glShaderSourceARB = NULL;
	PFNGLCOMPILESHADERARBPROC			glCompileShaderARB = NULL;
	PFNGLCREATEPROGRAMOBJECTARBPROC		glCreateProgramObjectARB = NULL;
	PFNGLATTACHOBJECTARBPROC			glAttachObjectARB = NULL;
	PFNGLLINKPROGRAMARBPROC				glLinkProgramARB = NULL;
	PFNGLUSEPROGRAMOBJECTARBPROC		glUseProgramObjectARB = NULL;
	PFNGLVALIDATEPROGRAMARBPROC			glValidateProgramARB = NULL;
	PFNGLUNIFORM1FARBPROC				glUniform1fARB = NULL;
	PFNGLUNIFORM2FARBPROC				glUniform2fARB = NULL;
	PFNGLUNIFORM3FARBPROC				glUniform3fARB = NULL;
	PFNGLUNIFORM4FARBPROC				glUniform4fARB = NULL;
	PFNGLUNIFORM1IARBPROC				glUniform1iARB = NULL;
	PFNGLUNIFORM2IARBPROC				glUniform2iARB = NULL;
	PFNGLUNIFORM3IARBPROC				glUniform3iARB = NULL;
	PFNGLUNIFORM4IARBPROC				glUniform4iARB = NULL;
	PFNGLUNIFORM1FVARBPROC				glUniform1fvARB = NULL;
	PFNGLUNIFORM2FVARBPROC				glUniform2fvARB = NULL;
	PFNGLUNIFORM3FVARBPROC				glUniform3fvARB = NULL;
	PFNGLUNIFORM4FVARBPROC				glUniform4fvARB = NULL;
	PFNGLUNIFORM1IVARBPROC				glUniform1ivARB = NULL;
	PFNGLUNIFORM2IVARBPROC				glUniform2ivARB = NULL;
	PFNGLUNIFORM3IVARBPROC				glUniform3ivARB = NULL;
	PFNGLUNIFORM4IVARBPROC				glUniform4ivARB = NULL;
	PFNGLUNIFORMMATRIX2FVARBPROC		glUniformMatrix2fvARB = NULL;
	PFNGLUNIFORMMATRIX3FVARBPROC		glUniformMatrix3fvARB = NULL;
	PFNGLUNIFORMMATRIX4FVARBPROC		glUniformMatrix4fvARB = NULL;
	PFNGLGETOBJECTPARAMETERFVARBPROC	glGetObjectParameterfvARB = NULL;
	PFNGLGETOBJECTPARAMETERIVARBPROC	glGetObjectParameterivARB = NULL;
	PFNGLGETINFOLOGARBPROC				glGetInfoLogARB = NULL;
	PFNGLGETATTACHEDOBJECTSARBPROC		glGetAttachedObjectsARB = NULL;
	PFNGLGETUNIFORMLOCATIONARBPROC		glGetUniformLocationARB = NULL;
	PFNGLGETACTIVEUNIFORMARBPROC		glGetActiveUniformARB = NULL;
	PFNGLGETUNIFORMFVARBPROC			glGetUniformfvARB = NULL;
	PFNGLGETUNIFORMIVARBPROC			glGetUniformivARB = NULL;
	PFNGLGETSHADERSOURCEARBPROC			glGetShaderSourceARB = NULL;
#endif 

PFNGLDRAWBUFFERSARBPROC				glDrawBuffersARB = NULL;




////////////////////////////////////////////////////////////////////////
//
// SMS- 03/10/06
//
// initializes openGL extensions ...should be called before any
// renderer specific calls in the main function's initialization function
//
// not for use with D3D renderers
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// SMS- 10/21/04
//
// isExtensionSupported (char* targetExtension) - checks the extension string
// with glGetString for the target extension string
//
///////////////////////////////////////////////////////////////////////////////
bool isExtensionSupported(const char* targetExtension)
{
	const unsigned char* pszExt = NULL;
	const unsigned char* start;
	unsigned char* where, *terminator;

	where = (unsigned char*)strchr(targetExtension, ' ');
	if(where || *targetExtension == '\0')
		return false;

	pszExt = glGetString(GL_EXTENSIONS);

	start = pszExt;
	for(;;)
	{
		where = (unsigned char*)strstr((const char*)start, targetExtension);
		if(!where)
			break;

		terminator = where + strlen(targetExtension);
		if(where == start || *(where - 1) == ' ')
			if(*terminator == ' ' || *terminator == '\0')
				return true;

		start = terminator;
	}

	return false;
}



/*
    getGLProcAddress
    Obtains the function procedure address for windows and X
    systems
*/
static void* getGLProcAddress(const char* proc)
{
    #ifdef WIN32
        return wglGetProcAddress(proc);
    #else
        return (void*)(glXGetProcAddressARB((const GLubyte*)proc));
    #endif
}



/*
    initExtensions

    Should be called only one time prior to any dependent draw calls.
    Queries the extension or version availability and defines the
    function
*/
bool initExtensions()
{
	#ifdef EXTEND_1_5
		if(atof((char*)glGetString(GL_VERSION)) >= 1.5)
		{
			EXT_VERTEX_BUFFER_OBJECT = true;

			glGenBuffers = (PFNGLGENBUFFERSPROC)getGLProcAddress("glGenBuffers");
			glBindBuffer = (PFNGLBINDBUFFERPROC)getGLProcAddress("glBindBuffer");
			glBufferData = (PFNGLBUFFERDATAPROC)getGLProcAddress("glBufferData");
			glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)getGLProcAddress("glDeleteBuffers");

			glMapBuffer = (PFNGLMAPBUFFERPROC)getGLProcAddress("glMapBuffer");
			glUnmapBuffer = (PFNGLUNMAPBUFFERPROC)getGLProcAddress("glUnmapBuffer");
			glGetBufferParameteriv = (PFNGLGETBUFFERPARAMETERIVPROC)getGLProcAddress("glGetBufferParameteriv");
		}
	#endif


	#ifdef EXTEND_2_0
		if(atof((char*)glGetString(GL_VERSION)) >= 2.0)
		{
			glAttachShader = (PFNGLATTACHSHADERPROC)getGLProcAddress("glAttachShader");
			glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)getGLProcAddress("glBindAttribLocation");
			glCompileShader	= (PFNGLCOMPILESHADERPROC)getGLProcAddress("glCompileShader");
			glCreateProgram	= (PFNGLCREATEPROGRAMPROC)getGLProcAddress("glCreateProgram");
			glCreateShader	= (PFNGLCREATESHADERPROC)getGLProcAddress("glCreateShader");
			glDeleteProgram	= (PFNGLDELETEPROGRAMPROC)getGLProcAddress("glDeleteProgram");
			glDeleteShader	= (PFNGLDELETESHADERPROC)getGLProcAddress("glDeleteShader");
			glDetachShader	= (PFNGLDETACHSHADERPROC)getGLProcAddress("glDetachShader");
			glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)getGLProcAddress("glEnableVertexAttribArray"); 
			glDisableVertexAttribArray	= (PFNGLDISABLEVERTEXATTRIBARRAYPROC)getGLProcAddress("glDisableVertexAttribArray"); 
			glGetActiveAttrib = (PFNGLGETACTIVEATTRIBPROC)getGLProcAddress("glGetActiveAttrib");
			glGetActiveUniform	= (PFNGLGETACTIVEUNIFORMPROC)getGLProcAddress("glGetActiveUniform");
			glGetAttribLocation	= (PFNGLGETATTRIBLOCATIONPROC)getGLProcAddress("glGetAttribLocation");
			glGetAttachedShaders = (PFNGLGETATTACHEDSHADERSPROC)getGLProcAddress("glGetAttachedShaders");
			glGetProgramiv = (PFNGLGETPROGRAMIVPROC)getGLProcAddress("glGetProgramiv");
			glGetProgramInfoLog	= (PFNGLGETPROGRAMINFOLOGPROC)getGLProcAddress("glGetProgramInfoLog");
			glGetShaderiv = (PFNGLGETSHADERIVPROC)getGLProcAddress("glGetShaderiv");
			glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)getGLProcAddress("glGetShaderInfoLog");
			glGetShaderSource = (PFNGLGETSHADERSOURCEPROC)getGLProcAddress("glGetShaderSource");
			glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)getGLProcAddress("glGetUniformLocation");
			glGetUniformfv = (PFNGLGETUNIFORMFVPROC)getGLProcAddress("glGetUniformfv");
			glGetUniformiv = (PFNGLGETUNIFORMIVPROC)getGLProcAddress("glGetUniformiv");
			glGetVertexAttribdv = (PFNGLGETVERTEXATTRIBDVPROC)getGLProcAddress("glGetVertexAttribdv");
			glGetVertexAttribfv = (PFNGLGETVERTEXATTRIBFVPROC)getGLProcAddress("glGetVertexAttribfv");
			glGetVertexAttribiv = (PFNGLGETVERTEXATTRIBIVPROC)getGLProcAddress("glGetVertexAttribiv");
			glIsProgram = (PFNGLISPROGRAMPROC)getGLProcAddress("glIsProgram");
			glIsShader = (PFNGLISSHADERPROC)getGLProcAddress("glIsShader");
			glLinkProgram = (PFNGLLINKPROGRAMPROC)getGLProcAddress("glLinkProgram"); 
			glShaderSource = (PFNGLSHADERSOURCEPROC)getGLProcAddress("glShaderSource");
			glUseProgram = (PFNGLUSEPROGRAMPROC)getGLProcAddress("glUseProgram");
			glUniform1f = (PFNGLUNIFORM1FPROC)getGLProcAddress("glUniform1f"); 
			glUniform2f =	(PFNGLUNIFORM2FPROC)getGLProcAddress("glUniform2f"); 
			glUniform3f =	(PFNGLUNIFORM3FPROC)getGLProcAddress("glUniform3f"); 
			glUniform4f =	(PFNGLUNIFORM4FPROC)getGLProcAddress("glUniform4f"); 
			glUniform1i =	(PFNGLUNIFORM1IPROC)getGLProcAddress("glUniform1i"); 
			glUniform2i =	(PFNGLUNIFORM2IPROC)getGLProcAddress("glUniform2i"); 
			glUniform3i =	(PFNGLUNIFORM3IPROC)getGLProcAddress("glUniform3i"); 
			glUniform4i =	(PFNGLUNIFORM4IPROC)getGLProcAddress("glUniform4i"); 
			glUniform1fv =	(PFNGLUNIFORM1FVPROC)getGLProcAddress("glUniform1fv");
			glUniform2fv =	(PFNGLUNIFORM2FVPROC)getGLProcAddress("glUniform2fv");
			glUniform3fv =	(PFNGLUNIFORM3FVPROC)getGLProcAddress("glUniform3fv");
			glUniform4fv =	(PFNGLUNIFORM3FVPROC)getGLProcAddress("glUniform4fv");
			glUniform1iv =	(PFNGLUNIFORM1IVPROC)getGLProcAddress("glUniform1iv");
			glUniform2iv =	(PFNGLUNIFORM2IVPROC)getGLProcAddress("glUniform2iv");
			glUniform3iv =	(PFNGLUNIFORM3IVPROC)getGLProcAddress("glUniform3iv");
			glUniform4iv =	(PFNGLUNIFORM4IVPROC)getGLProcAddress("glUniform4iv");
			glUniformMatrix2fv = (PFNGLUNIFORMMATRIX2FVPROC)getGLProcAddress("glUniformMatrix2fv");
			glUniformMatrix3fv = (PFNGLUNIFORMMATRIX3FVPROC)getGLProcAddress("glUniformMatrix3fv");
			glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)getGLProcAddress("glUniformMatrix4fv");
			glValidateProgram = (PFNGLVALIDATEPROGRAMPROC)getGLProcAddress("glValidateProgram"); 
			glVertexAttrib1d = (PFNGLVERTEXATTRIB1DPROC)getGLProcAddress("glVertexAttrib1d"); 
			glVertexAttrib1dv = (PFNGLVERTEXATTRIB1DVPROC)getGLProcAddress("glVertexAttrib1dv");
			glVertexAttrib1f =  (PFNGLVERTEXATTRIB1FPROC)getGLProcAddress("glVertexAttrib1f");
			glVertexAttrib1fv = (PFNGLVERTEXATTRIB1FVPROC)getGLProcAddress("glVertexAttrib1fv");
			glVertexAttrib1s =  (PFNGLVERTEXATTRIB1SPROC)getGLProcAddress("glVertexAttrib1s");
			glVertexAttrib1sv = (PFNGLVERTEXATTRIB1SVPROC)getGLProcAddress("glVertexAttrib1sv");
			glVertexAttrib2d =  (PFNGLVERTEXATTRIB2DPROC)getGLProcAddress("glVertexAttrib2d");
			glVertexAttrib2dv = (PFNGLVERTEXATTRIB2DVPROC)getGLProcAddress("glVertexAttrib2dv");
			glVertexAttrib2f =  (PFNGLVERTEXATTRIB2FPROC)getGLProcAddress("glVertexAttrib2f");
			glVertexAttrib2fv = (PFNGLVERTEXATTRIB2FVPROC)getGLProcAddress("glVertexAttrib2fv");
			glVertexAttrib2s =  (PFNGLVERTEXATTRIB2SPROC)getGLProcAddress("glVertexAttrib2s");
			glVertexAttrib2sv = (PFNGLVERTEXATTRIB2SVPROC)getGLProcAddress("glVertexAttrib2sv");
			glVertexAttrib3d =  (PFNGLVERTEXATTRIB3DPROC)getGLProcAddress("glVertexAttrib3d");
			glVertexAttrib3dv = (PFNGLVERTEXATTRIB3DVPROC)getGLProcAddress("glVertexAttrib3dv");
			glVertexAttrib3f =  (PFNGLVERTEXATTRIB3FPROC)getGLProcAddress("glVertexAttrib3f");
			glVertexAttrib3fv = (PFNGLVERTEXATTRIB3FVPROC)getGLProcAddress("glVertexAttrib3fv");
			glVertexAttrib3s =  (PFNGLVERTEXATTRIB3SPROC)getGLProcAddress("glVertexAttrib3s");
			glVertexAttrib3sv = (PFNGLVERTEXATTRIB3SVPROC)getGLProcAddress("glVertexAttrib3sv");
			glVertexAttrib4Nbv = (PFNGLVERTEXATTRIB4NBVPROC)getGLProcAddress("glVertexAttrib4Nbv");
			glVertexAttrib4Niv = (PFNGLVERTEXATTRIB4NIVPROC)getGLProcAddress("glVertexAttrib4Niv");
			glVertexAttrib4Nsv = (PFNGLVERTEXATTRIB4NSVPROC)getGLProcAddress("glVertexAttrib4Nsv");
			glVertexAttrib4Nub = (PFNGLVERTEXATTRIB4NUBPROC)getGLProcAddress("glVertexAttrib4Nuv");
			glVertexAttrib4Nubv = (PFNGLVERTEXATTRIB4NUBVPROC)getGLProcAddress("glVertexAttrib4Nubv");
			glVertexAttrib4Nuiv = (PFNGLVERTEXATTRIB4NUIVPROC)getGLProcAddress("glVertexAttrib4Nuiv");
			glVertexAttrib4Nusv = (PFNGLVERTEXATTRIB4NUSVPROC)getGLProcAddress("glVertexAttrib4Nusv");
			glVertexAttrib4bv = (PFNGLVERTEXATTRIB4BVPROC)getGLProcAddress("glVertexAttrib4bv");
			glVertexAttrib4d = (PFNGLVERTEXATTRIB4DPROC)getGLProcAddress("glVertexAttrib4d");
			glVertexAttrib4dv = (PFNGLVERTEXATTRIB4DVPROC)getGLProcAddress("glVertexAttrib4dv");
			glVertexAttrib4f = (PFNGLVERTEXATTRIB4FPROC)getGLProcAddress("glVertexAttrib4f");
			glVertexAttrib4fv = (PFNGLVERTEXATTRIB4FVPROC)getGLProcAddress("glVertexAttrib4fv");
			glVertexAttrib4iv = (PFNGLVERTEXATTRIB4IVPROC)getGLProcAddress("glVertexAttrib4iv");
			glVertexAttrib4s = (PFNGLVERTEXATTRIB4SPROC)getGLProcAddress("glVertexAttrib4s");
			glVertexAttrib4sv = (PFNGLVERTEXATTRIB4SVPROC)getGLProcAddress("glVertexAttrib4sv");
			glVertexAttrib4ubv = (PFNGLVERTEXATTRIB4UBVPROC)getGLProcAddress("glVertexAttrib4ubv");
			glVertexAttrib4uiv = (PFNGLVERTEXATTRIB4UIVPROC)getGLProcAddress("glVertexAttrib4uiv");
			glVertexAttrib4usv = (PFNGLVERTEXATTRIB4USVPROC)getGLProcAddress("glVertexAttrib4usv");
			glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)getGLProcAddress("glVertexAttribPointer");
		}
	#endif

	if(isExtensionSupported("GL_SCIS_generate_mipmap"))
	{
		EXT_SGIS_MIPMAP = true;
	}


//	if(isExtensionSupported("GL_ARB_half_float_pixel"))
//	{
//		EXT_HALF_FLOAT = true;
//	}

//	glTexImage3D = (PFNGLTEXIMAGE3DPROC)getGLProcAddress("glTexImage3D");


	#ifdef GL_ARB_framebuffer_object
		if(isExtensionSupported("GL_ARB_framebuffer_object"))
		{
			EXT_FRAMEBUFFER_OBJECT = true;

			glIsRenderbuffer = (PFNGLISRENDERBUFFERPROC)getGLProcAddress("glIsRenderbuffer");
			glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC)getGLProcAddress("glBindRenderbuffer");
			glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC)getGLProcAddress("glDeleteRenderbuffers");
			glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC)getGLProcAddress("glGenRenderbuffers");
			glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC)getGLProcAddress("glRenderbufferStorage");
			glRenderbufferStorageMultisample = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC)getGLProcAddress("glRenderbufferStorageMultisample");
			glGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIVPROC)getGLProcAddress("glGetRenderbufferParameteriv");
			glIsFramebuffer = (PFNGLISFRAMEBUFFERPROC)getGLProcAddress("glIsFramebuffer");
			glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)getGLProcAddress("glBindFramebuffer");
			glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)getGLProcAddress("glDeleteFramebuffers");
			glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)getGLProcAddress("glGenFramebuffers");
			glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)getGLProcAddress("glCheckFramebufferStatus");
			glFramebufferTexture1D = (PFNGLFRAMEBUFFERTEXTURE1DPROC)getGLProcAddress("glFramebufferTexture1D");
			glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)getGLProcAddress("glFramebufferTexture2D");
			glFramebufferTexture3D = (PFNGLFRAMEBUFFERTEXTURE3DPROC)getGLProcAddress("glFramebufferTexture3D");
			glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)getGLProcAddress("glFramebufferRenderbuffer");
			glGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC)getGLProcAddress("glGetFramebufferAttachmentParameteriv");
			glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)getGLProcAddress("glGenerateMipmap");
			glBlitFramebuffer = (PFNGLBLITFRAMEBUFFERPROC)getGLProcAddress("glBlitFramebuffer");
		}
	#endif

	#ifdef GL_ARB_vertex_array_object
		if(isExtensionSupported("GL_ARB_vertex_array_object"))
		{
			glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)getGLProcAddress("glBindVertexArray");
			glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)getGLProcAddress("glDeleteVertexArrays");
			glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)getGLProcAddress("glGenVertexArrays");
			glIsVertexArray = (PFNGLISVERTEXARRAYPROC)getGLProcAddress("glIsVertexArray");
		}
	#endif


/*
	if(isExtensionSupported("GL_ARB_texture_compression"))
	{
		EXT_TEXTURE_COMPRESSION = TRUE;

		glCompressedTexImage3DARB = (PFNGLCOMPRESSEDTEXIMAGE3DARBPROC)getGLProcAddress("glCompressedTexImage3DARB");
		glCompressedTexImage2DARB = (PFNGLCOMPRESSEDTEXIMAGE2DARBPROC)getGLProcAddress("glCompressedTexImage2DARB");
		glCompressedTexImage1DARB = (PFNGLCOMPRESSEDTEXIMAGE1DARBPROC)getGLProcAddress("glCompressedTexImage1DARB");
		glCompressedTexSubImage3DARB = (PFNGLCOMPRESSEDTEXSUBIMAGE3DARBPROC)getGLProcAddress("glCompressedTexSubImage3DARB");
		glCompressedTexSubImage2DARB = (PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC)getGLProcAddress("glCompressedTexSubImage2DARB");
		glCompressedTexSubImage1DARB = (PFNGLCOMPRESSEDTEXSUBIMAGE1DARBPROC)getGLProcAddress("glCompressedTexSubImage1DARB");
		glGetCompressedTexImageARB = (PFNGLGETCOMPRESSEDTEXIMAGEARBPROC)getGLProcAddress("glGetCompressedTexImageARB");
	}

	if(isExtensionSupported("GL_EXT_texture_compression_s3tc"))
	{
		EXT_TEXTURE_COMPRESSION_S3TC = TRUE;
	}

	if(isExtensionSupported("GL_ARB_multitexture"))
	{
		EXT_MULTITEXTURE = true;

		glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)getGLProcAddress("glActiveTextureARB");
		glClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC)getGLProcAddress("glClientActiveTextureARB");
	}
*/

    #ifdef WIN32
        if(isExtensionSupported("WGL_EXT_swap_control"))
        {
            EXT_VSYNC = true;
            wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)getGLProcAddress("wglSwapIntervalEXT");
        }
    #else
        if(isExtensionSupported("GLX_SGI_swap_control"))
        {
            EXT_VSYNC = true;
            glXSwapIntervalSGI = (PFNGLXSWAPINTERVALSGIPROC)getGLProcAddress("glXSwapIntervalSGI");
        }
    #endif

    #if defined (EXTEND_1_2)
		if(atof((char*)glGetString(GL_VERSION)) >= 1.2)
			glTexImage3D = (PFNGLTEXIMAGE3DPROC)getGLProcAddress("glTexImage3D");
	#endif

	// For windows initialize GL 1.3 functions //
	#ifdef EXTEND_1_3
		if(atof((char*)glGetString(GL_VERSION)) >= 1.3)
		{
			glActiveTexture = (PFNGLACTIVETEXTUREPROC)getGLProcAddress("glActiveTexture");
			glCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC)getGLProcAddress("glCompressedTexImage2D");
			glClientActiveTexture = (PFNGLCLIENTACTIVETEXTUREPROC)getGLProcAddress("glClientActiveTexture");
		}
	#endif

	#ifdef GL_ARB_shader_objects
		if(isExtensionSupported("GL_ARB_shader_objects"))
		{
			EXT_SHADER_OBJECTS = true;

			glDeleteObjectARB = (PFNGLDELETEOBJECTARBPROC)getGLProcAddress("glDeleteObjectARB");
			glGetHandleARB = (PFNGLGETHANDLEARBPROC)getGLProcAddress("glGetHandleARB");
			glDetachObjectARB = (PFNGLDETACHOBJECTARBPROC)getGLProcAddress("glDetachObjectARB");
			glCreateShaderObjectARB = (PFNGLCREATESHADEROBJECTARBPROC)getGLProcAddress("glCreateShaderObjectARB");
			glShaderSourceARB = (PFNGLSHADERSOURCEARBPROC)getGLProcAddress("glShaderSourceARB");
			glCompileShaderARB = (PFNGLCOMPILESHADERARBPROC)getGLProcAddress("glCompileShaderARB");
			glCreateProgramObjectARB = (PFNGLCREATEPROGRAMOBJECTARBPROC)getGLProcAddress("glCreateProgramObjectARB");
			glAttachObjectARB = (PFNGLATTACHOBJECTARBPROC)getGLProcAddress("glAttachObjectARB");
			glLinkProgramARB = (PFNGLLINKPROGRAMARBPROC)getGLProcAddress("glLinkProgramARB");
			glUseProgramObjectARB = (PFNGLUSEPROGRAMOBJECTARBPROC)getGLProcAddress("glUseProgramObjectARB");
			glValidateProgramARB = (PFNGLVALIDATEPROGRAMARBPROC)getGLProcAddress("glValidateProgramARB");
			glUniform1fARB = (PFNGLUNIFORM1FARBPROC)getGLProcAddress("glUniform1fARB");
			glUniform2fARB = (PFNGLUNIFORM2FARBPROC)getGLProcAddress("glUniform2fARB");
			glUniform3fARB = (PFNGLUNIFORM3FARBPROC)getGLProcAddress("glUniform3fARB");
			glUniform4fARB = (PFNGLUNIFORM4FARBPROC)getGLProcAddress("glUniform4fARB");
			glUniform1iARB = (PFNGLUNIFORM1IARBPROC)getGLProcAddress("glUniform1iARB");
			glUniform2iARB = (PFNGLUNIFORM2IARBPROC)getGLProcAddress("glUniform2iARB");
			glUniform3iARB = (PFNGLUNIFORM3IARBPROC)getGLProcAddress("glUniform3iARB");
			glUniform4iARB = (PFNGLUNIFORM4IARBPROC)getGLProcAddress("glUniform4iARB");
			glUniform1fvARB = (PFNGLUNIFORM1FVARBPROC)getGLProcAddress("glUniform1fvARB");
			glUniform2fvARB = (PFNGLUNIFORM2FVARBPROC)getGLProcAddress("glUniform2fvARB");
			glUniform3fvARB = (PFNGLUNIFORM3FVARBPROC)getGLProcAddress("glUniform3fvARB");
			glUniform4fvARB = (PFNGLUNIFORM4FVARBPROC)getGLProcAddress("glUniform4fvARB");
			glUniform1ivARB = (PFNGLUNIFORM1IVARBPROC)getGLProcAddress("glUniform1ivARB");
			glUniform2ivARB = (PFNGLUNIFORM2IVARBPROC)getGLProcAddress("glUniform2ivARB");
			glUniform3ivARB = (PFNGLUNIFORM3IVARBPROC)getGLProcAddress("glUniform3ivARB");
			glUniform4ivARB = (PFNGLUNIFORM4IVARBPROC)getGLProcAddress("glUniform4ivARB");
			glUniformMatrix2fvARB = (PFNGLUNIFORMMATRIX2FVARBPROC)getGLProcAddress("glUniformMatrix2fvARB");
			glUniformMatrix3fvARB = (PFNGLUNIFORMMATRIX3FVARBPROC)getGLProcAddress("glUniformMatrix3fvARB");
			glUniformMatrix4fvARB = (PFNGLUNIFORMMATRIX4FVARBPROC)getGLProcAddress("glUniformMatrix4fvARB");
			glGetObjectParameterfvARB = (PFNGLGETOBJECTPARAMETERFVARBPROC)getGLProcAddress("glGetObjectParameterfvARB");
			glGetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIVARBPROC)getGLProcAddress("glGetObjectParameterivARB");
			glGetInfoLogARB = (PFNGLGETINFOLOGARBPROC)getGLProcAddress("glGetInfoLogARB");
			glGetAttachedObjectsARB = (PFNGLGETATTACHEDOBJECTSARBPROC)getGLProcAddress("glGetAttachedObjectsARB");
			glGetUniformLocationARB = (PFNGLGETUNIFORMLOCATIONARBPROC)getGLProcAddress("glGetUniformLocationARB");
			glGetActiveUniformARB = (PFNGLGETACTIVEUNIFORMARBPROC)getGLProcAddress("glGetActiveUniformARB");
			glGetUniformfvARB = (PFNGLGETUNIFORMFVARBPROC)getGLProcAddress("glGetUniformfvARB");
			glGetUniformivARB = (PFNGLGETUNIFORMIVARBPROC)getGLProcAddress("glGetUniformivARB");
			glGetShaderSourceARB = (PFNGLGETSHADERSOURCEARBPROC)getGLProcAddress("glGetShaderSourceARB");
		}
	#endif

//	if(isExtensionSupported("GL_EXT_texture_filter_anisotropic"))
//	{
//		EXT_ANISOTROPIC = true;
//	}

	if(isExtensionSupported("GL_ARB_draw_buffers"))
	{
		EXT_DRAW_BUFFERS = true;
		glDrawBuffersARB = (PFNGLDRAWBUFFERSARBPROC)getGLProcAddress("glDrawBuffersARB");
	}

	return true;
}
