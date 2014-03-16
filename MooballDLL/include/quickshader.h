//////////////////////////////////////////////////////////////////////////////
//
// QUICKSHADER.H
//
// Adapted for Mooball project 12/09
//
// This header contains basic GLSL shader loading, parameter passing, and
// texture binding.
//
// Anything immediately shader related can be placed here
//
// see: http://googlecode.com/p/mooball
//
///////////////////////////////////////////////////////////////////////////////

#include "quickrender.h"

#ifndef __QUICKSHADER_H_
#define	__QUICKSHADER_H_

#include "quickhash.h"
#include <stdio.h>




#define MAX_UNIFORM_LEN			32
#define MAX_TEXTURE_UNITS       16
#define MAX_SHADERS				256


const unsigned int		MATRIX_MODELVIEW			= 0x00000001;
const unsigned int		MATRIX_PROJECTION			= 0x00000002;

const unsigned int		MATRIX_IDENTITY				= 0x00000010;
const unsigned int		MATRIX_TRANSPOSE			= 0x00000020;
const unsigned int		MATRIX_INVERSE				= 0x00000040;
const unsigned int		MATRIX_INVERSETRANSPOSE		= 0x00000080;



/*
    CShaderObject (Should be used in favor of CShaderInterface ("g_pShaderInterface"))


    This class comes to encapsulate vertex and fragment programs for
    OpenGL. Shaders are uploaded as text source, compiled into bytecode
    and uploaded to the GPU.

*/

class CShaderObject
{
    public:

        CShaderObject();
        ~CShaderObject();

        /*
            Load

            Loads a shader program from vertex and fragment source.
            Both must be specified and have appropriate extensions.
            (.vert) for vertex program (.frag) for fragment program

            "vertFileName" - path and filename of the vertex shader
            "fragFileName" - path and filename of the fragment shader

            Returns true on success false on fail
        */
        bool        Load(const char* vertFileName, const char* fragFileName);


        /*
            Bind

            Bind the shader object as the current render state.
            Must call "Unbind" when you wish to reset render state.
            This function will also forcefully evict any prior bound shader objects.

            Returns true on success false on fail
        */
        bool        Bind();

        /*
            Unbind

            Unbind the shader object and restore prior render state.
        */
        void        Unbind();


        /*
            BindTexture

            Binds a texture as a parameter to the bound shader object.

            "paramName" - Name of the parameter in the shader
            "texHandle" - Valid texture handle
        */
        bool        BindTexture(const char* paramName, int texHandle);

        /*
            EvictTextures

            Forcefully evict bound textures
        */
        void        EvictTextures();

        /*
            BindUniformFloat

            Binds a float as a constant parameter to the bound shader object

            "paramName" - Name of the parameter in the shader
            "f" - float ref
            "nFloats" - number of floats in "f"
        */
        void        BindUniformFloat(const char* paramName, const float* f, int nFloats);


        /*
            BindUniformInt

            Binds an integer as a constant parameter to the bound shader object

            Same parameters as above except with integer type
        */
        void        BindUniformInt(const char* paramName, const int* i, int nInts);


        /*
            BindUniformVec2Array

            Binds several arrays of vec2 objects which are 2-float arrays

            "paramName" - Name of parameter in the shader
            "v" - vec2 array
            "nVecs" - number of vectors in the array
        */
        void        BindUniformVec2Array(const char* paramName, const vec2* v, int nVecs);


        /*
            BindUniformVec4Array

            Binds several arrays of vec4 objects which are 4-float arrays

            "paramName" - Name of parameter in the shader
            "v" - vec4 array
            "nVecs" - number of vectors in the array
        */
        void        BindUniformVec4Array(const char* paramName, const vec4* v, int nVecs);


        /*
            BindStateMatrix

            Binds the current state matrix given by the flags at the top of this header.

            "paramName" - Name of the parameter in the shader
            "matrix" - flag combination of the state to set
        */
        void        BindStateMatrix(const char* paramName, unsigned int matrix);


