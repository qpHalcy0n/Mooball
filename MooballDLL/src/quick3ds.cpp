#include "quick3ds.h"
#include "quickindex_t.h"


/*
#pragma pack(push)
#pragma pack(1)

typedef struct
{
	int mat;
	int verts[3];
	int index;
	int smooth;
}triangle_3ds;

#pragma pack(pop)

// callback declaration //
typedef void (*cb)(chunk_3ds*, void*);

//bool read3DSFile(char* fName);
//void free3DSData(chunk_data3ds* dat);
//void calculate3DSNormals(chunk_data3ds* dat);
//void calculate3DSTangentSpace(chunk_data3ds* dat);
//void calculate3DSBoundingBox(chunk_data3ds* dat);
static void readString(char* str);
static void readChunkArray(int length, void(*callback)(chunk_3ds*, void*), void* dat);
static void inspectChunkArray(int length, void(*callback)(chunk_3ds*, void*), void* dat);
static void readMain3DS(chunk_3ds* chunk, chunk_data3ds* out);
static void inspectEdit3DS(chunk_3ds* chunk, chunk_data3ds* out);
static void inspectEditObject(chunk_3ds* chunk, chunk_data3ds* out);
static void readEdit3DS(chunk_3ds* chunk, chunk_data3ds* out);
static void readEditObject(chunk_3ds* chunk, chunk_data3ds* out);
static void readTriangleObject(chunk_3ds* chunk, chunk_mesh3ds* out);
static void readVertexList(chunk_mesh3ds* out);
static void readFaceList(chunk_mesh3ds* out, int length);
static void inspectFaceSubs(chunk_3ds* chunk, chunk_mesh3ds* out);
static void readFaceSubs(chunk_3ds* chunk, chunk_mesh3ds* out);
static void readTexCoords(chunk_mesh3ds* out);
static void readLocalAxis(chunk_mesh3ds* out);
static void readMaterialGroup(chunk_group3ds* group);
static void readMaterial(chunk_3ds* chunk, chunk_material3ds* mat);
static void readTexture(chunk_3ds* chunk, chunk_material3ds* mat);
static void readColor(chunk_3ds* chunk, float* color);
static void readPercentage(chunk_3ds* chunk, float* val);
//static short readChunkID();
//static void skipChunk();

static void removeDegenerates(chunk_mesh3ds* out);
static void sortTriangles(chunk_mesh3ds* out);
static void calculateMeshNormals(chunk_mesh3ds* dat);
static void calculateMeshTangentSpace(chunk_mesh3ds* dat);
static void calculateMeshBoundingBox(chunk_mesh3ds* dat);
static void smoothTangentSpace(chunk_mesh3ds* dat);
static void tangentSpace(float* v1, float* v2, float* v3, float* t1, float* t2, float* t3, float* norm, float* tangentSpace);
*/
/*
    Internal Usage
*/

