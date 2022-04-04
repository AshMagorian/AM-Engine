#include "TerrainBlock.h"
#include "TerrainHeightGenerator.h"
#include "PrimitiveShape.h"


TerrainBlock::TerrainBlock(int _x, int _z, std::shared_ptr<PBR_Material> _mat)
{
	m_gridPosX = _x;
	m_gridPosZ = _z;
	// Starting corner (0, 0) for the terrain block is in the far left corner
	x = (_x * m_size) - (m_size / 2);
	z = (_z * m_size) - (m_size / 2);
	m_material[0] = _mat;
	GenerateTerrain();
	CreateModelMatrix();
	
}

void TerrainBlock::GenerateTerrain()
{
	m_vertexCount = 128;
	std::vector<float> vertices;
	std::vector<float> normals;
	std::vector<float> texCoords;
	std::vector<float> tangents;
	std::vector<int> indices;
	std::cout << "starting terrain generation..." << std::endl;
	std::vector<float> heights;
	heights.resize(m_vertexCount * m_vertexCount);
	TerrainHeightGenerator::GenerateHeights(heights, m_vertexCount, m_gridPosX, m_gridPosZ);
	std::cout << "terrain heights created" << std::endl;
	for (int i = 0; i < m_vertexCount; ++i)
	{
		for (int j = 0; j < m_vertexCount; ++j)
		{
			float j_unitInterval = (float)j / ((float)m_vertexCount - 1.0f);
			float i_unitInterval = (float)i / ((float)m_vertexCount - 1.0f);

			vertices.push_back(j_unitInterval * m_size);
			vertices.push_back(heights[(i * m_vertexCount) + j]);
			vertices.push_back(i_unitInterval * m_size);
			glm::vec3 normal = CalculateNormal(j, i);
			normals.push_back(normal.x);
			normals.push_back(normal.y);
			normals.push_back(normal.z);
			texCoords.push_back(j_unitInterval);
			texCoords.push_back(1.0f - i_unitInterval); // UV coordinates start at the bottom left as opposed to the top left

		}
	}
	std::cout << "terrain vertices created" << std::endl;
	tangents.resize(vertices.size());
	std::fill(tangents.begin(), tangents.end(), 0);
	normals.resize(vertices.size());
	std::fill(normals.begin(), normals.end(), 0);
	glm::vec3 tmpTangent, tmpBitTangent, tmpNormal;
	for (int z = 0; z < m_vertexCount - 1; z++)
	{
		for (int x = 0; x < m_vertexCount - 1; x++)
		{
			int topLeft = (z * m_vertexCount) + x;
			int topRight = topLeft + 1;
			int bottomLeft = ((z + 1) * m_vertexCount) + x;
			int bottomRight = bottomLeft + 1;
			indices.push_back(topLeft);
			indices.push_back(bottomLeft);
			indices.push_back(topRight);
			indices.push_back(bottomLeft);
			indices.push_back(bottomRight);
			indices.push_back(topRight);

			TempVertex v1, v2, v3;
			v2.vector = {vertices[topLeft * 3],vertices[(topLeft * 3) + 1],vertices[(topLeft * 3) + 2] };
			v2.uv = { texCoords[topLeft * 2],texCoords[(topLeft * 2) + 1] };
			v1.vector = { vertices[bottomLeft * 3],vertices[(bottomLeft * 3) + 1],vertices[(bottomLeft * 3) + 2] };
			v1.uv = { texCoords[bottomLeft * 2],texCoords[(bottomLeft * 2) + 1] };
			v3.vector = { vertices[topRight * 3],vertices[(topRight * 3) + 1],vertices[(topRight * 3) + 2] };
			v3.uv = { texCoords[topRight * 2],texCoords[(topRight * 2) + 1] };
			PrimitiveShape::ComputeTangentBasis(v1, v2, v3, tmpTangent, tmpBitTangent);
			tangents[topLeft * 3] += tmpTangent.x; 
			tangents[(topLeft * 3) + 1] += tmpTangent.y; 
			tangents[(topLeft * 3) + 2] += tmpTangent.z;
			tangents[bottomLeft * 3] += tmpTangent.x;
			tangents[(bottomLeft * 3) + 1] += tmpTangent.y;
			tangents[(bottomLeft * 3) + 2] += tmpTangent.z;
			tangents[topRight * 3] += tmpTangent.x;
			tangents[(topRight * 3) + 1] += tmpTangent.y;
			tangents[(topRight * 3) + 2] += tmpTangent.z;

			// Tangent X BiTangent = Normal
			tmpNormal = glm::cross(tmpTangent, tmpBitTangent);
			normals[topLeft * 3] += tmpNormal.x;
			normals[(topLeft * 3) + 1] += tmpNormal.y;
			normals[(topLeft * 3) + 2] += tmpNormal.z;
			normals[bottomLeft * 3] += tmpNormal.x;
			normals[(bottomLeft * 3) + 1] += tmpNormal.y;
			normals[(bottomLeft * 3) + 2] += tmpNormal.z;
			normals[topRight * 3] += tmpNormal.x;
			normals[(topRight * 3) + 1] += tmpNormal.y;
			normals[(topRight * 3) + 2] += tmpNormal.z;


			v2.vector = { vertices[bottomLeft * 3],vertices[(bottomLeft * 3) + 1],vertices[(bottomLeft * 3) + 2] };
			v2.uv = { texCoords[bottomLeft * 2],texCoords[(bottomLeft * 2) + 1] };
			v1.vector = { vertices[bottomRight * 3],vertices[(bottomRight * 3) + 1],vertices[(bottomRight * 3) + 2] };
			v1.uv = { texCoords[bottomRight * 2],texCoords[(bottomRight * 2) + 1] };
			v3.vector = { vertices[topRight * 3],vertices[(topRight * 3) + 1],vertices[(topRight * 3) + 2] };
			v3.uv = { texCoords[topRight * 2],texCoords[(topRight * 2) + 1] };
			PrimitiveShape::ComputeTangentBasis(v1, v2, v3, tmpTangent, tmpBitTangent);
			tangents[bottomLeft * 3] += tmpTangent.x;
			tangents[(bottomLeft * 3) + 1] += tmpTangent.y;
			tangents[(bottomLeft * 3) + 2] += tmpTangent.z;
			tangents[bottomRight * 3] += tmpTangent.x;
			tangents[(bottomRight * 3) + 1] += tmpTangent.y;
			tangents[(bottomRight * 3) + 2] += tmpTangent.z;
			tangents[topRight * 3] += tmpTangent.x;
			tangents[(topRight * 3) + 1] += tmpTangent.y;
			tangents[(topRight * 3) + 2] += tmpTangent.z;

			// Tangent X BiTangent = Normal
			tmpNormal = glm::cross(tmpTangent, tmpBitTangent);
			normals[bottomLeft * 3] += tmpNormal.x;
			normals[(bottomLeft * 3) + 1] += tmpNormal.y;
			normals[(bottomLeft * 3) + 2] += tmpNormal.z;
			normals[bottomRight * 3] += tmpNormal.x;
			normals[(bottomRight * 3) + 1] += tmpNormal.y;
			normals[(bottomRight * 3) + 2] += tmpNormal.z;
			normals[topRight * 3] += tmpNormal.x;
			normals[(topRight * 3) + 1] += tmpNormal.y;
			normals[(topRight * 3) + 2] += tmpNormal.z;
		}
	}
	std::cout << "terrain tangents created" << std::endl;

	CreateVAO(vertices, normals, texCoords, tangents, indices);
}

