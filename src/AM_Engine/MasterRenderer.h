#pragma once
#include <memory>
#include <list>

class Application;
class Entity;
class ShaderProgram;

class MasterRenderer
{
	friend class DebugUIManager;
private:
	MasterRenderer() {}

	std::weak_ptr<Application> m_app;
	std::shared_ptr<ShaderProgram> m_geometryPassShader, m_lightingPassShader;
	std::list<std::shared_ptr<Entity>> m_entityList;
	std::list<std::shared_ptr<Entity>> m_outlineList;
	unsigned int quadVAO = 0;
	unsigned int gBuffer;
	unsigned int gPosition, gNormal, gAlbedo, gMRA, rboDepth;
	int m_screenWidth, m_screenHeight;

	void Init_Impl(std::weak_ptr<Application> _app, const int& _w, const int& _h);
	void RenderScene_Impl(const int& _width, const int& _height);

	void InitLightingPassUniforms();
	void Init_GBuffer(const int& _w, const int& _h);
	void BindGBufferTextures();
	void RenderQuad();

public:
	MasterRenderer(const MasterRenderer&) = delete;

	static MasterRenderer& Get()
	{
		static MasterRenderer instance;
		return instance;
	}

	static std::shared_ptr<ShaderProgram> GetGeoShader() { return Get().m_geometryPassShader; }

	static void Init(std::weak_ptr<Application> _app, const int& _w, const int& _h) { return Get().Init_Impl(_app, _w, _h); }
	static void RenderScene(const int& _width,const int& _height) { return Get().RenderScene_Impl(_width, _height); }

};