/*
bool read3DSFile(char* fName, chunk_data3ds* out);
void free3DSData(chunk_data3ds* dat);
void calculate3DSNormals(chunk_data3ds* dat);
void calculate3DSTangentSpace(chunk_data3ds* dat);
void calculate3DSBoundingBox(chunk_data3ds* dat);


static FILE* g_pFile = NULL;
static char g_szString[64];

// static qsort compare func //
static int comparePos(float* a, float* b)
{
	for(int i = 0; i < 3; ++i)
	{
		if(a[i] > b[i])
			return 1;

		if(a[i] < b[i])
			return -1;
	}

	return 0;
}

// static qsort compare func //
static int compareLongs(int* a, int* b)
{
	if(*a > *b)
		return 1;
	if(*a < *b)
		return -1;
	return 0;
}

// static sort by mat func //
static int sortByMaterial(triangle_3ds* a, triangle_3ds* b)
{
	if(a->mat > b->mat)
		return 1;
	if(a->mat < b->mat)
		return -1;

	if(a->smooth > b->smooth)
		return 1;
	if(a->smooth < b->smooth)
		return -1;

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// read3DSFile
// This is the main point of contact for loading of 3DS data via the c3DS class
// returns a pointer to the main 3ds data chunk
bool read3DSFile(const char* fName, chunk_data3ds* out)
{
	chunk_3ds chunk;
	memset(out, 0, sizeof(chunk_data3ds));

	g_pFile = fopen(fName, "rb");

	if(g_pFile)
	{
		// if file exists proceed with reading the chunk list //
		fseek(g_pFile, 0, SEEK_SET);
		fread(&chunk, sizeof(chunk_3ds), 1, g_pFile);
		if(chunk.id == 0x4D4D)
			readChunkArray(chunk.length - 6, (cb)readMain3DS, out);

		fclose(g_pFile);
	}

	else
		return false;

	// this essentially removes both duplicate triangles and those triangles that were generated
	// in error by the app, then sorts them via group/material //
	for(int i = 0; i < out->meshCount; ++i)
	{
		removeDegenerates(&out->meshes[i]);
		sortTriangles(&out->meshes[i]);
	}

	out->vertCount = 0;
	out->triCount = 0;

	for(int i = 0; i < out->meshCount; ++i)
	{
		out->vertCount += out->meshes[i].vertCount;
		out->triCount += out->meshes[i].triCount;
	}


	// calc normals, bb's, and tangent space out of new data object //
	calculate3DSBoundingBox(out);
	calculate3DSNormals(out);
	calculate3DSTangentSpace(out);

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// free3DSData
// frees up any allocated 3ds data, main point of contact for c3DS cleanup
void free3DSData(chunk_data3ds* dat)
{
	if(dat->meshes)
	{
		for(int i = 0; i < dat->meshCount; ++i)
		{
			if(dat->meshes[i].verts)
			{
				free(dat->meshes[i].verts);
				dat->meshes[i].verts = NULL;
			}

			if(dat->meshes[i].tris)
			{
				free(dat->meshes[i].tris);
				dat->meshes[i].tris = NULL;
			}

			if(dat->meshes[i].norms)
			{
				free(dat->meshes[i].norms);
				dat->meshes[i].norms = NULL;
			}

			if(dat->meshes[i].tangentSpace)
			{
				free(dat->meshes[i].tangentSpace);
				dat->meshes[i].tangentSpace = NULL;
			}

			if(dat->meshes[i].texCoords)
			{
				free(dat->meshes[i].texCoords);
				dat->meshes[i].texCoords = NULL;
			}

			if(dat->meshes[i].smooth)
			{
				free(dat->meshes[i].smooth);
				dat->meshes[i].smooth = NULL;
			}

			if(dat->meshes[i].groups)
			{
				for(int j = 0; j < dat->meshes[i].groupCount; ++j)
				{
					free(dat->meshes[i].groups[j].tris);
					dat->meshes[i].groups[j].tris = NULL;
				}

				free(dat->meshes[i].groups);
				dat->meshes[i].groups = NULL;
			}
		}

		free(dat->meshes);
		dat->meshes = NULL;
	}

	free(dat->materials);
	dat->materials = NULL;
	memset(dat, 0, sizeof(chunk_data3ds));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// calculate3DSNormals
// calculates normals per vertex for the data set
void calculate3DSNormals(chunk_data3ds* dat)
{
	for(int i = 0; i < dat->meshCount; ++i)
		calculateMeshNormals(&dat->meshes[i]);
}

////////////////////////////////////////////////////////////////////////////////////////
// calculate3DSTangentSpace
// calculates tangent space vectors for the entire 3ds data set
void calculate3DSTangentSpace(chunk_data3ds* dat)
{
	for(int i = 0; i < dat->meshCount; ++i)
	{
		calculateMeshTangentSpace(&dat->meshes[i]);
		if(dat->meshes[i].tangentSpace)
			smoothTangentSpace(&dat->meshes[i]);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// calculate3DSBoundingBox
// Calculates bounding box for entire 3DS model data set
void calculate3DSBoundingBox(chunk_data3ds* dat)
{
	for(int i = 0; i < dat->meshCount; ++i)
	{
		calculateMeshBoundingBox(&dat->meshes[i]);

		if(i == 0)
		{
			memcpy(dat->max, dat->meshes[i].max, sizeof(float) * 3);
			memcpy(dat->min, dat->meshes[i].min, sizeof(float) * 3);
		}

		// check and swap max values conditionally //
		else
		{
			for(int j = 0; j < 3; ++j)
			{
				if(dat->meshes[i].max[j] > dat->max[j])
					dat->max[j] = dat->meshes[i].max[j];
				if(dat->meshes[i].min[j] < dat->min[j])
					dat->min[j] = dat->meshes[i].min[j];
			}
		}
	}

	// compute model space center //
	dat->center[0] = dat->min[0] + (dat->max[0] - dat->min[0]) * 0.5f;
	dat->center[1] = dat->min[1] + (dat->max[1] - dat->min[1]) * 0.5f;
	dat->center[2] = dat->min[2] + (dat->max[2] - dat->min[2]) * 0.5f;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// readString
// reads a string up to a null terminator
void readString(char* str)
{
	int i = 0;
	do
	{
		fread(&str[i], sizeof(char), 1, g_pFile);
	}while(str[i++] != '\0');
}

////////////////////////////////////////////////////////////////////////////////////////////////
// readChunkArray
// reads the main chunk list for the 3ds data set
void readChunkArray(int length, void(*callback)(chunk_3ds*, void*), void* dat)
{
	chunk_3ds chunk;
	int start, pos;
	pos = 0;

	// eat one chunk at a time calling the appropriate callback to read it //
	do
	{
		start = ftell(g_pFile);
		fread(&chunk, sizeof(chunk_3ds), 1, g_pFile);

		callback(&chunk, dat);

		fseek(g_pFile, start + chunk.length, SEEK_SET);

		// move new pos up
		pos += chunk.length;
	}while(pos < length);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// inspectChunkArray
// Essentially the same as above but is called with a different callback
void inspectChunkArray(int length, void(*callback)(chunk_3ds*, void*), void* dat)
{
	chunk_3ds chunk;
	int start, chunkStart, pos;
	pos = 0;

	chunkStart = ftell(g_pFile);

	// eat a chunk at a time //
	do
	{
		start = ftell(g_pFile);
		fread(&chunk, sizeof(chunk_3ds), 1, g_pFile);

		callback(&chunk, dat);

		fseek(g_pFile, start + chunk.length, SEEK_SET);

		// update file pos ptr //
		pos += chunk.length;
	}while(pos < length);

	fseek(g_pFile, chunkStart, SEEK_SET);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// readMain3DS
// called from read3DSFile, and essentially reads chunk by chunk ensuring the appropriate
// callback funcs that read the chunks are called
void readMain3DS(chunk_3ds* chunk, chunk_data3ds* out)
{
	// If its a valid chunk it will be tagged w/ 0x3D3D as an id
	if(chunk->id == 0x3D3D)
	{
		inspectChunkArray(chunk->length - 6, (cb)inspectEdit3DS, out);

		// allocate for mesh and material list //
		if(out->meshCount)
		{
			out->meshes = (chunk_mesh3ds*)malloc(sizeof(chunk_mesh3ds) * out->meshCount);
			memset(out->meshes, 0, sizeof(chunk_mesh3ds) * out->meshCount);
		}

		if(out->materialCount)
		{
			out->materials = (chunk_material3ds*)malloc(sizeof(chunk_material3ds) * out->materialCount);
			memset(out->materials, 0, sizeof(chunk_material3ds) * out->materialCount);
		}

		out->meshCount = 0;
		out->materialCount = 0;

		// actually read the chunk data //
		readChunkArray(chunk->length - 6, (cb)readEdit3DS, out);

		// set the materials per mesh, per group
		for(int i = 0; i < out->meshCount; ++i)
		{
			for(int j = 0; j < out->meshes[i].groupCount; ++j)
			{
				for(int k = 0; k < out->materialCount; ++k)
				{
					if(!strcmp(out->meshes[i].groups[j].name, out->materials[k].name))
					{
						out->meshes[i].groups[j].mat = k;
						break;
					}
				}
			}
		}
	}

	else
	{
		return;
	}
}

////////////////////////////////////////////////////////////////////////////////////
// inspectEdit3DS
// switches the editing of the chunk over to the appropriate callback
void inspectEdit3DS(chunk_3ds* chunk, chunk_data3ds* out)
{
	if(chunk->id == 0xAFFF)			// flag for material edit
		out->materialCount++;
	else if(chunk->id == 0x4000)	// flag for object edit
	{
		readString(g_szString);
		inspectChunkArray(chunk->length - 6 + strlen(g_szString), (cb)inspectEditObject, out);
	}
	else
	{
		return;
	}
}

/////////////////////////////////////////////////////////////////////////////////
// inspectEditObject
// same as inspectEdit3DS except that it operates on a singular piece of 3DS data
void inspectEditObject(chunk_3ds* chunk, chunk_data3ds* out)
{
	if(chunk->id == 0x4100)		// flag for object edit
		out->meshCount++;

	else
	{
		return;
	}
}

void readFloat( chunk_3ds* chunk, chunk_data3ds* out )
{
	int start, pos;
	pos = 0;

	do
	{
		start = ftell( g_pFile );
		fread( &out->masterScale, sizeof(float), 1, g_pFile );
		fseek( g_pFile, start + chunk->length, SEEK_SET );
		pos += chunk->length;
	} while( pos < chunk->length );
}

//////////////////////////////////////////////////////////////////////////////////////////
// readEdit3DS
// This is in turn called by readMain3DS after the inspect chunk function has been called
void readEdit3DS(chunk_3ds* chunk, chunk_data3ds* out)
{
	int start;

	switch(chunk->id)
	{
		case 0x0100:
			start = ftell( g_pFile );
			fread( &out->masterScale, sizeof(float), 1, g_pFile );
			fseek( g_pFile, start + chunk->length, SEEK_SET );
			break;

		case 0x4000:
			readString(g_szString);
			readChunkArray(chunk->length - 6 + strlen(g_szString), (cb)readEditObject, out);
			break;

		case 0xAFFF:
			readChunkArray(chunk->length - 6, (cb)readMaterial, &out->materials[out->materialCount++]);
			break;

		default:
			break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// readEditObject
// Edits the global string with the one in the chunk being read
void readEditObject(chunk_3ds* chunk, chunk_data3ds* out)
{
	if(chunk->id == 0x4100)				// read triangle object flag
	{
		strcpy(out->meshes[out->meshCount].name, g_szString);
		readChunkArray(chunk->length - 6, (cb)readTriangleObject, &out->meshes[out->meshCount++]);
	}

	else
	{
		return;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// readTriangleObject
// Coordinates the reads of a triangle list chunk with reading vertices, tex coords, normals, etc...
void readTriangleObject(chunk_3ds* chunk, chunk_mesh3ds* out)
{
	switch(chunk->id)
	{
		case 0x4110:		// vertex list
			readVertexList(out);
			break;

		case 0x4120:		// face list
			readFaceList(out, chunk->length - 6);
			break;

		case 0x4140:		// tex coord list
			readTexCoords(out);
			break;

		case 0x4160:		// local axis
			readLocalAxis(out);
			break;

		case 0x4170:
			break;
		default:
			break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////
// readVertexList
// Reads in raw vertex data chunk
void readVertexList(chunk_mesh3ds* out)
{
	unsigned short nVerts;
	fread(&nVerts, sizeof(unsigned short), 1, g_pFile);

	out->vertCount = nVerts;
	out->verts = (float(*)[3])malloc(sizeof(float) * 3 * out->vertCount);

	// read raw vertex data in //
	fread(out->verts, sizeof(float), 3 * out->vertCount, g_pFile);
}

/////////////////////////////////////////////////////////////////////////////////////////
// readFaceList
// Reads faces in by groups in the mesh
void readFaceList(chunk_mesh3ds* out, int length)
{
	int pos = 6;
	unsigned short nTris;

	fread(&nTris, sizeof(unsigned short), 1, g_pFile);
	pos += sizeof(unsigned short);

	out->triCount = nTris;
	out->tris = (int(*)[3])malloc(sizeof(int) * 3 * out->triCount);

	unsigned short verts[4];

	// construct a temp face from 4 verts //
	for(int i = 0; i < nTris; ++i)
	{
		// read the verts in sequence and construct the tri list //
		fread(verts, sizeof(short), 4, g_pFile);
		for(int j = 0; j < 3; ++j)
		{
			out->tris[i][j] = (int)verts[j];
		}
	}

	// update fp pos //
	pos += sizeof(unsigned short) * 4 * nTris;

	// if we've got more data then there are sub faces in groups //
	if(pos < length)
	{
		inspectChunkArray(pos - 6, (cb)inspectFaceSubs, out);

		if(out->groupCount)
		{
			out->groups = (chunk_group3ds*)malloc(sizeof(chunk_group3ds) * out->groupCount);
			memset(out->groups, 0, sizeof(chunk_group3ds) * out->groupCount);
			if(out->groups)
			{
				out->groupCount = 0;
				readChunkArray(pos - 6, (cb)readFaceSubs, out);
			}
		}
	}
}

//////////////////////////////////////////////////////////////
// inspectFaceSubs
// Inspects possibly degenerate faces
void inspectFaceSubs(chunk_3ds* chunk, chunk_mesh3ds* out)
{
	if(chunk->id == 0x4130)
		out->groupCount++;

	else
	{
		return;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////
// readFaceSubs
void readFaceSubs(chunk_3ds* chunk, chunk_mesh3ds* out)
{
	switch(chunk->id)
	{
		case 0x4130:		// material group
			readMaterialGroup(&out->groups[out->groupCount++]);
			break;

		case 0x4150:		// smoothing group
			out->smooth = (int*)malloc(sizeof(int) * out->triCount);
			if(out->smooth)
				fread(out->smooth, sizeof(int), out->triCount, g_pFile);
			break;

		default:
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////
// readTexCoords
void readTexCoords(chunk_mesh3ds* out)
{
	// alloc and read raw texture data //
	unsigned short nTexCoords;
	fread(&nTexCoords, sizeof(unsigned short), 1, g_pFile);
	out->texCoordCount = nTexCoords;

	out->texCoords = (float(*)[2])malloc(sizeof(float) * 2 * out->texCoordCount);
	fread(out->texCoords, sizeof(float), 2 * out->texCoordCount, g_pFile);
}

///////////////////////////////////////////////////////////////////////////////////
// readLocalAxis
void readLocalAxis(chunk_mesh3ds* out)
{
	fread(out->axis, sizeof(float), 9, g_pFile);
	fread(out->position, sizeof(float), 3, g_pFile);
}

//////////////////////////////////////////////////////////////////////////////////
// readMaterialGroup
void readMaterialGroup(chunk_group3ds* group)
{
	unsigned short nFaces;
	unsigned short face;

	readString(group->name);
	fread(&nFaces, sizeof(unsigned short), 1, g_pFile);

	group->tris = (int*)malloc(nFaces * sizeof(int));

	// group together tris that share material properties //
	if(group->tris)
	{
		memset(group->tris, 0, nFaces * sizeof(int));

		group->size = nFaces;
		group->mat = 0;

		// read in face indices that share this material //
		for(int i = 0; i < nFaces; ++i)
		{
			fread(&face, sizeof(unsigned short), 1, g_pFile);
			group->tris[i] = face;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// readMaterial
void readMaterial(chunk_3ds* chunk, chunk_material3ds* mat)
{
	switch(chunk->id)
	{
		case 0xA000:			// name
			readString(mat->name);
			break;

		case 0xA010:			// ambient
			readChunkArray(chunk->length - 6, (cb)readColor, mat->ambient);
			mat->ambient[3] = 1.0f;
			break;

		case 0xA020:			// diffuse
			readChunkArray(chunk->length - 6, (cb)readColor, mat->diffuse);
			mat->diffuse[3] = 1.0f;
			break;

		case 0xA030:			// specular
			readChunkArray(chunk->length - 6, (cb)readColor, mat->specular);
			mat->specular[3] = 1.0f;
			break;

		case 0xA040:			// shininess
			readChunkArray(chunk->length - 6, (cb)readPercentage, &mat->shininess);
			mat->shininess *= 140.0f;
			break;

		case 0xA080:			// emissive
			readChunkArray(chunk->length - 6, (cb)readColor, mat->emissive);
			mat->emissive[3] = 1.0f;
			break;

		case 0xA200:			// texture index
			readChunkArray(chunk->length - 6, (cb)readTexture, mat);
			break;

		default:
			return;
	}
}

/////////////////////////////////////////////////////////////////////////////////
// readTexture
void readTexture(chunk_3ds* chunk, chunk_material3ds* mat)
{
	if(chunk->id == 0xA300)
		readString(mat->texture);
	else
	{
		return;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// readColor
void readColor(chunk_3ds* chunk, float* val)
{
	unsigned char rgb[3];

	if(chunk->id == 0x0010)     // COLOR_F
		fread(val, sizeof(float), 3, g_pFile);

	// color is stored as a normalized char //
	else if(chunk->id == 0x0011)
	{
		fread(rgb, sizeof(char), 3, g_pFile);
		val[0] = float(rgb[0]) / 256.0f;
		val[1] = float(rgb[1]) / 256.0f;
		val[2] = float(rgb[2]) / 256.0f;
	}

	else
	{
		return;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// readPercentage
void readPercentage(chunk_3ds* chunk, float* val)
{
	short sPer;

	// short percentage
	if(chunk->id == 0x0030)
	{
		fread(&sPer, sizeof(short), 1, g_pFile);
		*val = (float)sPer;
	}

	// float percentage
	else if(chunk->id == 0x0031)
		fread(val, sizeof(float), 1, g_pFile);

	else
	{
		return;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// removeDegenerates
void removeDegenerates(chunk_mesh3ds* mesh)
{
	int i, j, k, l;
	int* found;

	for(i = 0; i < mesh->triCount; ++i)
	{
		if((mesh->tris[i][0] >= mesh->vertCount) ||
		   (mesh->tris[i][1] >= mesh->vertCount) ||
		   (mesh->tris[i][2] >= mesh->vertCount))
		{
			i = i;
		}
	}

	for(i = 0; i < mesh->groupCount; ++i)
	{
		// sort triangle groups by size
		qsort(mesh->groups[i].tris, mesh->groups[i].size, sizeof(int), (sortFunc_t)compareLongs);
	}

	for(i = 0; i < mesh->triCount; ++i)
	{
		// this monolithic if statement basically checks for dupe triangles
		if(((mesh->tris[i][0] == mesh->tris[i][1]) ||
            (mesh->tris[i][0] == mesh->tris[i][2]) ||
            (mesh->tris[i][1] == mesh->tris[i][2])) ||

          ((mesh->verts[mesh->tris[i][0]][0] == mesh->verts[mesh->tris[i][1]][0]) &&
           (mesh->verts[mesh->tris[i][0]][1] == mesh->verts[mesh->tris[i][1]][1]) &&
           (mesh->verts[mesh->tris[i][0]][2] == mesh->verts[mesh->tris[i][1]][2])) ||

          ((mesh->verts[mesh->tris[i][0]][0] == mesh->verts[mesh->tris[i][2]][0]) &&
           (mesh->verts[mesh->tris[i][0]][1] == mesh->verts[mesh->tris[i][2]][1]) &&
           (mesh->verts[mesh->tris[i][0]][2] == mesh->verts[mesh->tris[i][2]][2])) ||

          ((mesh->verts[mesh->tris[i][1]][0] == mesh->verts[mesh->tris[i][2]][0]) &&
           (mesh->verts[mesh->tris[i][1]][1] == mesh->verts[mesh->tris[i][2]][1]) &&
           (mesh->verts[mesh->tris[i][1]][2] == mesh->verts[mesh->tris[i][2]][2])))
		{
			// if there were any found, scooch over and kick it out
			if(i != (mesh->triCount - 1))
				memmove(&mesh->tris[i], &mesh->tris[i + 1], sizeof(int) * 3 * (mesh->triCount - i - 1));

			for(j = 0; j < mesh->groupCount; ++j)
			{
				found = (int*)bsearch(&i, mesh->groups[j].tris, mesh->groups[j].size, sizeof(int), (sortFunc_t)compareLongs);

				if(found != NULL)
				{
					k = ((int*)found - (int*)mesh->groups[j].tris) / sizeof(int);

					if(k < mesh->groups[j].size - 1)
						memmove(&mesh->groups[j].tris[k], &mesh->groups[j].tris[k + 1], sizeof(int) * (mesh->groups[j].size - k -1));

					mesh->groups[j].size--;

					for(l = k; l < mesh->groups[j].size; ++l)
						mesh->groups[j].tris[l]--;
				}
			}

			mesh->triCount--;
			--i;
        }
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// sortTriangles
void sortTriangles(chunk_mesh3ds* dat)
{
	int result, next, nextNext, i, j;
	result = next = nextNext = 0;
	triangle_3ds* tris;
	triangle_3ds* temp;
	int* oldSmooth = NULL;
	int groupCount;

	if(dat->groupCount == 0)
	{
		dat->groups = (chunk_group3ds*)malloc(sizeof(chunk_group3ds));
		if(dat->groups)
		{
			dat->groupCount = 1;
			memset(&dat->groups[0], 0, sizeof(chunk_group3ds));
			dat->groups[0].mat = -1;
			dat->groups[0].start = 0;
			dat->groups[0].size = dat->triCount;
			dat->groups[0].tris = (int*)malloc(sizeof(int) * dat->triCount);

			for(i = 0; i < dat->groups[0].size; ++i)
			{
				dat->groups[0].tris[i] = i;
			}
		}

		if(dat->smooth == NULL)
			return;
	}

	tris = (triangle_3ds*)malloc(dat->triCount * sizeof(triangle_3ds));
	temp = (triangle_3ds*)malloc(dat->triCount * sizeof(triangle_3ds));

	for(i = 0; i < dat->triCount; ++i)
	{
		memcpy(tris[i].verts, dat->tris[i], sizeof(int) * 3);
		tris[i].mat = -1;
		tris[i].index = i;

		if(dat->smooth)
			tris[i].smooth = dat->smooth[i];
		else
			tris[i].smooth = 0;
	}

	for(i = 0; i < dat->groupCount; ++i)
	{
		for(j = 0; j < dat->groups[i].size; ++j)
		{
			tris[dat->groups[i].tris[j]].mat = dat->groups[i].mat;
		}
	}


	// sort tris by material //
	i = 0;
	while(i < dat->triCount)
	{
		result = 0;
		next = i + 1;
		while((next < dat->triCount) && (result == 0))
		{
			result = sortByMaterial(&tris[i], &tris[next]);
			++next;
		}

		if(next == dat->triCount)
			break;
		--next;

		if(result > 0)
		{
			result = 0;
			nextNext = next + 1;
			while((nextNext < dat->triCount) && (result == 0))
			{
				result = sortByMaterial(&tris[next], &tris[nextNext]);
				++nextNext;
			}

			if(result != 0)
				--nextNext;

			// copy less than portion off to a temp buffer //
			memcpy(temp, &tris[next], (nextNext - next) * sizeof(triangle_3ds));

			// slide the greater than portion ahead //
			memmove(&tris[(nextNext - next) + i], &tris[i], (next - i) * sizeof(triangle_3ds));

			// copy the less than portion back in from temp //
			memcpy(&tris[i], temp, (nextNext - next) * sizeof(triangle_3ds));

			i = 0;
		}

		else if(result < 0)
			i = next;
		else
			break;
	}

	groupCount = 1;
	dat->groups[0].mat = tris[0].mat;
	dat->groups[0].start = 0;
	dat->groups[0].size = 0;
	for(i = 0; i < dat->triCount; ++i)
	{
		memcpy(dat->tris[i], tris[i].verts, sizeof(int) * 3);
		if(dat->smooth)
			dat->smooth[i] = tris[i].smooth;

		if(dat->groups[groupCount - 1].mat != tris[i].mat)
		{
			dat->groups[groupCount].mat = tris[i].mat;
			dat->groups[groupCount].start = i;
			dat->groups[groupCount].size = 0;
			++groupCount;
		}

		dat->groups[groupCount - 1].size++;
	}

	if(tris)
	{
		free(tris);
		tris = NULL;
	}

	if(temp)
	{
		free(temp);
		temp = NULL;
	}
}

////////////////////////////////////////////////////////////////////////////////////
// calculateMeshTangentSpace
void calculateMeshTangentSpace(chunk_mesh3ds* mesh)
{
	if(mesh->norms == NULL)
		calculateMeshNormals(mesh);

	if((mesh->norms == NULL) || (mesh->texCoords == NULL))
		return;

	if(mesh->tangentSpace != NULL)
	{
		free(mesh->tangentSpace);
		mesh->tangentSpace = NULL;
	}


	mesh->tangentSpace = (float(*)[9])malloc(sizeof(float) * 9 * mesh->triCount * 3);


	memset(mesh->tangentSpace, 0, sizeof(float) * 9 * mesh->triCount * 3);
	for(int i = 0; i < mesh->triCount; ++i)
	{
		for(int j = 0; j < 3; ++j)
		{
			tangentSpace(mesh->verts[mesh->tris[i][0]], mesh->verts[mesh->tris[i][1]], mesh->verts[mesh->tris[i][2]],
						 mesh->texCoords[mesh->tris[i][0]], mesh->texCoords[mesh->tris[i][1]], mesh->texCoords[mesh->tris[i][2]],
						 mesh->norms[i * 3 + j], mesh->tangentSpace[i * 3 + j]);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// calculateMeshBoundingBox
void calculateMeshBoundingBox(chunk_mesh3ds* mesh)
{
	for(int i = 0; i < mesh->vertCount; ++i)
	{
		if(i == 0)
		{
			memcpy(mesh->max, mesh->verts[i], sizeof(float) * 3);
			memcpy(mesh->min, mesh->verts[i], sizeof(float) * 3);
		}

		else
		{
			for(int j = 0; j < 3; ++j)
			{
				if(mesh->verts[i][j] > mesh->max[j])
					mesh->max[j] = mesh->verts[i][j];
				if(mesh->verts[i][j] < mesh->min[j])
					mesh->min[j] = mesh->verts[i][j];
			}
		}
	}

	mesh->center[0] = mesh->min[0] + (mesh->max[0] - mesh->min[0]) * 0.5f;
	mesh->center[1] = mesh->min[1] + (mesh->max[1] - mesh->min[1]) * 0.5f;
	mesh->center[2] = mesh->min[2] + (mesh->max[2] - mesh->min[2]) * 0.5f;
}



//////////////////////////////////////////////////////////////////////////////////////////////
// smoothTangentSpace
void smoothTangentSpace(chunk_mesh3ds* mesh)
{
	int i, j, k;
	int tri, vert;
	int tanCount;
	int (*tanRefs)[50];
	float tanSpace[9];
	float tempBinorm[3];
	float dotTangent;
	float dotBinormal;
	float dotNormal;
	float (*newTanSpace)[9];

	newTanSpace = (float(*)[9])malloc(sizeof(float) * 9 * mesh->triCount * 3);
	tanRefs = (int(*)[50])malloc(sizeof(int) * 50 * mesh->vertCount);


	memset(tanRefs, 0, sizeof(int) * 50 * mesh->vertCount);

	for(i = 0; i < mesh->triCount; ++i)
	{
		for(j = 0; j < 3; ++j)
		{
			if(tanRefs[mesh->tris[i][j]][0] < 48)
			{
				tanRefs[mesh->tris[i][j]][0]++;
				tanRefs[mesh->tris[i][j]][tanRefs[mesh->tris[i][j]][0]] = i * 3 + j;
			}
		}
	}

	for(i = 0; i < mesh->triCount; ++i)
	{
		for(j = 0; j < 3; ++j)
		{
			memset(tanSpace, 0, sizeof(float) * 9);
			tanCount = 0;

			for(k = 0; k <= tanRefs[mesh->tris[i][j]][0]; ++k)
			{
				tri = tanRefs[mesh->tris[i][j]][k] / 3;
				vert = tanRefs[mesh->tris[i][j]][k] % 3;

				dotTangent = vec3_dot(&mesh->tangentSpace[i * 3 + j][0], &mesh->tangentSpace[tri * 3 + vert][0]);
				dotBinormal = vec3_dot(&mesh->tangentSpace[i * 3 + j][3], &mesh->tangentSpace[tri * 3 + vert][3]);
				dotNormal = vec3_dot(&mesh->tangentSpace[i * 3 + j][6], &mesh->tangentSpace[tri * 3 + vert][6]);

				if((dotTangent > 0.85) && (dotBinormal > 0.85) && (dotNormal > 0.85))
				{
					vec3_add_replace(&tanSpace[0], &mesh->tangentSpace[tri * 3 + vert][0]);
					vec3_add_replace(&tanSpace[3], &mesh->tangentSpace[tri * 3 + vert][3]);
					vec3_add_replace(&tanSpace[6], &mesh->tangentSpace[tri * 3 + vert][6]);
					++tanCount;
				}
			}

			float iScale = 1.0f / (float)tanCount;
			tanSpace[0] *= iScale;
			tanSpace[1] *= iScale;
			tanSpace[2] *= iScale;
			tanSpace[3] *= iScale;
			tanSpace[4] *= iScale;
			tanSpace[5] *= iScale;
			tanSpace[6] *= iScale;
			tanSpace[7] *= iScale;
			tanSpace[8] *= iScale;
			vec3_normalize(&tanSpace[0]);
			vec3_normalize(&tanSpace[3]);
			vec3_normalize(&tanSpace[6]);

			vec3_cross(&tanSpace[0], &tanSpace[6], tempBinorm);
			vec3_normalize(tempBinorm);
			dotBinormal = vec3_dot(&tanSpace[3], tempBinorm);
			if(dotBinormal < 0.0f)
			{
				tempBinorm[0] = -tempBinorm[0];
				tempBinorm[1] = -tempBinorm[1];
				tempBinorm[2] = -tempBinorm[2];
			}

			memcpy(newTanSpace[i * 3 + j], tanSpace, sizeof(float) * 9);
			memcpy(&newTanSpace[i * 3 + j][3], tempBinorm, sizeof(float) * 3);
		}
	}

	free(mesh->tangentSpace);
	mesh->tangentSpace = newTanSpace;

	if(tanRefs)
	{
		free(tanRefs);
		tanRefs = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// tangentSpace
void tangentSpace(float* vert1, float* vert2, float* vert3, float* t1, float* t2, float* t3, float* norm, float* tangentSpace)
{
	float a, b;
	float u1, u2;
	float v1, v2;
	float edge1[3];
	float edge2[3];
	float dot;
	float dvTmp[3];
	float duTmp[3];

	if((t1[0] == t2[0]) ||
	   (t1[1] == t2[1]) ||
	   (t1[0] == t3[0]) ||
	   (t1[1] == t3[1]))
	{
		tangentSpace[0] = 1.0f;
		tangentSpace[1] = 0.0f;
		tangentSpace[2] = 0.0f;

		tangentSpace[3] = 0.0f;
		tangentSpace[4] = 1.0f;
		tangentSpace[5] = 0.0f;

		tangentSpace[6] = 0.0f;
		tangentSpace[7] = 0.0f;
		tangentSpace[8] = 1.0f;

		return;
	}

	edge1[0] = vert2[0] - vert1[0];
	edge1[1] = vert2[1] - vert1[1];
	edge1[2] = vert2[2] - vert1[2];
	edge2[0] = vert3[0] - vert1[0];
	edge2[1] = vert3[1] - vert1[1];
	edge2[2] = vert3[2] - vert1[2];

	u1 = t2[0] - t1[0];
    u2 = t3[0] - t1[0];
    v1 = t2[1] - t1[1];
    v2 = t3[1] - t1[1];

    a = (u1 - v1 * u2 / v2);
    if(a != 0.0f)
		a = 1.0f / a;

	b = (u2 - v2 * u1 / v1);
	if(b != 0.0f)
		b = 1.0f / b;

	duTmp[0] = a * edge1[0] + b * edge2[0];
	duTmp[1] = a * edge1[1] + b * edge2[1];
	duTmp[2] = a * edge1[2] + b * edge2[2];
	vec3_normalize(duTmp);

	a = (v1 - u1 * v2 / u2);
	if(a != 0.0f)
		a = 1.0f / a;

	b = (v2 - u2 * v1 / u1);
	if(b != 0.0f)
		b = 1.0f / b;

	dvTmp[0] = a * edge1[0] + b * edge2[0];
	dvTmp[1] = a * edge1[1] + b * edge2[1];
	dvTmp[2] = a * edge1[2] + b * edge2[2];
	vec3_normalize(dvTmp);

	tangentSpace[6] = norm[0];
	tangentSpace[7] = norm[1];
	tangentSpace[8] = norm[2];
	dot = vec3_dot(duTmp, &tangentSpace[6]);

	tangentSpace[0] = duTmp[0] - (dot * tangentSpace[6]);
	tangentSpace[1] = duTmp[1] - (dot * tangentSpace[7]);
	tangentSpace[2] = duTmp[2] - (dot * tangentSpace[8]);
	vec3_normalize(tangentSpace);

	dot = vec3_dot(dvTmp, &tangentSpace[6]);

	tangentSpace[3] = dvTmp[0] - (dot * tangentSpace[6]);
	tangentSpace[4] = dvTmp[1] - (dot * tangentSpace[7]);
	tangentSpace[5] = dvTmp[2] - (dot * tangentSpace[8]);
    vec3_normalize(&tangentSpace[3]);
}

//////////////////////////////////////////////////////////
// calculateMeshNormals
// Calculate face normals from surface tangent space
void calculateMeshNormals(chunk_mesh3ds* mesh)
{
	int i, j, k;
	int normCount;
	int (*triRefs)[50];
	float (*faceNorms)[3];
	float v1[3];
	float v2[3];

	mesh->norms = (float(*)[3])malloc(sizeof(float) * 3 * mesh->triCount * 3);
	memset(mesh->norms, 0, sizeof(float) * 3 * mesh->triCount * 3);

//	if(mesh->smooth == NULL)
//	{
		for(i = 0; i < mesh->triCount; ++i)
		{
			vec3_sub(mesh->verts[mesh->tris[i][1]], mesh->verts[mesh->tris[i][0]], v1);
			vec3_sub(mesh->verts[mesh->tris[i][2]], mesh->verts[mesh->tris[i][0]], v2);
			vec3_cross(v1, v2, mesh->norms[i * 3]);
			vec3_normalize(mesh->norms[i * 3]);

			memcpy(mesh->norms[i * 3 + 1], mesh->norms[i * 3], sizeof(float) * 3);
			memcpy(mesh->norms[i * 3 + 2], mesh->norms[i * 3], sizeof(float) * 3);
		}

		return;
//	}

/*
	index_t vertIndex;
	int vert = 0;
	int result = 0;

	indexArray(&vertIndex, (char*)mesh->verts, sizeof(float) * 3, mesh->vertCount, (sortFunc_t)comparePos);

	triRefs = (int(*)[50])malloc(sizeof(int) * 50 * vertIndex.count);

	memset(triRefs, 0, sizeof(unsigned int) * 50 * vertIndex.count);

	for(i = 0; i < mesh->triCount; ++i)
	{
		for(j = 0; j < 3; ++j)
		{
			vert = indexFind(&vertIndex, mesh->verts[mesh->tris[i][j]], &result);
			if(triRefs[vert][0] < 48)
			{
				triRefs[vert][0]++;
				triRefs[vert][triRefs[vert][0]] = i;
			}
		}
	}

	faceNorms = (float(*)[3])malloc(sizeof(float) * 3 * mesh->triCount);


	if(triRefs != NULL)
	{
		memset(faceNorms, 0, sizeof(float) * 3 * mesh->triCount);

		for(i = 0; i < mesh->triCount; ++i)
		{
			vec3_sub(mesh->verts[mesh->tris[i][1]], mesh->verts[mesh->tris[i][0]], v1);
			vec3_sub(mesh->verts[mesh->tris[i][2]], mesh->verts[mesh->tris[i][0]], v2);
			vec3_cross(v1, v2, faceNorms[i]);
			vec3_normalize(faceNorms[i]);
		}

		for(i = 0; i < mesh->triCount; ++i)
		{
			for(j = 0; j < 3; ++j)
			{
				vert = indexFind(&vertIndex, mesh->verts[mesh->tris[i][j]], &result);

				normCount = 0;
				for(k = 1; k <= triRefs[vert][0]; ++k)
				{
					if(mesh->smooth[i] == mesh->smooth[triRefs[vert][k]])
					{
						vec3_add_replace(mesh->norms[i * 3 + j], faceNorms[triRefs[vert][k]]);
						++normCount;
					}
				}

				float iScale = 1.0f / (float)normCount;

				vec3_scale(mesh->norms[i * 3 + j], iScale);
				vec3_normalize(mesh->norms[i * 3 + j]);
			}
		}
	}

	indexFree(&vertIndex);

	if(triRefs)
	{
		free(triRefs);
		triRefs = NULL;
	}

	if(faceNorms)
	{
		free(faceNorms);
		faceNorms = NULL;
	}

}

*/
/*
    Construct vertex normals from 3DS face normals
*/


