#include "MasterRenderer.h"
#include "Application.h"
#include "Entity.h"
#include "SceneManager.h"
#include "OutlineRenderer.h"
#include "Skybox.h"
#include "ShaderProgram.h"
#include "PBR_Material.h"
#include "Lights.h"
#include "Camera.h"
#include "Transform.h"
#include "TerrainRenderer.h"
#include "WaterRenderer.h"

void MasterRenderer::Init_Impl(std::weak_ptr<Application> _app, const int& _w, const int& _h)
{
	m_app = _app;
	m_geometryPassShader = std::make_shared<ShaderProgram>("deferredGeometryPass.vert", "deferredGeometryPass.frag");
	m_lightingPassShader = std::make_shared<ShaderProgram>("deferredLightingPass.vert", "deferredLightingPass.frag");
	InitLightingPassUniforms();
	m_simpleRendering = false;

	Init_GBuffer(_w, _h);
	TerrainRenderer::Init(_app);
	WaterRenderer::Init(_app, _w, _h);
}

void MasterRenderer::InitLightingPassUniforms()
{
	glUseProgram(m_lightingPassShader->GetId());
	m_lightingPassShader->SetUniform("gPosition", 0);
	m_lightingPassShader->SetUniform("gNormal", 1);
	m_lightingPassShader->SetUniform("gAlbedo", 2);
	m_lightingPassShader->SetUniform("gMRA", 3);
	m_lightingPassShader->SetUniform("irradianceMap", 4);
	m_lightingPassShader->SetUniform("prefilterMap", 5);
	m_lightingPassShader->SetUniform("brdfLUT", 6);
	glUseProgram(0);
}

void MasterRenderer::RenderScene_Impl(const int& _width, const int& _height)
{
	// Update the entity list
	m_entityList = m_app.lock()->GetSceneManager()->GetCurrentScene()->entities;

	//Check if window size has changed
	if (_width != m_screenWidth || _height != m_screenHeight)
	{
		Init_GBuffer(_width, _height);
		WaterRenderer::ResetBuffers(_width, _height);
	}

	// Draw all outlined objects to fill up the stencil buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	m_app.lock()->GetOutlineRenderer()->StencilDraw();

	m_simpleRendering = false;

	BindGeometryFramebuffer();
	RenderGeometryPass();
	RenderLightingPass();
	BlitFrameBuffer(gBuffer, 0, m_screenWidth, m_screenHeight, m_screenWidth, m_screenHeight);
	RenderSkybox();

	m_simpleRendering = true;
	WaterRenderer::RenderWater();

	m_app.lock()->GetOutlineRenderer()->RenderOutlines();
}

void MasterRenderer::BindGeometryFramebuffer()
{
	glBindTexture(GL_TEXTURE_2D, 0);//To make sure the texture isn't bound
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glViewport(0, 0, m_screenWidth, m_screenHeight);
}
void MasterRenderer::RenderGeometryPass()
{
	// Geometry pass: render all geometric/color data to g-buffer 
	glClearColor(0.6f, 0.4f, 0.6f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	UseGeometryPassShader();
	m_geometryPassShader->SetUniform("in_ViewPos", m_app.lock()->GetCamera()->GetCurrentCamera()->GetTransform()->GetPos());
	m_geometryPassShader->SetUniform("in_clippingPlane", m_clippingPlane);
	for (std::list<std::shared_ptr<Entity>>::iterator i = m_entityList.begin(); i != m_entityList.end(); ++i)
	{
		(*i)->Display();
	}
	TerrainRenderer::RenderTerrain();
	// Reset to the default FrameBuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

int MasterRenderer::UseGeometryPassShader()
{
	int id = m_geometryPassShader->GetId();
	glUseProgram(id);
	return id;
}

void MasterRenderer::RenderLightingPass()
{
	UseLightPassShader();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	BindGBufferTextures();
	BindIBLTextures();
	SetLightingUniforms();
	RenderQuad();
}

void MasterRenderer::UseLightPassShader()
{
	glUseProgram(m_lightingPassShader->GetId());
}

void MasterRenderer::BindGBufferTextures()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, gMRA);
}

void MasterRenderer::BindIBLTextures()
{
	// binds IBL maps
	std::string mapName = m_app.lock()->GetSkybox()->GetCurrentMapName();
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_CUBE_MAP, PBR_Material::GetIrradiance(mapName));
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_CUBE_MAP, PBR_Material::GetPrefilter(mapName));
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, PBR_Material::GetBRDF());
}

void MasterRenderer::SetLightingUniforms()
{
	m_app.lock()->GetSceneManager()->GetCurrentScene()->lightManager->SetLightingUniforms(m_lightingPassShader);
}

void MasterRenderer::BlitFrameBuffer(const int& _srcBuffer, const int& _dstBuffer, const int& _srcW, const int& _srcH, const int& _dstW, const int& _dstH)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, _srcBuffer); // read from the gBuffer
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _dstBuffer); // write to default framebuffer
	glBlitFramebuffer(
		0, 0, _srcW, _srcH, 0, 0, _dstW, _dstH, GL_DEPTH_BUFFER_BIT, GL_NEAREST
	);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void MasterRenderer::RenderSkybox()
{
	m_app.lock()->GetSkybox()->DrawSkybox();
}

void MasterRenderer::SetClippingPlane(glm::vec4 _plane)
{
	m_clippingPlane = _plane;
	TerrainRenderer::Get().SetClippingPlane(_plane);
}

void MasterRenderer::Init_GBuffer(const int& _w, const int& _h)
{
	m_screenWidth = _w; m_screenHeight = _h;
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	// - position color buffer
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_screenWidth, m_screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	// - normal color buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_screenWidth, m_screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	// - color buffer
	glGenTextures(1, &gAlbedo);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_screenWidth, m_screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);

	// - mra buffer
	glGenTextures(1, &gMRA);
	glBindTexture(GL_TEXTURE_2D, gMRA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_screenWidth, m_screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gMRA, 0);

	// - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(4, attachments);

	// create and attach depth buffer (renderbuffer)
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_screenWidth, m_screenHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	// Check to see if the frame buffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void MasterRenderer::RenderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		unsigned int quadVBO;
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}