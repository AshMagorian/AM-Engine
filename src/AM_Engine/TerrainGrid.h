#pragma once
#include <memory>
#include <list>

class TerrainBlock;
class PBR_Material;
class ShaderProgram;

class TerrainGrid
{
private:
	std::list<std::shared_ptr<TerrainBlock>> m_terrainBlocks;

public:
	void AddTerrainBlock(int _x, int _z, std::shared_ptr<PBR_Material> _material);
	void Draw(std::shared_ptr<ShaderProgram> _shader);

	std::list<std::shared_ptr<TerrainBlock>> GetTerrain() { return m_terrainBlocks; }
};