/*
static void make_3ds_vertex_normals(chunk_data3ds& dat)
{
    int triCount = 0;
    int vertCount = 0;

    for(int i = 0; i < dat.meshCount; ++i)
    {
        triCount = dat.meshes[i].triCount;
        vertCount = dat.meshes[i].vertCount;

        if(vertCount > 0 && triCount > 0)
        {
            // Dupe and strip the tri and vert data from this mesh //
            unsigned int* pl = new unsigned int[triCount * 3];
            vec3* vl = new vec3[vertCount];

            memcpy(pl, dat.meshes[i].tris, sizeof(int) * triCount * 3);
            memcpy(vl, dat.meshes[i].verts, sizeof(float) * 3 * vertCount);

            // Make vertex normals from tri list //
            create_vertex_normals(vl, vertCount, pl, triCount, (float*)&dat.meshes[i].norms[0]);

            delete[] pl; pl = NULL;
            delete[] vl; vl = NULL;
        }
    }
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CModel3DS::CModel3DS()
{
    m_bIsLoaded = false;
    m_pMeshes   = NULL;
}

CModel3DS::~CModel3DS()
{
    if(m_bIsLoaded)
    {
        free3DSData(&m_modelData);

		for(int i = 0; i < m_nMeshes; ++i)
		{
			glDeleteBuffers(1, &m_pMeshes[i].m_vboID);
			glDeleteBuffers(1, &m_pMeshes[i].m_iboID);

			//TODO: Delete Texture here
		}

		delete[] m_pMeshes; m_pMeshes = NULL;
    }

    m_bIsLoaded = false;
}


bool CModel3DS::Load(const char* fName)
{
    // Open and validate file //
    if(!read3DSFile(fName, &m_modelData))
        return false;

    // Not all meshes are renderable. So sanity check:
    // Find those w/ no triangles/vertices and store the result in m_nMeshes locally
    m_nMeshes = m_modelData.meshCount;
    int i = 0;
    int j = 0;
    for(i = 0; i < m_modelData.meshCount; ++i)
    {
        if(m_modelData.meshes[i].vertCount <= 0 || m_modelData.meshes[i].triCount <= 0)
            --m_nMeshes;
    }

    // Make per-vertex normals //
    make_3ds_vertex_normals(m_modelData);

    // Make new mesh list for renderable meshes and generate the GL buffers //
	m_pMeshes = new s_3ds_mesh[m_nMeshes];
	for(i = 0; i < m_nMeshes; ++i)
	{
		glGenBuffers(1, &m_pMeshes[i].m_vboID);
		glGenBuffers(1, &m_pMeshes[i].m_iboID);
	}

    // Populate data from renderable meshes //
    // "idx" holds an index value of VALID meshes, otherwise our list would be gappy //
	int idx = 0;
	int curMeshIdx = 0;
    for(i = 0; i < m_modelData.meshCount; ++i)
    {    
		float r, g, b;
		unsigned int emissive = 0xFFFFFFFF;
    
		// Loop through the materials and fish out textures and surface properties //
        chunk_mesh3ds curMesh = m_modelData.meshes[i];
        if( curMesh.vertCount <= 0 || curMesh.triCount <= 0 )
			continue;
        
        for(int j = 0; j < curMesh.groupCount; ++j)
        {
        	chunk_group3ds curGroup = curMesh.groups[j];
        	chunk_material3ds curMat = m_modelData.materials[curGroup.mat];

			//TODO: This is a complete hack, there may be more than one material and texture per mesh, we must accomidate for this //
        	if(strlen(curMat.texture) >= 12)
        	{
				std::string texName = "Textures/";
				texName.append(curMat.texture);
				m_pMeshes[curMeshIdx].m_textureHandle = g_pTexInterface->AddTexture(texName.c_str(), SAMPLE_TRILINEAR_ANISO | SAMPLE_CLAMP);
        	}
        	
        	if( curMat.diffuse )
        	{
        		r = curMat.diffuse[0];
        		g = curMat.diffuse[1];
        		b = curMat.diffuse[2];
   
				emissive = 0xFFFFFFFF;     		
 //       		emissive |= ( ( (unsigned char)r * 255 ) << 24 ) | ( ( (unsigned char)g * 255 ) << 16 ) | ( ( (unsigned char)b * 255 ) );
        	}

			emissive = 0xFFFFFFFF;
        }


        int nIndices = curMesh.triCount * 3;
        int nVertices = curMesh.vertCount;

        // Populate index list //
        unsigned int* mesh_indices = new unsigned int[nIndices];
        memcpy(mesh_indices, curMesh.tris, nIndices * sizeof(unsigned int));

        // Clear out vertex buffer memory //
        s_3ds_vertex_format* mesh_verts = (s_3ds_vertex_format*)calloc(nVertices, sizeof(s_3ds_vertex_format));

        // Populate vertex data //
        for(j = 0; j < nVertices; ++j)
        {
            mesh_verts[j].wPos[0] = curMesh.verts[j][0];
            mesh_verts[j].wPos[1] = curMesh.verts[j][1];
            mesh_verts[j].wPos[2] = curMesh.verts[j][2];

            vec3 norm = {curMesh.norms[j][0], curMesh.norms[j][1], curMesh.norms[j][2]};
            vec3_normalize(norm);
            mesh_verts[j].wNorm[0] = norm[0];
            mesh_verts[j].wNorm[1] = norm[1];
            mesh_verts[j].wNorm[2] = norm[2];

            if(curMesh.tangentSpace)
            {
                mesh_verts[j].wTan[0] = curMesh.tangentSpace[j][3];
                mesh_verts[j].wTan[1] = curMesh.tangentSpace[j][4];
                mesh_verts[j].wTan[2] = curMesh.tangentSpace[j][5];
            }

            if(curMesh.texCoordCount > 0)
            {
                mesh_verts[j].tex[0] = curMesh.texCoords[j][0];
                mesh_verts[j].tex[1] = curMesh.texCoords[j][1];
            }

            // TODO: Add material properties to 3DS //
            mesh_verts[j].emissive = emissive;
        }

        // Upload vertex buffer for this mesh //
        glBindBuffer(GL_ARRAY_BUFFER_ARB, m_pMeshes[curMeshIdx].m_vboID);
        glBufferData(GL_ARRAY_BUFFER_ARB, sizeof(s_3ds_vertex_format) * nVertices, mesh_verts, GL_STATIC_DRAW_ARB);
        glBindBuffer(GL_ARRAY_BUFFER_ARB, 0);

        // Upload the index buffer for this mesh //
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, m_pMeshes[curMeshIdx].m_iboID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER_ARB, sizeof(unsigned int) * nIndices, mesh_indices, GL_STATIC_DRAW_ARB);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

        free(mesh_verts);  mesh_verts = NULL;
        delete[] mesh_indices; mesh_indices = NULL;
        ++idx;
        ++curMeshIdx;
    }

    m_bIsLoaded = true;
    m_fileName = fName;
}


void CModel3DS::TestRender()
{
    char* base = 0;
	int idx = 0;

    for(int i = 0; i < m_modelData.meshCount; ++i)
    {
        if(m_modelData.meshes[i].triCount <= 0 || m_modelData.meshes[i].vertCount <= 0)
            continue;

		if(IS_HANDLE_VALID(m_pMeshes[idx].m_textureHandle))
			g_pTexInterface->BindTexture(m_pMeshes[idx].m_textureHandle);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, m_pMeshes[idx].m_iboID);
        glBindBuffer(GL_ARRAY_BUFFER_ARB, m_pMeshes[idx].m_vboID);

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glVertexPointer(3, GL_FLOAT, sizeof(s_3ds_vertex_format), base);            // Vertex
		glNormalPointer(GL_FLOAT, sizeof(s_3ds_vertex_format), base + 12);          // Normal
		glTexCoordPointer(3, GL_FLOAT, sizeof(s_3ds_vertex_format), base + 24);     // Tangent      (texcoord0)
		glTexCoordPointer(2, GL_FLOAT, sizeof(s_3ds_vertex_format), base + 36);     // Texcoord     (texcoord1)
		glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(s_3ds_vertex_format), base + 44);// Color

		glDrawElements(GL_TRIANGLES, m_modelData.meshes[i].triCount * 3, GL_UNSIGNED_INT, 0);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER_ARB, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

		g_pTexInterface->EvictTextures();

		++idx;
    }
}



*/



