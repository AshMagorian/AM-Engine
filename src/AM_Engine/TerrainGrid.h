#pragma once
#include <memory>
#include <list>

class TerrainBlock;
class PBR_Material;
class Texture;
class ShaderProgram;

class TerrainGrid
{
	friend class DebugUIManager;
private:
	std::list<std::shared_ptr<TerrainBlock>> m_terrainBlocks;
	unsigned int m_blockOrder;

	int m_parallax_ClipBorders = true;
	float m_parallax_Height = 0.1f;
	int m_parallax_MinLayers = 8;
	int m_parallax_MaxLayers = 32;
	float m_texCoordScale = 20.0f;

	void SetNewBlockOrder(int _x, int _z);
	unsigned int CalcBlocksSurrounding(int _x, int _z);

public:
	void AddTerrainBlock(int _x, int _z, std::shared_ptr<PBR_Material> _material);

	void SetTerrainBlockMaterial_bg(int _x, int _z, std::shared_ptr<PBR_Material> _material);
	void SetTerrainBlockMaterial_r(int _x, int _z, std::shared_ptr<PBR_Material> _material);
	void SetTerrainBlockMaterial_g(int _x, int _z, std::shared_ptr<PBR_Material> _material);
	void SetTerrainBlockMaterial_b(int _x, int _z, std::shared_ptr<PBR_Material> _material);
	void SetTerrainBlockBlendMap(int _x, int _z, std::shared_ptr<Texture> _texture);

	void Draw(std::shared_ptr<ShaderProgram> _shader);

	std::list<std::shared_ptr<TerrainBlock>> GetTerrain() { return m_terrainBlocks; }
};