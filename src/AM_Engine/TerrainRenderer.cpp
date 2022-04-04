#include "TerrainRenderer.h"
#include "Application.h"
#include "SceneManager.h"
#include "TerrainGrid.h"
#include "ShaderProgram.h"
#include "Camera.h"
#include "Entity.h"
#include "Transform.h"
#include "Exception.h"

void TerrainRenderer::Init_Impl(std::weak_ptr<Application> _app) 
{
	m_app = _app;
	try
	{
		m_terrainGeoPassShader = std::make_shared<ShaderProgram>("deferredGeometryTerrain.vert", "deferredGeometryTerrain.frag");
	}
	catch (Exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}
void TerrainRenderer::RenderTerrain_Impl()
{
	glUseProgram(m_terrainGeoPassShader->GetId());
	SetShaderUniforms();
	m_app.lock()->GetSceneManager()->GetCurrentScene()->terrain->Draw(m_terrainGeoPassShader);
}
void TerrainRenderer::SetShaderUniforms()
{
	m_terrainGeoPassShader->SetUniform("in_View", m_app.lock()->GetCamera()->GetViewMatrix());
	m_terrainGeoPassShader->SetUniform("in_Projection", m_app.lock()->GetCamera()->GetProjectionMatrix());
	m_terrainGeoPassShader->SetUniform("in_ViewPos", m_app.lock()->GetCamera()->GetCurrentCamera()->GetTransform()->GetPos());
	m_terrainGeoPassShader->SetUniform("in_clippingPlane", m_clippingPlane);
}