#define MAIN3DS				0x4D4D
 #define MAIN_VERS			0x0002
 #define EDIT3DS			0x3D3D
  #define MESH_VERS			0x3D3E
  #define OBJECT			0x4000
   #define TRIG_MESH		0x4100
    #define VERT_LIST		0x4110
    #define FACE_DESC		0x4120
     #define FACE_MAT		0x4130
    #define TEX_VERTS		0x4140
     #define SMOOTH_GROUP	0x4150
    #define LOCAL_COORDS	0x4160
  #define MATERIAL			0xAFFF
   #define MAT_NAME			0xA000
   #define MAT_AMBIENT		0xA010
   #define MAT_DIFFUSE		0xA020
   #define MAT_SPECULAR		0xA030
   #define SHINY_PERC		0xA040
   #define SHINY_STR_PERC	0xA041
   #define TRANS_PERC		0xA050
   #define TRANS_FOFF_PERC	0xA052
   #define REF_BLUR_PERC	0xA053
   #define RENDER_TYPE		0xA100
   #define SELF_ILLUM		0xA084
   #define MAT_SELF_ILPCT	0xA08A
   #define WIRE_THICKNESS	0xA087
   #define MAT_TEXMAP		0xA200
    #define MAT_MAPNAME		0xA300
  #define ONE_UNIT			0x0100
 #define KEYF3DS			0xB000
  #define FRAMES			0xB008
  #define MESH_INFO			0xB002
   #define HIER_POS			0xB030
   #define HIER_FATHER		0xB010
   #define PIVOT_PT			0xB013
   #define TRACK00			0xB020
   #define TRACK01			0xB021
   #define TRACK02			0xB022
