#include "extensions.h"



/////////////////////////////////////////////////////////////////////////////////////
//
// 3DSMODEL.H
//
// .3DS File parser adapted for Mooball project
//
// Contains chunk types and load routines for parsing an Autodesk 3D Studio Max .3ds
// model object.
//
// see: http://googlecode.com/p/mooball
//
/////////////////////////////////////////////////////////////////////////////////////


#ifndef   __QUICK3DS_H_
#define   __QUICK3DS_H_


#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>

#include "quickmath.h"
#include "quickimage.h"
#include "quickgeom.h"
#include "quickshader.h"


/*

#pragma pack(push)
#pragma pack(1)


// basic 3ds chunk type //
typedef struct
{
	unsigned short id;			// id of chunk
	unsigned int  length;		// length in bytes of chunk  (ulong)
} chunk_3ds;

// 3ds material chunk type //
typedef struct
{
	char			name[32];		// name of material
	float			ambient[4];		// ambient color
	float			diffuse[4];		// difffuse color
	float			specular[4];	// specular color
	float			emissive[4];	// emissive color
	float			shininess;		// phong model exponent
	char			texture[32];	// texture name
} chunk_material3ds;

// 3ds group chunk type //
typedef struct
{
	char		name[32];		// group name
	int			mat;			// material index
	int			start;			// starting index
	int			size;			// length in bytes of group
	int 		*tris;			// poly list (long)
} chunk_group3ds;

// 3ds mesh type //
typedef struct
{
	char			name[32];				// name of mesh
	int				vertCount;				// vertexCount
	int				texCoordCount;			// texture coordinate count
	int				triCount;				// triangle count
	int				groupCount;				// number of chunk_group3ds's
	float			(*verts)[3];			// vertex list
	float			(*norms)[3];			// normal list
	float			(*texCoords)[2];		// texture coordinate list
	float			(*tangentSpace)[9];		// tangent space vectors
	int 			(*tris)[3];				// poly list (long)
	chunk_group3ds  *groups;				// group list
	int 			*smooth;				// smoothing group list (long)
	float			axis[3][3];				// local axis definition
	float			position[3];			// world space pos
	float			min[3];					// bb mins
	float			max[3];					// bb maxs
	float			center[3];				// center locally
} chunk_mesh3ds;

// main 3ds data type //
typedef struct
{
	int					 materialCount;		// num of materials
	int					 meshCount;			// num of meshes
	int					 vertCount;			// num of verts
	int					 triCount;			// triangle count
	chunk_material3ds*	 materials;			// material list
	chunk_mesh3ds*		 meshes;			// mesh list
	float				 min[3];			// bb mins
	float				 max[3];			// bb max
	float				 center[3];			// model center
	float				 masterScale;		// master scaling
} chunk_data3ds;

#pragma pack(pop)

*/



/*
	3DS Vertex format
*/

/*
typedef struct
{
    vec3    wPos;       // World position
    vec3    wNorm;      // World space normal
    vec3    wTan;       // World space tangent
    vec2    tex;        // Base texture coordinate pair

    unsigned int    emissive;       // Emissive color

} s_3ds_vertex_format;

*/

/*
	3DS Mesh encapsulation

	Contains vertex buffer handle, index buffer handle,
	and texture handle
*/

/*
typedef struct
{
	GLuint		m_vboID;
	GLuint		m_iboID;

	int			m_textureHandle;

} s_3ds_mesh;
*/
/*
    CModel3DS

    Provides an interface for the 3DS loading routines within this header.
    This class will allow you to query file name, bounding boxes, material properties,
    and access to geometry.

    Adapted for Mooball project
*/

/*
class CModel3DS
{
    public:

        CModel3DS();
        ~CModel3DS();


        bool Load(const char* fName);

        void TestRender();

    protected:

    private:

        chunk_data3ds           m_modelData;            // Primary model data
        std::string             m_fileName;             // File name

        bool                    m_bIsLoaded;            // Is model ready?


		s_3ds_mesh*				m_pMeshes;              // Mesh structs w/ render and texture handles

        int                     m_nMeshes;              // Mesh count
};

*/


class C3DSModel
{
	public:

		C3DSModel();
		virtual ~C3DSModel();

		bool		Load( char* name );

		void		Render( );

	private:

		void		IntColorChunkProcessor( long len, long index, int matIndex );
		void		FloatColorChunkProcessor( long len, long index, int matIndex );

		void		MainChunkProcessor( long len, long index );
		void		EditChunkProcessor( long len, long index );

		void		MaterialChunkProcessor( long len, long index, int matIndex );
		void		MaterialNameChunkProcessor( long len, long index, int matIndex );
		void		DiffuseColorChunkProcessor( long len, long index, int matIndex );
		void		TextureMapChunkProcessor( long len, long index, int matIndex );
		void		MapNameChunkProcessor( long len, long index, int matIndex );

		void		ObjectChunkProcessor( long len, long index, int objIndex );
		void		TriangularMeshChunkProcessor( long len, long index, int objIndex );
		void		VertexListChunkProcessor( long len, long index, int objIndex );
		void		TexCoordsChunkProcessor( long len, long index, int objIndex );
		void		FacesDescriptionChunkProcessor( long len, long index, int objIndex );
		void		FacesMaterialsListChunkProcessor( long len, long index, int objIndex, int subFacesIndex );

		void		CalculateVertexNormals();


		struct S3DSVertex
		{
			vec3f		pos;
			vec3f		norm;
			vec2f		texCoord;
		};

		struct S3DSMaterial
		{
			S3DSMaterial();
			~S3DSMaterial();

			char			name[80];
			CImage*			pTexture;
			bool			hasTexture;
			unsigned int	color;
			int				texHandle;
		};


		struct S3DSHeaderChunk
		{
			unsigned short	id;
			unsigned int	len;
		};


		struct S3DSMaterialFaces
		{
			S3DSMaterialFaces();
			~S3DSMaterialFaces();

			unsigned short*	pSubFaces;
			unsigned int	nSubFaces;
			int				matIndex;
			int				iboOffset;
		};


		struct S3DSObject
		{
			S3DSObject();
			~S3DSObject();

			char					name[80];
			float*					pVerts;
			float*					pNorms;
			float*					pTexCoords;
			unsigned short*			pFaces;
			int						nFaces;
			unsigned int			m_nMatFaces;
			unsigned int			nVerts;
			unsigned int			nTexCoords;
			bool					isTextured;
			S3DSMaterialFaces*		pMatFaces;
			vec3f					pos;
			vec3f					rot;

			GLuint					vbo;
			GLuint					ibo;

		};


		char*				pModelName;
		char*				pFilePath;
		unsigned int		nObjects;
		unsigned int		nMaterials;
		unsigned int		nVerts;
		unsigned int		nFaces;
		S3DSMaterial*		pMaterials;
		S3DSObject*			pObjects;
		vec3f				pos;
		vec3f				rot;
		float				scale;

		FILE*				bin3DS;
};






#endif
