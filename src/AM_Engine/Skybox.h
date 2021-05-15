#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>

class ShaderProgram;
class Application;

struct CubemapTexture
{
	std::string name;
	GLuint id;
	GLuint preview_id;
};

class Skybox
{
	friend class DebugUIManager;
private:
	GLuint textureID;
	static std::vector<CubemapTexture> m_cubemaps;
	CubemapTexture m_currentMap;
	GLuint vaID;

	std::vector<std::string> m_texturesFaces;
	std::shared_ptr<ShaderProgram> m_shader;
	std::shared_ptr<ShaderProgram> m_hdrShader;
	std::shared_ptr<ShaderProgram> m_irradianceShader;
	std::shared_ptr<ShaderProgram> m_prefilterShader;
	std::weak_ptr<Application> m_application;

	void InitBoxVertexArray();
	void SetFaces(std::string _rt, std::string _lt, std::string _tp, std::string _bm, std::string _bk, std::string _ft);
	void SetShaderUniforms();

public:
	Skybox() {}

	void Init(std::weak_ptr<Application>_app);

	/*
	*Creates a BoxTexture from a HDR map
	*/
	void CreateSkybox(std::string _name, std::string _path);
	void CreateSkybox(std::string _name, std::string _right, std::string _left, std::string _top, std::string _bottom, std::string _back, std::string _front);
	void SetSkybox(std::string _name);
	std::string GetCurrentMapName() { return m_currentMap.name; }

	GLuint LoadHDRTexture(std::string _path);
	GLuint MakeCubemapFromHDR(GLuint _hdr_id, GLuint* _captureFBO, GLuint* _captureRBO, GLuint* _preview_id);
	GLuint MakeIrradianceMap(GLuint _captureFBO, GLuint _captureRBO, GLuint _textureID);
	GLuint MakePrefilterMap(GLuint _captureFBO, GLuint _captureRBO, GLuint _textureID);

	void DrawSkybox();
};