#define	COLOR_RGB			0x0010
#define COLOR_TRU			0x0011
#define COLOR_TRUG			0x0012
#define COLOR_RGBG			0x0013
#define PERC_INT			0x0030
#define PERC_FLOAT			0x0031




//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
//				S3DSMATERIALFACES CONSTRUCTOR DESTRUCTOR		  //
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
C3DSModel::S3DSMaterialFaces::S3DSMaterialFaces()
{
	pSubFaces = NULL;
	iboOffset = 0;
}

C3DSModel::S3DSMaterialFaces::~S3DSMaterialFaces()
{
	if( pSubFaces ) delete[] pSubFaces;
	pSubFaces = NULL;
	iboOffset = 0;
}



//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
//				S3DSOBJECT CONSTRUCTOR/DESTRUCTOR				  //
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
C3DSModel::S3DSObject::S3DSObject()
{
	memset( name, 0, 80 );
	pVerts = NULL;
	pNorms = NULL;
	pTexCoords = NULL;
	pFaces = NULL;
	pMatFaces = NULL;
	vbo = ibo = 0;
}


C3DSModel::S3DSObject::~S3DSObject()
{
	memset( name, 0, 80 );
	if( pVerts ) delete[] pVerts;
	if( pNorms ) delete[] pNorms;
	if( pTexCoords ) delete[] pTexCoords;
	if( pFaces ) delete[] pFaces;
	if( pMatFaces ) delete[] pMatFaces;	
	pVerts = NULL;
	pNorms = NULL;
	pTexCoords = NULL;
	pFaces = NULL;
	pMatFaces = NULL;

	if( IS_HANDLE_VALID( vbo ) )
		glDeleteBuffers( 1, &vbo );
	vbo = 0;

	if( IS_HANDLE_VALID( ibo ) ) 
		glDeleteBuffers( 1, &ibo );
	ibo = 0;
}




