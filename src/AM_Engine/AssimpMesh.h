#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <vector>
#include <string>

class ShaderProgram;

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec3 Tangent;
	glm::vec2 TexCoords;
};

struct AssimpTexture
{
	unsigned int id;
	std::string type;
	std::string path;
};

class AssimpMesh
{
private:
	unsigned int VAO, VBO, EBO;

	void SetupMesh();
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<AssimpTexture> textures;

	AssimpMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<AssimpTexture> textures);

	void Draw(std::shared_ptr<ShaderProgram> shader);
	void DrawOutline(std::shared_ptr<ShaderProgram> shader);

};