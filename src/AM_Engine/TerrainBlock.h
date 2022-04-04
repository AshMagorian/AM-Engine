#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <vector>
class PBR_Material;
class Texture;

class TerrainBlock
{
	friend class TerrainGrid;
	friend class DebugUIManager;
private:
	const float m_size = 40.0f;
	int m_vertexCount;
	const float m_maxHeight = 5.0f;
	const float m_maxPixelColour = 256.0f;
	float x, z;
	int m_gridPosX, m_gridPosZ;
	glm::mat4 m_modelMatrix;
	GLuint m_model_id;
	std::shared_ptr<PBR_Material> m_material[4];
	// up | down | left | right
	int m_blocksSurrounding;

	std::shared_ptr<Texture> m_blendMap;
	void GenerateTerrain();
	glm::vec3 CalculateNormal(int x, int z);
	float GetHeight(int x, int z, const int& vertexCount, const int& gridX, const int& gridZ);
	void CreateVAO(std::vector<float> _vert, std::vector<float> _norm, std::vector<float> _tex, std::vector<float> _tan, std::vector<int> _ind);
	void CreateModelMatrix();


	int GetModelVertexCount();

public:
	TerrainBlock(int _x, int _z, std::shared_ptr<PBR_Material> _mat);

	glm::mat4 GetModelMatrix() { return m_modelMatrix; }
	GLuint GetVaId() { return m_model_id; }
};