//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
//				S3DSMATERIAL CONSTRUCTOR/DESTRUCTOR				  //
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
C3DSModel::S3DSMaterial::S3DSMaterial()
{
	memset( name, 0, 80 );
	pTexture = NULL;
	hasTexture = 0;
	color = 0xFFFFFFFF;
}

C3DSModel::S3DSMaterial::~S3DSMaterial()
{
	if( pTexture ) delete pTexture;
	memset( name, 0, 80 );
	pTexture = NULL;
	hasTexture = 0;
	color = 0xFFFFFFFF;
}



//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
//				C3DSMODEL CONSTRUCTOR/DESTRUCTOR				  //
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
C3DSModel::C3DSModel()
{
	pModelName = NULL;
	pFilePath  = NULL;
	pMaterials = NULL;
	pObjects   = NULL;
	bin3DS	   = NULL;
}


C3DSModel::~C3DSModel()
{
	if( pMaterials ) delete[] pMaterials;
	if( pObjects ) delete[] pObjects;
	if( bin3DS ) fclose( bin3DS );
	bin3DS = NULL;
	pModelName = NULL;
	pFilePath = NULL;
	pMaterials = NULL;
	pObjects = NULL;
}




//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
//					C3DSMODEL METHODS							  //
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Load
// Load 3DS Model from file name
// Return:
//     true- success
//     false- fail
bool C3DSModel::Load( char* name )
{
	S3DSHeaderChunk main;
	if( strstr( name, "\"" ) )
		name = strtok( name, "\"" );

	if( strstr( name, "/" ) || strstr( name, "\\" ) )
	{
		char* temp;
		if( strstr( name, "/" ) ) 
			temp = strrchr( name, '/' );
		else
			temp = strrchr( name, '\\' );

		pFilePath = new char[strlen( name ) - strlen( temp ) + 1];
		char* src = name + strlen( name ) - 1;

		while( src != pFilePath && ! ( ( * ( src - 1 ) ) == '\\' || ( * ( src - 1 ) ) == '/' ) ) 
			--src;

		memcpy( pFilePath, name, src - name );
		pFilePath[src - name] = 0;
	}

	bin3DS = fopen( name, "rb" );
	if( !bin3DS )
		return false;

	fseek( bin3DS, 0, SEEK_SET );
	fread( &main.id, sizeof( main.id ), 1, bin3DS );
	fread( &main.len, sizeof( main.len ), 1, bin3DS );

	MainChunkProcessor( main.len, ftell( bin3DS ) );

	fclose( bin3DS );
	bin3DS = NULL;

	CalculateVertexNormals();

	int nameLen = strlen( name );
	pModelName = new char[nameLen];
	strcpy( pModelName, name );
	nFaces = nVerts = 0;

	for( unsigned i = 0; i < nObjects; ++i )
	{
		nFaces += pObjects[i].nFaces / 3;
		nVerts += pObjects[i].nVerts;
	}

	for( unsigned k = 0; k < nObjects; ++k )
	{
		if( pObjects[k].nTexCoords == 0 && pObjects[k].nVerts > 0 )
		{
			pObjects[k].nTexCoords = pObjects[k].nVerts;
			pObjects[k].pTexCoords = new float[pObjects[k].nTexCoords * 2];
			
			for( unsigned c = 0; c < pObjects[k].nTexCoords; ++c )
			{
				pObjects[k].pTexCoords[2 * c] = pObjects[k].pVerts[3 * c];
				pObjects[k].pTexCoords[2 * c + 1] = pObjects[k].pVerts[3 * c + 1];
			}	
		}
	}

	for( unsigned j = 0; j < nMaterials; ++j )
	{
		if( !pMaterials[j].hasTexture )
		{
			pMaterials[j].pTexture = new CImage;
			pMaterials[j].pTexture->GenerateColorTexture( pMaterials[j].color, PIXEL_FORMAT_RGBA8 );
			pMaterials[j].texHandle = g_pTexInterface->AddTexture( pMaterials[j].pTexture, SAMPLE_LINEAR | SAMPLE_CLAMP );
			pMaterials[j].hasTexture = true;

			delete pMaterials[j].pTexture; pMaterials[j].pTexture = NULL;
		}	
	}

	// Load vertex buffer and index buffer assets into the GL //
	for( unsigned i = 0; i < nObjects; ++i )
	{
		if( pObjects[i].nVerts <= 0 || pObjects[i].nFaces <= 0 )
			continue;

		glGenBuffers( 1, &pObjects[i].vbo );
		glGenBuffers( 1, &pObjects[i].ibo );

		// Interleave the data //
		S3DSVertex* vlist = new S3DSVertex[pObjects[i].nVerts];
		for( unsigned j = 0; j < pObjects[i].nVerts; ++j )
		{
			vlist[j].pos.set( pObjects[i].pVerts[j * 3], pObjects[i].pVerts[j * 3 + 1], pObjects[i].pVerts[j * 3 + 2] );
			vlist[j].norm.set( pObjects[i].pNorms[j * 3], pObjects[i].pVerts[j * 3 + 1], pObjects[i].pVerts[j * 3 + 2] );
			vlist[j].texCoord.set( pObjects[i].pTexCoords[j * 2], pObjects[i].pTexCoords[j * 2 + 1] );
		}

		glBindBuffer( GL_ARRAY_BUFFER_ARB, pObjects[i].vbo );
		glBufferData( GL_ARRAY_BUFFER_ARB, sizeof( S3DSVertex ) * pObjects[i].nVerts, vlist, GL_STATIC_DRAW_ARB );
		glBindBuffer( GL_ARRAY_BUFFER_ARB, 0 );

		unsigned int nIndices = 0;
		unsigned short curIndex = 0;
		if( pObjects[i].m_nMatFaces > 0 )
		{
			for( unsigned j = 0; j < pObjects[i].m_nMatFaces; ++j )
			{
				pObjects[i].pMatFaces[j].iboOffset = nIndices;
				nIndices += pObjects[i].pMatFaces[j].nSubFaces * 3;
			}
		}

		unsigned short* plist = new unsigned short[nIndices];
		if( pObjects[i].m_nMatFaces > 0 )
		{
			for( unsigned j = 0; j < pObjects[i].m_nMatFaces; ++j )
			{
				for( unsigned c = 0; c < pObjects[i].pMatFaces[j].nSubFaces; c += 3 )
				{
					plist[c] = pObjects[i].pMatFaces[j].pSubFaces[c]; ++curIndex;
					plist[c + 1] = pObjects[i].pMatFaces[j].pSubFaces[c + 1]; ++curIndex;
					plist[c + 2] = pObjects[i].pMatFaces[j].pSubFaces[c + 2]; ++curIndex;
				}
//				memcpy( ( void* )( plist + curIndex ), pObjects[i].pMatFaces[j].pSubFaces, sizeof( unsigned short ) * pObjects[i].pMatFaces[j].nSubFaces * 3 );
//				curIndex += pObjects[i].pMatFaces[j].nSubFaces * 3;
			}
		} 

		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB, pObjects[i].ibo );
		glBufferData( GL_ELEMENT_ARRAY_BUFFER_ARB, sizeof( unsigned short ) * nIndices, plist, GL_STATIC_DRAW_ARB );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB, 0 );

		delete[] vlist;
		delete[] plist;
		vlist = NULL;
		plist = NULL;
	}

	return true;
}


void C3DSModel::CalculateVertexNormals()
{
	for( unsigned i = 0; i < nObjects; ++i )
	{
		if( pObjects[i].nVerts > 0 )
		{
			for( unsigned j = 0; j < pObjects[i].nVerts; ++j )
			{
				vec3f norm( pObjects[i].pNorms[j * 3], pObjects[i].pNorms[j * 3 + 1], pObjects[i].pNorms[j * 3 + 2] );
				norm.normalize();

				pObjects[i].pNorms[j * 3] = norm.x;
				pObjects[i].pNorms[j * 3 + 1] = norm.y;
				pObjects[i].pNorms[j * 3 + 2] = norm.z;
			}
		}
	}

//	for( int i = 0; i < nObjects; ++i ) 
//	{
//		CreateVertexNormals( (vec3*)pObjects[i].pVerts, pObjects[i].nVerts, pObjects[i].pFaces, pObjects[i].nFaces, pObjects[i].pNorms );
//	}
}


