#include "quickgeom.h"



void CreateVertexNormals( vec3* verts, const unsigned int& nVerts, const unsigned short* polys, const unsigned int& nPolys, float* norms)
{
	unsigned short* tmp;
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
			tmp = (unsigned short*)&polys[v * 3];
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
			vec3_sub( a, c, ac );
			vec3_sub( a, b, ab );

			vec3_cross( ac, ab, tmpNorm );
			vec3_normalize( tmpNorm );

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
		vec3_normalize( tmpNorm );
		norms[j * 3] = tmpNorm[0];
		norms[j * 3 + 1] = tmpNorm[1];
		norms[j * 3 + 2] = tmpNorm[2];
	}
}


