#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <vector>
class PBR_Material;

class TerrainBlock
{
	friend class TerrainGrid;
private:
	const float m_size = 20.0f;
	int m_vertexCount;
	const float m_maxHeight = 5.0f;
	const float m_maxPixelColour = 256.0f;
	float x, z;
	glm::mat4 m_modelMatrix;
	GLuint m_model_id;
	std::shared_ptr<PBR_Material> m_material;

	void GenerateTerrain();
	void CreateVAO(std::vector<float> _vert, std::vector<float> _norm, std::vector<float> _tex, std::vector<float> _tan, std::vector<int> _ind);
	void CreateModelMatrix();


	int GetModelVertexCount();

public:
	TerrainBlock(int _x, int _z, std::shared_ptr<PBR_Material> _mat);

	glm::mat4 GetModelMatrix() { return m_modelMatrix; }
	GLuint GetVaId() { return m_model_id; }

};