void C3DSModel::MainChunkProcessor( long len, long index )
{
	S3DSHeaderChunk h;
	fseek( bin3DS, index, SEEK_SET );

	while( ftell( bin3DS ) < ( index + len - 6 ) )
	{
		fread( &h.id, sizeof( h.id ), 1, bin3DS );
		fread( &h.len, sizeof( h.len ), 1, bin3DS );

		switch( h.id )
		{
			case EDIT3DS:
				EditChunkProcessor( h.len, ftell( bin3DS ) );
				break;

			case KEYF3DS:
				break;

			default:
				break;
		}

		fseek( bin3DS, ( h.len - 6 ), SEEK_CUR );
	}

	fseek( bin3DS, index, SEEK_SET );
}



void C3DSModel::EditChunkProcessor( long len, long index )
{
	S3DSHeaderChunk h;
	fseek( bin3DS, index, SEEK_SET );

	while( ftell( bin3DS ) < ( index + len - 6 ) )
	{
		fread( &h.id, sizeof( h.id ), 1, bin3DS );
		fread( &h.len, sizeof( h.len ), 1, bin3DS );

		switch( h.id )
		{
			case OBJECT:
				++nObjects;
				break;

			case MATERIAL:
				++nMaterials;
				break;

			default:
				break;
		}

		fseek( bin3DS, ( h.len - 6 ), SEEK_CUR );
	}

	if( nMaterials > 0 )
	{
		pMaterials = new S3DSMaterial[nMaterials];
		for( unsigned k = 0; k < nMaterials; ++k )
			pMaterials[k].hasTexture = false;

		fseek( bin3DS, index, SEEK_SET );
		int i = 0;
		while( ftell( bin3DS ) < ( index + len - 6 ) )
		{
			fread( &h.id, sizeof( h.id ), 1, bin3DS );
			fread( &h.len, sizeof( h.len ), 1, bin3DS );

			switch( h.id )
			{
				case MATERIAL:
				{
					MaterialChunkProcessor( h.len, ftell( bin3DS ), i );
					++i;
					break;
				}

				default:
					break;
			}

			fseek( bin3DS, ( h.len - 6 ), SEEK_CUR );
		}
	}

	if( nObjects > 0 )
	{
		pObjects = new S3DSObject[nObjects];
		for( unsigned i = 0; i < nObjects; ++i )
		{
			pObjects[i].isTextured = false;
			pObjects[i].pos.set( 0.0f, 0.0f, 0.0f );
			pObjects[i].rot.set( 0.0f, 0.0f, 0.0f );
			pObjects[i].nTexCoords = 0;
		}

		fseek( bin3DS, index, SEEK_SET );
		int j = 0;

		while( ftell( bin3DS ) < ( index + len - 6 ) )
		{
			fread( &h.id, sizeof( h.id ), 1, bin3DS );
			fread( &h.len, sizeof( h.len ), 1, bin3DS );

			switch( h.id )
			{
				case OBJECT:
					ObjectChunkProcessor( h.len, ftell( bin3DS ), j );
					++j;
					break;

				default:
					break;
			}

			fseek( bin3DS, ( h.len - 6 ), SEEK_CUR );
		}
	}

	fseek( bin3DS, index, SEEK_SET );
}


void C3DSModel::MaterialChunkProcessor( long len, long index, int matIndex )
{
	S3DSHeaderChunk h;
	fseek( bin3DS, index, SEEK_SET );

	while( ftell( bin3DS ) < ( index + len - 6 ) )
	{
		fread( &h.id, sizeof( h.id ), 1, bin3DS );
		fread( &h.len, sizeof( h.len ), 1, bin3DS );

		switch( h.id )
		{
			case MAT_NAME:
				MaterialNameChunkProcessor( h.len, ftell( bin3DS ), matIndex );
				break;

			case MAT_DIFFUSE:
				DiffuseColorChunkProcessor( h.len, ftell( bin3DS ), matIndex );
				break;

			case MAT_TEXMAP:
				TextureMapChunkProcessor( h.len, ftell( bin3DS ), matIndex );
				break;
			
			default:
				break;
		}

		fseek( bin3DS, ( h.len - 6 ), SEEK_CUR );
	}

	fseek( bin3DS, index, SEEK_SET );
}


void C3DSModel::MaterialNameChunkProcessor( long len, long index, int matIndex )
{
	fseek( bin3DS, index, SEEK_SET );
	for( unsigned i = 0; i < 80; ++i )
	{
		pMaterials[matIndex].name[i] = fgetc( bin3DS );
		if( pMaterials[matIndex].name[i] == 0 )
		{
			pMaterials[matIndex].name[i] = NULL;
			break;
		}
	}

	fseek( bin3DS, index, SEEK_SET );
}


void C3DSModel::DiffuseColorChunkProcessor( long len, long index, int matIndex )
{
	S3DSHeaderChunk h;
	fseek( bin3DS, index, SEEK_SET );

	while( ftell( bin3DS ) < ( index + len - 6 ) ) 
	{
		fread( &h.id, sizeof( h.id ), 1, bin3DS );
		fread( &h.len, sizeof( h.len ), 1, bin3DS );

		switch( h.id )
		{
			case COLOR_RGB:
			case COLOR_RGBG:
				FloatColorChunkProcessor( h.len, ftell( bin3DS ), matIndex );
				break;

			case COLOR_TRU:
			case COLOR_TRUG:
				IntColorChunkProcessor( h.len, ftell( bin3DS ), matIndex );
				break;

			default:
				break;
		}

		fseek( bin3DS, ( h.len - 6 ), SEEK_CUR );
	}

	fseek( bin3DS, index, SEEK_SET );
}


void C3DSModel::FloatColorChunkProcessor( long len, long index, int matIndex )
{
	float r, g, b;
	fseek( bin3DS, index, SEEK_SET );

	fread( &r, sizeof( r ), 1, bin3DS );
	fread( &g, sizeof( g ), 1, bin3DS );
	fread( &b, sizeof( b ), 1, bin3DS );

	pMaterials[matIndex].color = 0x000000ff;
	pMaterials[matIndex].color |= ( ( ( (unsigned char)( r * 255 ) ) << 24 ) | ( ( (unsigned char)( g * 255 ) ) << 16 ) | ( (unsigned char)( b * 255 ) ) << 8 );

	fseek( bin3DS, index, SEEK_SET );
}


void C3DSModel::IntColorChunkProcessor( long len, long index, int matIndex )
{
	unsigned char r, g, b;
	fseek( bin3DS, index, SEEK_SET );
	
	fread( &r, sizeof( r ), 1, bin3DS );
	fread( &g, sizeof( g ), 1, bin3DS );
	fread( &b, sizeof( b ), 1, bin3DS );
	
	pMaterials[matIndex].color = 0x000000ff;
	pMaterials[matIndex].color |= ( ( r << 24 ) | ( g << 16 ) | ( b << 8 ) );	

	fseek( bin3DS, index, SEEK_SET );
}



void C3DSModel::TextureMapChunkProcessor( long len, long index, int matIndex )
{
	S3DSHeaderChunk h;
	fseek( bin3DS, index, SEEK_SET );

	while( ftell( bin3DS ) < ( index + len - 6 ) )
	{
		fread( &h.id, sizeof( h.id ), 1, bin3DS );
		fread( &h.len, sizeof( h.len ), 1, bin3DS );

		switch( h.id )
		{
			case MAT_MAPNAME:
				MapNameChunkProcessor( h.len, ftell( bin3DS ), matIndex );
				break;

			default:
				break;
		}

		fseek( bin3DS, ( h.len - 6 ), SEEK_CUR );
	}

	fseek( bin3DS, index, SEEK_SET );
}




void C3DSModel::MapNameChunkProcessor( long len, long index, int matIndex )
{
	char name[80];
	fseek( bin3DS, index, SEEK_SET );

	for( unsigned i = 0; i < 80; ++i )
	{
		name[i] = fgetc( bin3DS );
		if( name[i] == 0 )
		{
			name[i] = NULL;
			break;
		}
	}

	char fullName[80];
	char* texturePath = "Media/Textures/";
	sprintf( fullName, "%s%s", texturePath, name );
	
	pMaterials[matIndex].pTexture = NULL;
	pMaterials[matIndex].texHandle = g_pTexInterface->AddTexture( fullName, SAMPLE_BILINEAR );
	if( !IS_HANDLE_VALID( pMaterials[matIndex].texHandle ) )
		pMaterials[matIndex].hasTexture = false;
	else
		pMaterials[matIndex].hasTexture = true;

	fseek( bin3DS, index, SEEK_SET );
}


void C3DSModel::ObjectChunkProcessor( long len, long index, int objIndex )
{
	S3DSHeaderChunk h;
	fseek( bin3DS, index, SEEK_SET );

	for( unsigned i = 0; i < 80; ++i )
	{
		pObjects[objIndex].name[i] = fgetc( bin3DS );
		if( pObjects[objIndex].name[i] == 0 )
		{
			pObjects[objIndex].name[i] = NULL;
			break;
		}
	}

	while( ftell( bin3DS ) < ( index + len - 6 ) )
	{
		fread( &h.id, sizeof( h.id ), 1, bin3DS );
		fread( &h.len, sizeof( h.len ), 1, bin3DS );

		switch( h.id )
		{
			case TRIG_MESH:
				TriangularMeshChunkProcessor( h.len, ftell( bin3DS ), objIndex );
				break;

			default:
				break;
		}

		fseek( bin3DS, ( h.len - 6 ), SEEK_CUR );
	}

	fseek( bin3DS, index, SEEK_SET );
}


