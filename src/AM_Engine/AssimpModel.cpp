#include "AssimpModel.h"

#include <assimp/pbrmaterial.h>
#include  "stb_image.h"

#include <iostream>

AssimpModel::AssimpModel(std::string path, const int flip)
{
	if (flip == FLIP_TEXTURES)
		stbi_set_flip_vertically_on_load(true);
	else
		stbi_set_flip_vertically_on_load(false);
	LoadModel(path.c_str());
}

void AssimpModel::LoadModel(std::string path)
{
	//stbi_set_flip_vertically_on_load(true);
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));
	ProcessNode(scene->mRootNode, scene);
}
void AssimpModel::ProcessNode(aiNode *node, const aiScene *scene)
{
	// process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(ProcessMesh(mesh, scene));
	}
	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene);
	}
}
AssimpMesh AssimpModel::ProcessMesh(aiMesh *mesh, const aiScene *scene)
{
	std::vector<Vertex> vertices;
	vertices.resize(mesh->mNumVertices);
	std::vector<unsigned int> indices;
	std::vector<AssimpTexture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		// process vertex positions, normals and texture coordinates
		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;

		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.Normal = vector;

		vector.x = mesh->mTangents[i].x;
		vector.y = mesh->mTangents[i].y;
		vector.z = mesh->mTangents[i].z;
		vertex.Tangent = vector;

		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);

		vertices.at(i) = vertex;
	}
	// process indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	// process material
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<AssimpTexture> diffuseMaps = LoadMaterialTextures(material,
			aiTextureType_DIFFUSE, "texture_diffuse");								// Diffuse = Albedo
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		std::vector<AssimpTexture> normalMaps = LoadMaterialTextures(material,
			aiTextureType_HEIGHT, "texture_normal");								// Height = Normal
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		std::vector<AssimpTexture> metallicMaps = LoadMaterialTextures(material,
			aiTextureType_SPECULAR, "texture_metallic");							// Specular = Metallic
		textures.insert(textures.end(), metallicMaps.begin(), metallicMaps.end());
		std::vector<AssimpTexture> roughnessMaps = LoadMaterialTextures(material,
			aiTextureType_SHININESS, "texture_roughness");							// Shininess = Roughness
		textures.insert(textures.end(), roughnessMaps.begin(), roughnessMaps.end());
		std::vector<AssimpTexture> aoMaps = LoadMaterialTextures(material,
			aiTextureType_AMBIENT, "texture_ao");									// Ambient = AO
		textures.insert(textures.end(), aoMaps.begin(), aoMaps.end());
	}

	return AssimpMesh(vertices, indices, textures);
}
std::vector<AssimpTexture> AssimpModel::LoadMaterialTextures(aiMaterial *mat, aiTextureType type,
	std::string typeName)
{
	std::vector<AssimpTexture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}
		if (!skip)
		{   // if texture hasn't been loaded already, load it
			AssimpTexture texture;
			texture.id = TextureFromFile(str.C_Str(), directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture); // add to loaded textures

			std::cout << str.C_Str() << " loaded for assimp" << std::endl;
		}
	}
	return textures;
}

unsigned int AssimpModel::TextureFromFile(const char *path, const std::string &directory, bool gamma)
{
	std::string filename = std::string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Assimp texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

void AssimpModel::Draw(std::shared_ptr<ShaderProgram> shader)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i].Draw(shader);
}
void AssimpModel::DrawOutline(std::shared_ptr<ShaderProgram> shader)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i].DrawOutline(shader);
}