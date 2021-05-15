#pragma once

#include "Resource.h"

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <vector>

/**
*The texture class is a resource which stores the texture to apply to a model
*/
class Texture : public Resource
{
private:

	GLuint id;
	glm::vec2 size; ///< The size of the texture

	Texture(int width, int height);
public:
	Texture() {}
	/**
	*\brief Loads the texture from the path specified
	*/
	Texture(std::string path);
	/**
	*\brief Returns the size
	*/
	glm::vec2 getSize() { return size; }
	/**
	*\brief Returns the id
	*/
	GLuint getId() { return id; }
	void SetID(GLuint _id) { id = _id; }
};