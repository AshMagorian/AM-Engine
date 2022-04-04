#include "WaterRenderer.h"
#include "MasterRenderer.h"
#include "ShaderProgram.h"
#include "WaterBuffers.h"
#include "Application.h"
#include "Camera.h"
#include "Entity.h"
#include "Transform.h"
#include "SceneManager.h"
#include "Texture.h"
#include "Lights.h"
#include "WaterTile.h"

#include <glm/ext.hpp>

void WaterRenderer::Init_Impl(std::weak_ptr<Application> _app, const int& _screenWidth, const int& _screenHeight)
{
	m_app = _app;
	m_waterShader = std::make_shared<ShaderProgram>("water.vert", "water.frag");
	InitShaderUniforms();
	m_dudvMap = std::make_shared<Texture>("../src/resources/Textures/water/dudv.png");
	m_normalMap = std::make_shared<Texture>("../src/resources/Textures/water/normal.png");
	m_waterBuffers = std::make_shared<WaterBuffers>();
	m_waterBuffers->Init( _screenWidth, _screenHeight);

	m_waterTiles.push_back(std::make_shared<WaterTile>(0.0f, 0.0f, -0.5f, 20.0f));
}

void WaterRenderer::ResetBuffers_Impl(const int& _screenWidth, const int& _screenHeight)
{
	m_waterBuffers->Init(_screenWidth, _screenHeight);
}

void WaterRenderer::InitShaderUniforms()
{
	glUseProgram(m_waterShader->GetId());
	m_waterShader->SetUniform("in_ReflectionTexture", 0);
	m_waterShader->SetUniform("in_RefractionTexture", 1);
	m_waterShader->SetUniform("in_dudvMap", 2);
	m_waterShader->SetUniform("in_normalMap", 3);
	m_waterShader->SetUniform("in_DepthMap", 4);
	glUseProgram(0);
}

void WaterRenderer::RenderWater_Impl()
{
	m_cameraTransform = m_app.lock()->GetSceneManager()->GetCurrentScene()->camera->GetTransform();
	glUseProgram(m_waterShader->GetId());
	glEnable(GL_DEPTH_TEST);

	for (std::list<std::shared_ptr<WaterTile>>::iterator i = m_waterTiles.begin(); i != m_waterTiles.end(); ++i)
	{
		glEnable(GL_CLIP_DISTANCE0);
		// Reflections
		MasterRenderer::Get().SetClippingPlane((*i)->GetReflectionPlane());
		float camDistanceAboveWater = 2.0f * (m_cameraTransform->GetPos().y - (*i)->GetHeight());
		SetReflectionCamera(camDistanceAboveWater);
		RenderReflections();
		ResetCamera(camDistanceAboveWater);

		// Refractions
		MasterRenderer::Get().SetClippingPlane((*i)->GetRefractionPlane());
		SetRefractionCamera();
		RenderRefractions();

		// Reset settings
		m_app.lock()->GetCamera()->UpdateMatrix(m_app.lock()->GetWindowWidth(), m_app.lock()->GetWindowHeight());
		glDisable(GL_CLIP_DISTANCE0);
		m_waterBuffers->UnbindCurrentFramebuffer(m_app.lock()->GetWindowWidth(), m_app.lock()->GetWindowHeight());

		// Set up shader
		glUseProgram(m_waterShader->GetId());
		BindTextures();
		SetShaderUniforms(*i);
		RenderQuad();
	}
	glUseProgram(0);
}

void WaterRenderer::SetReflectionCamera(const float& _distance)
{
	m_cameraTransform->SetPosY(m_cameraTransform->GetPos().y - _distance);
	m_app.lock()->GetCamera()->InvertPitch();
	m_app.lock()->GetCamera()->UpdateMatrix(m_waterBuffers->REFLECTION_WIDTH, m_waterBuffers->REFLECTION_HEIGHT);
}

void WaterRenderer::RenderReflections()
{
	// Geometry pass
	m_waterBuffers->BindReflectionGBuffer();
	MasterRenderer::Get().RenderGeometryPass();
	// Lighting pass
	MasterRenderer::Get().UseLightPassShader();
	m_waterBuffers->BindReflectionFramebuffer();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_waterBuffers->BindReflectionGBufferTextures();
	MasterRenderer::Get().BindIBLTextures();
	MasterRenderer::Get().SetLightingUniforms();
	MasterRenderer::Get().RenderQuad();
	// skybox
	m_waterBuffers->BlitReflectionFrameBuffer();
	MasterRenderer::Get().RenderSkybox();
}

void WaterRenderer::ResetCamera(const float& _distance)
{
	m_app.lock()->GetCamera()->InvertPitch();
	m_cameraTransform->SetPosY(m_cameraTransform->GetPos().y + _distance);
}

void WaterRenderer::SetRefractionCamera()
{
	m_app.lock()->GetCamera()->UpdateMatrix(m_waterBuffers->REFRACTION_WIDTH, m_waterBuffers->REFRACTION_HEIGHT);
}

void WaterRenderer::RenderRefractions()
{
	// Geometry pass
	m_waterBuffers->BindRefractionGBuffer();
	MasterRenderer::Get().RenderGeometryPass();
	// Lighting pass
	MasterRenderer::Get().UseLightPassShader();
	m_waterBuffers->BindRefractionFrameBuffer();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_waterBuffers->BindRefractionGBufferTextures();
	MasterRenderer::Get().BindIBLTextures();
	MasterRenderer::Get().SetLightingUniforms();
	MasterRenderer::Get().RenderQuad();
	// skybox
	m_waterBuffers->BlitRefractionFrameBuffer();
	MasterRenderer::Get().RenderSkybox();
}

void WaterRenderer::BindTextures()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_waterBuffers->GetReflectionTexture());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_waterBuffers->GetRefractionTexture());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_dudvMap->getId());
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_normalMap->getId());
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, m_waterBuffers->GetRefractionDepthTexture());
}

void WaterRenderer::SetShaderUniforms(std::shared_ptr<WaterTile> _waterTile)
{
	m_waterShader->SetUniform("in_MoveFactor", _waterTile->GetUpdatedWaveMoveFactor(m_app.lock()->GetDeltaTime()));
	m_waterShader->SetUniform("in_CameraPos", m_cameraTransform->GetPos());
	m_waterShader->SetUniform("in_DirLightDirection", m_app.lock()->GetSceneManager()->GetCurrentScene()->lightManager->GetDirectionalLight()->direction);
	m_waterShader->SetUniform("in_DirLightColor", m_app.lock()->GetSceneManager()->GetCurrentScene()->lightManager->GetDirectionalLight()->colour);

	m_waterShader->SetUniform("in_Model", _waterTile->GetModelMatrix());
	m_waterShader->SetUniform("in_View", m_app.lock()->GetCamera()->GetViewMatrix());
	m_waterShader->SetUniform("in_Projection", m_app.lock()->GetCamera()->GetProjectionMatrix());
}

void WaterRenderer::RenderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// x z positions // texture Coords
			-1.0f, -1.0f,	 //0.0f, 1.0f,
			-1.0f,  1.0f,	 //0.0f, 0.0f,
			 1.0f, -1.0f,	 //1.0f, 1.0f,
			 1.0f,  1.0f,	 //1.0f, 0.0f,
		};

		// setup plane VAO
		unsigned int quadVBO;
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		//glEnableVertexAttribArray(1);
		//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}