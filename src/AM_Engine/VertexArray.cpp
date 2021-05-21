#include "VertexArray.h"
#include "Exception.h"
#include "PrimitiveShape.h"
#include "ShaderProgram.h"

#include <fstream>
#include <iostream>

VertexArray::VertexArray()
{
	dirty = false;

	glGenVertexArrays(1, &id);
	if (!id)
	{
		throw std::exception();
	}
}

/**
*\brief Reads the model file and stores the data
*/
VertexArray::VertexArray(std::string path)
{
	if (path == "sphere")
	{
		id = PrimitiveShape::SetupSphere(indexCount);
		dirty = false;
		renderMethod = GL_TRIANGLE_STRIP;
		return;
	}
	if (path == "plane")
	{
		id = PrimitiveShape::SetupPlane(indexCount);
		dirty = false;
		renderMethod = GL_TRIANGLES;
		return;
	}
	if (path == "cube")
	{
		id = PrimitiveShape::SetupCube(indexCount);
		dirty = false;
		renderMethod = GL_TRIANGLES;
		return;
	}
}

GLuint VertexArray::GetId()
{
	return id;
}

void VertexArray::Draw(std::shared_ptr<ShaderProgram> shader)
{
	glBindVertexArray(id);
	shader->BindTextures();
	glDrawElements(renderMethod, indexCount, GL_UNSIGNED_INT, 0);
	shader->UnbindTextures();
	glBindVertexArray(0);
}