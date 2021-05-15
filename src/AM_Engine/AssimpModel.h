#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "AssimpMesh.h"
#include "Resource.h"

#define FLIP_TEXTURES 1

class AssimpModel : public Resource
{
private:
	std::vector<AssimpTexture> textures_loaded;
	// model data
	std::vector<AssimpMesh> meshes;
	std::string directory;

	void LoadModel(std::string path);
	void ProcessNode(aiNode *node, const aiScene *scene);
	AssimpMesh ProcessMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<AssimpTexture> LoadMaterialTextures(aiMaterial *mat, aiTextureType type,
		std::string typeName);
	unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);

public:
	AssimpModel(std::string path)
	{
		LoadModel(path.c_str());
	}
	AssimpModel(std::string path, int flip);

	void Draw(std::shared_ptr<ShaderProgram> shader);
	void DrawOutline(std::shared_ptr<ShaderProgram> shader);
};