        /*
            GetShaderHandle

            Retrieve the shader object handle
        */
        const inline GLuint     GetShaderHandle() { return m_shaderObject; }

		/*
			GetAttribLoc

			Gets the location of the attribute specified by a string
		*/
		GLint GetAttribLoc(const char* attribName);

        /*
            IsBound

            Query that the shader object is currently bound as current render state
        */
        const inline bool       IsBound() { return m_bIsBound; }

    protected:

    private:

		void		HashSamplers();
		void		HashAttributes();

		CHashTable	<GLint> m_samplerHash;							// Hash of texture sampler to sampler name
		CHashTable	<GLint> m_attributeHash;	

        bool        m_bIsBound;                                     // Bind status of the object
        GLuint      m_shaderObject;                                 // Shader object handle
};





/*
    CShaderInterface (Primary point of interaction for shaders)


    This object is the primary point of interaction for the programmer into the
	shader state.

	Maintains an active list of compiled shaders and the currently bound shader as well
	as any state associated with it.

*/

class CShaderInterface
{
	public:

		CShaderInterface();
		~CShaderInterface();


		/*
			AddShader

			Add a shader, which consists of a vertex program and a fragment program
			(.vert and .frag respectively). This compiles the shader and loads it
			into the resource manager

			"vertProg" - string of the path and file name of the vertex program
			"pixelProg" - string of the path and file name of the pixel program
		*/
		int		AddShader( const std::string& vertProg, const std::string& pixelProg );

		/*
			DeleteShader

			Remove a shader and release resources from the resource manager.

			"handle" - handle returned from "AddShader" of the program to be released
		*/
		void	DeleteShader( int handle );


		/*
			BindShader

			Bind a shader to the current render state. All subsequent draw calls
			and shader operations are thenceforth conducted via this shader until it
			is unbound

			"handle" - handle returned from "AddShader" of the program to be bound
		*/
		bool	BindShader( int handle );

		/*
			UnbindShader

			Unloads the currently bound shader from the current render state.
			This also evicts all bound texture units
		*/
		void	UnbindShader();

		/*
			SetStateMatrix

			Set a state matrix and upload to the currently bound shader state

			"name" - name of the shader to set within the program
			"mat" - combination of flags of the matrix state to set
		*/
		bool	SetStateMatrix( const std::string& name, unsigned int mat );


		/*
			SetFloat

			Set a float or float array and upload to the currently bound shader state

			"name" - name of the object within the shader
			"f" - pointer to float array (or float)
			"nFloats" - number of elements in "f"
		*/
		bool	SetFloat( const std::string& name, float* f, int nFloats );


		/*
			SetInt

			Set an int or an int array and upload to the currently bound shader

			"name" - name of the object within the shader
			"i" - pointer to int array (or int)
			"nInts" - number of elements in "i"
		*/
		bool	SetInt( const std::string& name, int* i, int nInts );


		/*
			SetTexture

			Set a texture and upload to the currently bound shader state
			This is unloaded on calls to "UnbindShader"

			"name" - name of the texture object within the shader
			"texHandle" - handle to a valid texture
		*/
		bool	SetTexture( const std::string& name, int texHandle );


		/*
			GetCurrentShaderIndex

			Obtain the handle to the currently bound shader
			Returns INVALID_HANDLE if no shader is bound
		*/
		int		GetCurrentShaderIndex();


		/*
			IsShaderBound

			Query whether a shader is bound to the current state.
		*/
		bool	IsShaderBound();


		/*
			GetAttribLoc

			Obtain the attribute bind location given by a string in the currently 
			bound shader
		*/
		GLint	GetAttribLoc(const char* attribName);


	private:

		bool		m_bIsShaderBound;

		std::vector <CShaderObject*>*	m_vShaderResources;		// Container for shader resources

		int				m_curShader;				// handle to the currently bound shader
		unsigned int	m_nShaders;					// current count of shaders

		CShaderObject*		m_pCurShader;		// the currently bound shader ptr
};



extern CShaderInterface*		g_pShaderInterface;

#endif
