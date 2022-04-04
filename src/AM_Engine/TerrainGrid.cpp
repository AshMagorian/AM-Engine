#include "TerrainGrid.h"
#include "ShaderProgram.h"
#include "TerrainBlock.h"
#include "PBR_Material.h"
#include "MasterRenderer.h"


void TerrainGrid::AddTerrainBlock(int _x, int _z, std::shared_ptr<PBR_Material> _material)
{
	std::shared_ptr<TerrainBlock> tmp = std::make_shared<TerrainBlock>(_x, _z, _material);
	SetNewBlockOrder(_x, _z);
	m_terrainBlocks.push_back(tmp);
	// Update the blocksSurrounding variable for all blocks
	for (std::list<std::shared_ptr<TerrainBlock>>::iterator i = m_terrainBlocks.begin(); i != m_terrainBlocks.end(); ++i)
	{
		(*i)->m_blocksSurrounding = CalcBlocksSurrounding((*i)->m_gridPosX, (*i)->m_gridPosZ);
	}
}

void TerrainGrid::SetNewBlockOrder(int _x, int _z)
{
	// Top left (-2, -2) is 0, bottom right (2, 2) is 24
	int i = (_x + 2) + 5 * (_z + 2);
	m_blockOrder += pow(2, i);
}

void TerrainGrid::SetTerrainBlockMaterial_bg(int _x, int _z, std::shared_ptr<PBR_Material> _material)
{
	for (std::list<std::shared_ptr<TerrainBlock>>::iterator i = m_terrainBlocks.begin(); i != m_terrainBlocks.end(); ++i)
	{
		if ((*i)->m_gridPosX == _x && (*i)->m_gridPosZ == _z)
			(*i)->m_material[0] = _material;
	}
}
void TerrainGrid::SetTerrainBlockMaterial_r(int _x, int _z, std::shared_ptr<PBR_Material> _material)
{
	for (std::list<std::shared_ptr<TerrainBlock>>::iterator i = m_terrainBlocks.begin(); i != m_terrainBlocks.end(); ++i)
	{
		if ((*i)->m_gridPosX == _x && (*i)->m_gridPosZ == _z)
			(*i)->m_material[1] = _material;
	}
}
void TerrainGrid::SetTerrainBlockMaterial_g(int _x, int _z, std::shared_ptr<PBR_Material> _material)
{
	for (std::list<std::shared_ptr<TerrainBlock>>::iterator i = m_terrainBlocks.begin(); i != m_terrainBlocks.end(); ++i)
	{
		if ((*i)->m_gridPosX == _x && (*i)->m_gridPosZ == _z)
			(*i)->m_material[2] = _material;
	}
}
void TerrainGrid::SetTerrainBlockMaterial_b(int _x, int _z, std::shared_ptr<PBR_Material> _material)
{
	for (std::list<std::shared_ptr<TerrainBlock>>::iterator i = m_terrainBlocks.begin(); i != m_terrainBlocks.end(); ++i)
	{
		if ((*i)->m_gridPosX == _x && (*i)->m_gridPosZ == _z)
			(*i)->m_material[3] = _material;
	}
}
void TerrainGrid::SetTerrainBlockBlendMap(int _x, int _z, std::shared_ptr<Texture> _texture)
{
	for (std::list<std::shared_ptr<TerrainBlock>>::iterator i = m_terrainBlocks.begin(); i != m_terrainBlocks.end(); ++i)
	{
		if ((*i)->m_gridPosX == _x && (*i)->m_gridPosZ == _z)
			(*i)->m_blendMap = _texture;
	}
}

