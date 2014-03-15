#ifndef __QUICKMODEL_H_
#define __QUICKMODEL_H_


#include <string>
#include <vector>
#include "tiny_obj_loader.h"
#include "quickimage.h"


class CModelObject
{
	public:

		CModelObject() {}
		~CModelObject() {}

		// Return error string or null string 
		virtual std::string LoadModel(const char* fileName, const char* filePath = NULL) = 0;

	protected:

		std::vector<int> m_materialDiffuseTextures;
		std::vector<int> m_materialSpecularTextures;

	private:
};


class COBJModel : public CModelObject
{
	public:

		COBJModel() {}
		~COBJModel() {}

		std::string LoadModel(const char* fileName, const char* filePath = NULL);

	private:
		
		std::vector<tinyobj::shape_t> m_shapes;
};


#endif