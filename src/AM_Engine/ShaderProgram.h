#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <fstream>
#include <string>
#include <vector>
#include "Resource.h"

class VertexArray;
class Texture;

struct Sampler
{
	GLint id;
	std::shared_ptr<Texture> texture;
};

class ShaderProgram : public Resource
{
private:
	GLuint id;
	std::vector<Sampler> samplers;

	void CreateShaderProgram(std::string _vert, std::string _frag);
	void ReadFromFile(std::string& _path, std::string& _text);
	void CompileShader(GLuint _id, std::string _shaderType);
	GLuint AttachVetexShader(std::string& _path);
	GLuint AttachFragmentShader(std::string& _path);

public:
	ShaderProgram(std::string _path);
	ShaderProgram(std::string _vert, std::string _frag);

	void BindTextures();
	void UnbindTextures();

	void Draw(std::shared_ptr<VertexArray> vertexArray);

	void SetUniform(std::string uniform, glm::vec2 value);
	void SetUniform(std::string uniform, glm::vec3 value);
	void SetUniform(std::string uniform, glm::vec4 value);
	void SetUniform(std::string uniform, float value);
	void SetUniform(std::string uniform, int value);
	void SetUniform(std::string uniform, glm::mat4 value);
	void SetUniform(std::string uniform, std::shared_ptr<Texture> texture);

	GLuint GetId() { return id; }
};