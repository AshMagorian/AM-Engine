#pragma once
#include <memory>
#include <list>
#include <glm/glm.hpp>

class Application;
class Entity;
class ShaderProgram;

class MasterRenderer
{
	friend class DebugUIManager;
	friend class WaterRenderer;
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
	glm::vec4 m_clippingPlane;
	bool m_simpleRendering; // Simple rendering is toggled on for rendering water reflections and refractions

	void Init_Impl(std::weak_ptr<Application> _app, const int& _w, const int& _h);
	void InitLightingPassUniforms();
	void Init_GBuffer(const int& _w, const int& _h);

	void RenderScene_Impl(const int& _width, const int& _height);
	
	void BindGeometryFramebuffer();
	void RenderGeometryPass(); //
	int UseGeometryPassShader();
	
	void RenderLightingPass();
	void UseLightPassShader();
	void BindGBufferTextures();
	void BindIBLTextures(); //
	void SetLightingUniforms(); //
	void RenderQuad(); //
	
	void BlitFrameBuffer(const int& _srcBuffer, const int& _dstBuffer, const int& _srcW, const int& _srcH, const int& _dstW, const int& _dstH);
	void RenderSkybox(); //

	void SetClippingPlane(glm::vec4 _plane);

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
	static bool IsSimpleRendering() { return Get().m_simpleRendering; }

};