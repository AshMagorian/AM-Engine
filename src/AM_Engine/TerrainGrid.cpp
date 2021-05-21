#include "TerrainGrid.h"
#include "ShaderProgram.h"
#include "TerrainBlock.h"
#include "PBR_Material.h"

void TerrainGrid::AddTerrainBlock(int _x, int _z, std::shared_ptr<PBR_Material> _material)
{
	std::shared_ptr<TerrainBlock> tmp = std::make_shared<TerrainBlock>(_x, _z, _material);

	m_terrainBlocks.push_back(tmp);
}
void TerrainGrid::Draw(std::shared_ptr<ShaderProgram> _shader)
{
	for (std::list<std::shared_ptr<TerrainBlock>>::iterator i = m_terrainBlocks.begin(); i != m_terrainBlocks.end(); ++i)
	{
		_shader->SetUniform("in_Material.texture_diffuse1", (*i)->m_material->GetAlbedo());
		_shader->SetUniform("in_Material.texture_normal1", (*i)->m_material->GetNormal());
		_shader->SetUniform("in_Material.texture_metallic1", (*i)->m_material->GetMetallic());
		_shader->SetUniform("in_Material.texture_roughness1", (*i)->m_material->GetRoughness());
		_shader->SetUniform("in_Material.texture_ao1", (*i)->m_material->GetAO());
		_shader->SetUniform("in_Material.texture_displacement1", (*i)->m_material->GetDisplacement());
		_shader->SetUniform("in_Model", (*i)->m_modelMatrix);
		_shader->SetUniform("in_TexCoordScale", 10.0f);

		glBindVertexArray((*i)->m_model_id);
		_shader->BindTextures();

		glDrawElements(GL_TRIANGLES, (*i)->GetModelVertexCount(), GL_UNSIGNED_INT, 0);

		_shader->UnbindTextures();
		glBindVertexArray(0);
	}
}