void C3DSModel::TriangularMeshChunkProcessor( long len, long index, int objIndex )
{
	S3DSHeaderChunk h;
	fseek( bin3DS, index, SEEK_SET );

	while( ftell( bin3DS ) < ( index + len - 6 ) )
	{
		fread( &h.id, sizeof( h.id ), 1, bin3DS );
		fread( &h.len, sizeof( h.len ), 1, bin3DS );

		switch( h.id )
		{
			case VERT_LIST:
				VertexListChunkProcessor( h.len, ftell( bin3DS ), objIndex );
				break;

			case TEX_VERTS:
				TexCoordsChunkProcessor( h.len, ftell( bin3DS ), objIndex );
				pObjects[objIndex].isTextured = true;
				break;

			default:
				break;
		}

		fseek( bin3DS, ( h.len - 6 ), SEEK_CUR );
	}

	fseek( bin3DS, index, SEEK_SET );

	while( ftell( bin3DS ) < ( index + len - 6 ) )
	{
		fread( &h.id, sizeof( h.id ), 1, bin3DS );
		fread( &h.len, sizeof( h.len ), 1, bin3DS );

		switch( h.id )
		{
			case FACE_DESC:
				FacesDescriptionChunkProcessor( h.len, ftell( bin3DS ), objIndex );
				break;

			default:
				break;
		}

		fseek( bin3DS, ( h.len - 6 ), SEEK_CUR );
	}

	fseek( bin3DS, index, SEEK_SET );
}


void C3DSModel::VertexListChunkProcessor( long len, long index, int objIndex )
{
	unsigned short nVerts;
	fseek( bin3DS, index, SEEK_SET );

	fread( &nVerts, sizeof( nVerts ), 1, bin3DS );

	pObjects[objIndex].pVerts = new float[nVerts * 3];
	pObjects[objIndex].pNorms = new float[nVerts * 3];
	pObjects[objIndex].nVerts = nVerts;

	for( unsigned short i = 0; i < nVerts * 3; i += 3 )
	{
		fread( &pObjects[objIndex].pVerts[i], sizeof( float ), 1, bin3DS );
		fread( &pObjects[objIndex].pVerts[i + 2], sizeof( float ), 1, bin3DS );
		fread( &pObjects[objIndex].pVerts[i + 1], sizeof( float ), 1, bin3DS );

		pObjects[objIndex].pVerts[i + 2] *= -1.0f;
	}

	fseek( bin3DS, index, SEEK_SET );
}


void C3DSModel::TexCoordsChunkProcessor( long len, long index, int objIndex )
{
	unsigned short nCoords;
	fseek( bin3DS, index, SEEK_SET );

	fread( &nCoords, sizeof( nCoords ), 1, bin3DS );

	pObjects[objIndex].pTexCoords = new float[nCoords * 2];
	pObjects[objIndex].nTexCoords = nCoords;

	for( unsigned short i = 0; i < nCoords * 2; i += 2 )
	{
		fread( &pObjects[objIndex].pTexCoords[i], sizeof( float ), 1, bin3DS );
		fread( &pObjects[objIndex].pTexCoords[i + 1], sizeof( float ), 1, bin3DS );
	}

	fseek( bin3DS, index, SEEK_SET );
}


void C3DSModel::FacesDescriptionChunkProcessor( long len, long index, int objIndex )
{
	S3DSHeaderChunk h;
	unsigned short nFaces;
	unsigned short vertA;
	unsigned short vertB;
	unsigned short vertC;
	unsigned short flags;
	long subs;
	unsigned int nMatFaces = 0;

	fseek( bin3DS, index, SEEK_SET );
	fread( &nFaces, sizeof( nFaces ), 1, bin3DS );

	pObjects[objIndex].pFaces = new unsigned short[nFaces * 3];
	pObjects[objIndex].nFaces = nFaces * 3;

	for( unsigned short i = 0; i < nFaces * 3; i += 3 )
	{
		fread( &vertA, sizeof( vertA ), 1, bin3DS );
		fread( &vertB, sizeof( vertB ), 1, bin3DS );
		fread( &vertC, sizeof( vertC ), 1, bin3DS );
		fread( &flags, sizeof( flags ), 1, bin3DS );

		pObjects[objIndex].pFaces[i] = vertA;
		pObjects[objIndex].pFaces[i + 1] = vertB;
		pObjects[objIndex].pFaces[i + 2] = vertC;

		vec3f n, v1, v2, v3;

		v1.set( pObjects[objIndex].pVerts[vertA * 3], pObjects[objIndex].pVerts[vertA * 3 + 1], pObjects[objIndex].pVerts[vertA * 3 + 2] );
		v2.set( pObjects[objIndex].pVerts[vertB * 3], pObjects[objIndex].pVerts[vertB * 3 + 1], pObjects[objIndex].pVerts[vertB * 3 + 2] );
		v3.set( pObjects[objIndex].pVerts[vertC * 3], pObjects[objIndex].pVerts[vertC * 3 + 1], pObjects[objIndex].pVerts[vertC * 3 + 2] );

		vec3f u, v;
		u = v2 - v3;
		v = v2 - v1;
		n = u.crossProd( v );

		pObjects[objIndex].pNorms[vertA * 3]		+= n.x;
		pObjects[objIndex].pNorms[vertA * 3 + 1]    += n.y;
		pObjects[objIndex].pNorms[vertA * 3 + 2]    += n.z;
		pObjects[objIndex].pNorms[vertB * 3]		+= n.x;
		pObjects[objIndex].pNorms[vertB * 3 + 1]	+= n.y;
		pObjects[objIndex].pNorms[vertB * 3 + 2]	+= n.z;
		pObjects[objIndex].pNorms[vertC * 3]		+= n.x;
		pObjects[objIndex].pNorms[vertC * 3 + 1]	+= n.y;
		pObjects[objIndex].pNorms[vertC * 3 + 2]	+= n.z;
	}

	subs = ftell( bin3DS );
	while( ftell( bin3DS ) < ( index + len - 6 ) )
	{
		fread( &h.id, sizeof( h.id ), 1, bin3DS );
		fread( &h.len, sizeof( h.len ), 1, bin3DS );

		switch( h.id )
		{
			case FACE_MAT:
				++nMatFaces;
				break;

			default:
				break;
		}

		fseek( bin3DS, ( h.len - 6 ), SEEK_CUR );
	}

	if( nMatFaces > 0 )
	{
		pObjects[objIndex].pMatFaces = new S3DSMaterialFaces[nMatFaces];
		pObjects[objIndex].m_nMatFaces = nMatFaces;

		fseek( bin3DS, subs, SEEK_SET );

		int j = 0;
		while( ftell( bin3DS ) < ( index + len - 6 ) ) 
		{
			fread( &h.id, sizeof( h.id ), 1, bin3DS );
			fread( &h.len, sizeof( h.len ), 1, bin3DS );

			switch( h.id )
			{
				case FACE_MAT:
					FacesMaterialsListChunkProcessor( h.len, ftell( bin3DS ), objIndex, j );
					++j;
					break;

				default:
					break;
			}

			fseek( bin3DS, ( h.len - 6 ), SEEK_CUR );
		}
	}

	fseek( bin3DS, index, SEEK_SET );
}


void C3DSModel::FacesMaterialsListChunkProcessor( long len, long index, int objIndex, int subFacesIndex )
{
	char name[80];
	unsigned short nEntries;
	unsigned short face;
	unsigned int material;

	fseek( bin3DS, index, SEEK_SET );

	for( unsigned i = 0; i < 80; ++i )
	{
		name[i] = fgetc( bin3DS );
		if( name[i] == 0 )
		{
			name[i] = NULL;
			break;
		}
	}

	for( material = 0; material < nMaterials; ++material )
	{
		if( strcmp( name, pMaterials[material].name ) == 0 )
			break;
	}

	pObjects[objIndex].pMatFaces[subFacesIndex].matIndex = material;
	fread( &nEntries, sizeof( nEntries ), 1, bin3DS );

	pObjects[objIndex].pMatFaces[subFacesIndex].pSubFaces = new unsigned short[nEntries * 3];
	pObjects[objIndex].pMatFaces[subFacesIndex].nSubFaces = nEntries * 3;

	for( unsigned short i = 0; i < nEntries * 3; i += 3 )
	{
		fread( &face, sizeof( face ), 1, bin3DS );

		pObjects[objIndex].pMatFaces[subFacesIndex].pSubFaces[i] = pObjects[objIndex].pFaces[face * 3];
		pObjects[objIndex].pMatFaces[subFacesIndex].pSubFaces[i + 1] = pObjects[objIndex].pFaces[face * 3 + 1];
		pObjects[objIndex].pMatFaces[subFacesIndex].pSubFaces[i + 2] = pObjects[objIndex].pFaces[face * 3 + 2];
	}

	fseek( bin3DS, index, SEEK_SET );
}


void C3DSModel::Render()
{
	char* base = 0;

	for( unsigned i = 0; i < nObjects; ++i )
	{
		if( pObjects[i].nVerts <= 0 || pObjects[i].nFaces <= 0 )
			continue;

		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB, pObjects[i].ibo );
		glBindBuffer( GL_ARRAY_BUFFER_ARB, pObjects[i].vbo );

		glEnableClientState( GL_VERTEX_ARRAY );
		glEnableClientState( GL_NORMAL_ARRAY );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );

		glVertexPointer( 3, GL_FLOAT, sizeof( S3DSVertex ), base );
		glNormalPointer( GL_FLOAT, sizeof( S3DSVertex ), base + 12 );
		glTexCoordPointer( 2, GL_FLOAT, sizeof( S3DSVertex ), base + 24 );

		if( pObjects[i].m_nMatFaces > 0 )
		{
			for( unsigned j = 0; j < pObjects[i].m_nMatFaces; ++j )
			{
				if( IS_HANDLE_VALID( pMaterials[pObjects[i].pMatFaces[j].matIndex].texHandle ) )
					if( g_pShaderInterface->IsShaderBound() )
					{
						int textured = 1;
						g_pShaderInterface->SetInt( "g_bIsTextured", &textured, 1 );
						g_pShaderInterface->SetTexture( "g_diffuseTexture", pMaterials[pObjects[i].pMatFaces[j].matIndex].texHandle );
					}
					else
						g_pTexInterface->BindTexture( pMaterials[pObjects[i].pMatFaces[j].matIndex].texHandle );

				glDrawElements( GL_TRIANGLES, pObjects[i].pMatFaces[j].nSubFaces, GL_UNSIGNED_SHORT, base + pObjects[i].pMatFaces[j].iboOffset );

				int textured = 0;
				g_pShaderInterface->SetInt( "g_bIsTextured", &textured, 1 );
			}
		}

		glDisableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_NORMAL_ARRAY );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );

		glBindBuffer( GL_ARRAY_BUFFER_ARB, 0 );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB, 0 );

		g_pTexInterface->EvictTextures();
	}
}