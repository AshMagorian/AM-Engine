#pragma once
#include <memory>
#include <list>
#include <glm/glm.hpp>

class Application;
class ShaderProgram;
class WaterBuffers;
class Transform;
class Texture;
class WaterTile;

class WaterRenderer
{
	friend class DebugUIManager;
private:
	WaterRenderer() {}

	std::weak_ptr<Application> m_app;
	std::shared_ptr<ShaderProgram> m_waterShader;
	std::list<std::shared_ptr<WaterTile>> m_waterTiles;
	std::shared_ptr<WaterBuffers> m_waterBuffers;
	std::shared_ptr<Texture> m_dudvMap;
	std::shared_ptr<Texture> m_normalMap;

	std::shared_ptr<Transform> m_cameraTransform;
	unsigned int quadVAO = 0; // The quad used for rendering water textures

	void Init_Impl(std::weak_ptr<Application> _app, const int& _screenWidth, const int& _screenHeight);
	void ResetBuffers_Impl(const int& _screenWidth, const int& _screenHeight);
	void RenderWater_Impl();

	void InitShaderUniforms();

	void SetReflectionCamera(const float& _distance);
	void RenderReflections();
	void ResetCamera(const float& _distance);
	void SetRefractionCamera();
	void RenderRefractions();

	void BindTextures();
	void SetShaderUniforms(std::shared_ptr<WaterTile> _waterTile);

	void RenderQuad();

public:
	WaterRenderer(const WaterRenderer&) = delete;
	static WaterRenderer& Get()
	{
		static WaterRenderer instance;
		return instance;
	}
	static void Init(std::weak_ptr<Application> _app, const int& _screenWidth, const int& _screenHeight) { return Get().Init_Impl(_app, _screenWidth, _screenHeight); }
	static void ResetBuffers(const int& _screenWidth, const int& _screenHeight) { return Get().ResetBuffers_Impl(_screenWidth, _screenHeight); }
	static void RenderWater() { return Get().RenderWater_Impl(); }
};