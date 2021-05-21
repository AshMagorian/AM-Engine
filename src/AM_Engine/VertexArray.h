#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <vector>
#include <string>

#include "Resource.h"

class ShaderProgram;

class VertexArray : public Resource
{
private:
	GLuint id;
	int indexCount = 0;
	int renderMethod;
	bool dirty;

public:
	VertexArray();
	VertexArray(std::string path);

	void Draw(std::shared_ptr<ShaderProgram> shader);

	int GetIndexCount() { return indexCount; }
	GLuint GetId();
};