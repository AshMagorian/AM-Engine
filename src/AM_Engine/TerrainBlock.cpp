#include "TerrainBlock.h"
#include "PrimitiveShape.h"

TerrainBlock::TerrainBlock(int _x, int _z, std::shared_ptr<PBR_Material> _mat)
	: m_material(_mat)
{
	x = _x * m_size;
	z = _z * m_size;
	GenerateTerrain();
	CreateModelMatrix();
}

void TerrainBlock::GenerateTerrain()
{
	m_vertexCount = 256;
	std::vector<float> vertices;
	std::vector<float> normals;
	std::vector<float> texCoords;
	std::vector<float> tangents;
	std::vector<int> indices;

	for (int i = 0; i < m_vertexCount; ++i)
	{
		for (int j = 0; j < m_vertexCount; ++j)
		{
			float j_unitInterval = (float)j / ((float)m_vertexCount - 1.0f);
			float i_unitInterval = (float)i / ((float)m_vertexCount - 1.0f);

			vertices.push_back(j_unitInterval * m_size);
			vertices.push_back(0.0f);
			vertices.push_back(i_unitInterval * -m_size);
			glm::vec3 normal = {0.0f, 1.0f, 0.0f};
			normals.push_back(normal.x);
			normals.push_back(normal.y);
			normals.push_back(normal.z);
			texCoords.push_back(j_unitInterval);
			texCoords.push_back(i_unitInterval);

		}
	}
	tangents.resize(vertices.size());
	std::fill(tangents.begin(), tangents.end(), 0);
	glm::vec3 tmpTangent, tmpBitTangent;
	for (int z = 0; z < m_vertexCount - 1; z++)
	{
		for (int x = 0; x < m_vertexCount - 1; x++)
		{
			int topLeft = (z * m_vertexCount) + x;
			int topRight = topLeft + 1;
			int bottomLeft = ((z + 1) * m_vertexCount) + x;
			int bottomRight = bottomLeft + 1;
			indices.push_back(bottomLeft);
			indices.push_back(topLeft);
			indices.push_back(topRight);
			indices.push_back(bottomLeft);
			indices.push_back(topRight);
			indices.push_back(bottomRight);

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

			v2.vector = { vertices[topRight * 3],vertices[(topRight * 3) + 1],vertices[(topRight * 3) + 2] };
			v2.uv = { texCoords[topRight * 2],texCoords[(topRight * 2) + 1] };
			v1.vector = { vertices[bottomLeft * 3],vertices[(bottomLeft * 3) + 1],vertices[(bottomLeft * 3) + 2] };
			v1.uv = { texCoords[bottomLeft * 2],texCoords[(bottomLeft * 2) + 1] };
			v3.vector = { vertices[bottomRight * 3],vertices[(bottomRight * 3) + 1],vertices[(bottomRight * 3) + 2] };
			v3.uv = { texCoords[bottomRight * 2],texCoords[(bottomRight * 2) + 1] };
			PrimitiveShape::ComputeTangentBasis(v1, v2, v3, tmpTangent, tmpBitTangent);
			tangents[topRight * 3] += tmpTangent.x;
			tangents[(topRight * 3) + 1] += tmpTangent.y;
			tangents[(topRight * 3) + 2] += tmpTangent.z;
			tangents[bottomLeft * 3] += tmpTangent.x;
			tangents[(bottomLeft * 3) + 1] += tmpTangent.y;
			tangents[(bottomLeft * 3) + 2] += tmpTangent.z;
			tangents[bottomRight * 3] += tmpTangent.x;
			tangents[(bottomRight * 3) + 1] += tmpTangent.y;
			tangents[(bottomRight * 3) + 2] += tmpTangent.z;
		}
	}
	CreateVAO(vertices, normals, texCoords, tangents, indices);
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