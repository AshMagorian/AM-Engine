#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <vector>
#include <string>

#include "Resource.h"
#include "VertexBuffer.h"

class VertexBuffer;
class ShaderProgram;

class VertexArray : public Resource
{
private:
	GLuint id;
	int indexCount = 0;
	int renderMethod;
	std::vector<std::shared_ptr<VertexBuffer>> buffers; ///< Stores the buffers (Which holds the model data)
	bool dirty;

public:
	VertexArray();
	VertexArray(std::string path);

	void Draw(std::shared_ptr<ShaderProgram> shader);

	int GetIndexCount() { return indexCount; }
	GLuint GetId();
};