void TerrainGrid::Draw(std::shared_ptr<ShaderProgram> _shader)
{
	if (MasterRenderer::IsSimpleRendering() == false)
	{
		// Set parallax settings
		_shader->SetUniform("in_Parallax_Height", m_parallax_Height);
		_shader->SetUniform("in_Parallax_MinLayers", m_parallax_MinLayers);
		_shader->SetUniform("in_Parallax_MaxLayers", m_parallax_MaxLayers);
	}

	for (std::list<std::shared_ptr<TerrainBlock>>::iterator i = m_terrainBlocks.begin(); i != m_terrainBlocks.end(); ++i)
	{
		//Loop through all 4 material slots
		for (int j = 0; j < 4; j++)
		{
			char buffer[64];
			if ((*i)->m_material[j] != nullptr)
			{
				int matBinary = 0;
				//Albedo
				if ((*i)->m_material[j]->GetAlbedo() != nullptr) {
					snprintf(buffer, sizeof(buffer), "in_ColorValue[%i].matTextures.texture_diffuse1", j);
					_shader->SetUniform(buffer, (*i)->m_material[j]->GetAlbedo());
					matBinary += 16;
				}
				else {
					snprintf(buffer, sizeof(buffer), "in_ColorValue[%i].albedo", j);
					_shader->SetUniform(buffer, (*i)->m_material[j]->GetAlbedoValue());
				}
				//Normal
				if ((*i)->m_material[j]->GetNormal() != nullptr) {
					snprintf(buffer, sizeof(buffer), "in_ColorValue[%i].matTextures.texture_normal1", j);
					_shader->SetUniform(buffer, (*i)->m_material[j]->GetNormal());
					matBinary += 8;
				}
				//Metallic
				if ((*i)->m_material[j]->GetMetallic() != nullptr) {
					snprintf(buffer, sizeof(buffer), "in_ColorValue[%i].matTextures.texture_metallic1", j);
					_shader->SetUniform(buffer, (*i)->m_material[j]->GetMetallic());
					matBinary += 4;
				}
				else {
					snprintf(buffer, sizeof(buffer), "in_ColorValue[%i].metallic", j);
					_shader->SetUniform(buffer, (*i)->m_material[j]->GetMetallicValue());
				}
				//Roughness
				if ((*i)->m_material[j]->GetRoughness() != nullptr) {
					snprintf(buffer, sizeof(buffer), "in_ColorValue[%i].matTextures.texture_roughness1", j);
					_shader->SetUniform(buffer, (*i)->m_material[j]->GetRoughness());
					matBinary += 2;
				}
				else {
					snprintf(buffer, sizeof(buffer), "in_ColorValue[%i].roughness", j);
					_shader->SetUniform(buffer, (*i)->m_material[j]->GetRoughnessValue());
				}
				//Ambient Occlusion
				if ((*i)->m_material[j]->GetAO() != nullptr) {
					snprintf(buffer, sizeof(buffer), "in_ColorValue[%i].matTextures.texture_ao1", j);
					_shader->SetUniform(buffer, (*i)->m_material[j]->GetAO());
					matBinary += 1;
				}
				else {
					snprintf(buffer, sizeof(buffer), "in_ColorValue[%i].ao", j);
					_shader->SetUniform(buffer, 1.0f);
				}
				if (MasterRenderer::IsSimpleRendering() == false)
				{
					//Parallax Mapping
					if ((*i)->m_material[j]->GetDisplacement() != nullptr)
					{
						snprintf(buffer, sizeof(buffer), "in_ColorValue[%i].matTextures.texture_displacement1", j);
						_shader->SetUniform(buffer, (*i)->m_material[j]->GetDisplacement());
						snprintf(buffer, sizeof(buffer), "in_ColorValue[%i].parallax_Adjustment", j);
						_shader->SetUniform(buffer, (*i)->m_material[j]->m_parallax_adjustment);
						snprintf(buffer, sizeof(buffer), "in_ColorValue[%i].parallax_heightScaleAdjustment", j);
						_shader->SetUniform(buffer, (*i)->m_material[j]->m_parallax_heightScaleAdjustment);

						matBinary += 32;
					}
				}

				snprintf(buffer, sizeof(buffer), "in_ColorValue[%i].matValue", j);
				_shader->SetUniform(buffer, matBinary);
			}
			else {
				// Set matValue to -1 if there is no material
				snprintf(buffer, sizeof(buffer), "in_ColorValue[%i].matValue", j);
				_shader->SetUniform(buffer, -1);
			}
		}

		if (MasterRenderer::IsSimpleRendering() == false)
		{
			_shader->SetUniform("in_BlocksSurrounding", (*i)->m_blocksSurrounding);
		}
		_shader->SetUniform("blendMap", (*i)->m_blendMap);

		_shader->SetUniform("in_Model", (*i)->m_modelMatrix);
		_shader->SetUniform("in_TexCoordScale", m_texCoordScale);

		glBindVertexArray((*i)->m_model_id);
		_shader->BindTextures();

		glDrawElements(GL_TRIANGLES, (*i)->GetModelVertexCount(), GL_UNSIGNED_INT, 0);

		_shader->UnbindTextures();
		glBindVertexArray(0);
	}
}
/*
	0	|	1	|	2	|	3	|	4
	5	|	6	|	7	|	8	|	9
	10	|	11	|	12	|	13	|	14
	15	|	16	|	17	|	18	|	19
	20	|	21	|	22	|	23	|	24
*/
#include <iostream>
unsigned int TerrainGrid::CalcBlocksSurrounding(int _x, int _z)
{
	int gridWidth = 5;
	// Top left (-2, -2) is 0, bottom right (2, 2) is 24
	int i = (_x + 2) + gridWidth * (_z + 2);
	int top, bottom, left, right;
	if (i > gridWidth - 1)
		top = i - gridWidth;
	else
		top = -1;
	if (i < gridWidth * (gridWidth - 1))
		bottom = i + gridWidth;
	else
		bottom = -1;
	if (i % gridWidth != 0)
		left = i - 1;
	else
		left = -1;
	if (i % gridWidth != gridWidth - 1)
		right = i + 1;
	else
		right = -1;

	unsigned int rtn = 0;
	// up | down | left | right
	if (right != -1 && m_blockOrder & int(pow(2, right)))
		rtn += 1;
	if (left != -1 && m_blockOrder & int(pow(2, left)))
		rtn += 2;
	if (bottom != -1 && m_blockOrder & int(pow(2, bottom)))
		rtn += 4;
	if (top != -1 && m_blockOrder & int(pow(2, top)))
		rtn += 8;
	return rtn;
}