glm::vec3 TerrainBlock::CalculateNormal(int x, int z)
{
	//std::cout << "Calculating normal for: " << x << ", " << z << " (Grid position: " << x + (m_gridPosX * (m_vertexCount - 1)) + (m_vertexCount * 5) << ", " <<  z + (m_gridPosZ * (m_vertexCount - 1)) + (m_vertexCount * 5) << ")" <<  std::endl;

	float heightL = GetHeight(x - 1, z, m_vertexCount, m_gridPosX, m_gridPosZ);
	float heightR = GetHeight(x + 1, z, m_vertexCount, m_gridPosX, m_gridPosZ);
	float heightD = GetHeight(x, z + 1, m_vertexCount, m_gridPosX, m_gridPosZ);
	float heightU = GetHeight(x, z - 1, m_vertexCount, m_gridPosX, m_gridPosZ);
	glm::vec3 normal = glm::vec3(heightL - heightR, 0.6f, heightD - heightU);
	//std::cout << "L: " << heightL << ", R: " << heightR << ", D: " << heightD << ", U: " << heightU << std::endl;

	glm::normalize(normal);
	//std::cout << "Normal: " << normal.x << "," << normal.y << ", " << normal.z << std::endl;
	//std::cout << std::endl;
	return normal;
}

float TerrainBlock::GetHeight(int x, int z, const int& vertexCount, const int& gridX, const int& gridZ)
{
	return TerrainHeightGenerator::GenerateHeight(x, z, vertexCount, gridX, gridZ);
}

void TerrainBlock::CreateVAO(std::vector<float> _vert, std::vector<float> _norm, std::vector<float> _tex, std::vector<float> _tan, std::vector<int> _ind)
{
	glGenVertexArrays(1, &m_model_id);
	glBindVertexArray(m_model_id);

	//create and attach vertex buffer
	GLuint vector_id;
	glGenBuffers(1, &vector_id);
	glBindBuffer(GL_ARRAY_BUFFER, vector_id);
	glBufferData(GL_ARRAY_BUFFER, _vert.size() * sizeof(float), &_vert.at(0), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		3 * sizeof(GLfloat), (void*)0);

	//create and attach normal buffer
	GLuint normal_id;
	glGenBuffers(1, &normal_id);
	glBindBuffer(GL_ARRAY_BUFFER, normal_id);
	glBufferData(GL_ARRAY_BUFFER, _norm.size() * sizeof(float), &_norm.at(0), GL_STATIC_DRAW);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE,
		3 * sizeof(GLfloat), (void*)0);

	//create and attach texCoord buffer
	GLuint tex_id;
	glGenBuffers(1, &tex_id);
	glBindBuffer(GL_ARRAY_BUFFER, tex_id);
	glBufferData(GL_ARRAY_BUFFER, _tex.size() * sizeof(float), &_tex.at(0), GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
		2 * sizeof(GLfloat), (void*)0);

	//create and attach tangent buffer
	GLuint tangent_id;
	glGenBuffers(1, &tangent_id);
	glBindBuffer(GL_ARRAY_BUFFER, tangent_id);
	glBufferData(GL_ARRAY_BUFFER, _tan.size() * sizeof(float), &_tan.at(0), GL_STATIC_DRAW);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE,
		3 * sizeof(GLfloat), (void*)0);

	//create and attach index buffer
	GLuint index_id;
	glGenBuffers(1, &index_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _ind.size() * sizeof(int), &_ind.at(0), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
void TerrainBlock::CreateModelMatrix()
{
	glm::mat4 model(1.0f);
	glm::vec3 position(x, 0.0f, z);
	model = glm::translate(model, position);
	m_modelMatrix = model;
}
int TerrainBlock::GetModelVertexCount()
{
	return (m_vertexCount - 1) * (m_vertexCount - 1) * 6;
}