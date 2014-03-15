#include "quickmodel.h"


std::string COBJModel::LoadModel(const char* fileName, const char* filePath)
{
	std::string fName = filePath;
	fName.append(fileName);

	std::string err = tinyobj::LoadObj(m_shapes, fName.c_str(), "Media/Models/");
	if (err.length() >= 1)
		return err;

	// Load up textures //
	for(int i = 0; i < m_shapes.size(); ++i)
	{
		std::string texName = m_shapes[i].material.diffuse_texname;
		size_t slash = texName.find_first_of("\\");
		if(slash != std::string::npos)
			texName.replace(slash, 1, "/");
		std::string finalTexName = filePath;
		finalTexName.append(texName);
		int texID = g_pTexInterface->AddTexture(finalTexName.c_str());
		m_materialDiffuseTextures.push_back(texID);

		texName.clear();
		texName = m_shapes[i].material.specular_texname;
		slash = texName.find_first_of("\\");
		if(slash != std::string::npos)
			texName.replace(slash, 1, "/");
		finalTexName.clear();
		finalTexName = filePath;
		finalTexName.append(texName);
		texID = g_pTexInterface->AddTexture(finalTexName.c_str());
		m_materialSpecularTextures.push_back(texID);
	}

	return err;
}