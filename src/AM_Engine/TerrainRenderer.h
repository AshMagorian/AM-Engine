#pragma once
#include <memory>
#include <list>

class Application;
class ShaderProgram;

class TerrainRenderer
{
private:
	TerrainRenderer() {}

	std::weak_ptr<Application> m_app;
	std::shared_ptr<ShaderProgram> m_terrainGeoPassShader;

	void Init_Impl(std::weak_ptr<Application> _app);
	void RenderTerrain_Impl();
	void SetShaderUniforms();
public:
	TerrainRenderer(const TerrainRenderer&) = delete;
	static TerrainRenderer& Get()
	{
		static TerrainRenderer instance;
		return instance;
	}

	static void Init(std::weak_ptr<Application> _app) { return Get().Init_Impl(_app); }
	static void RenderTerrain() { return Get().RenderTerrain_Impl(); }

};