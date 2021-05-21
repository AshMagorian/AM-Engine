#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <algorithm>
#include <iostream>

struct TempVertex
{
	glm::vec3 vector;
	glm::vec2 uv;
};

class PrimitiveShape
{
private:
	PrimitiveShape() {}
	GLuint sphere_id = 0;
	GLuint plane_id = 0;
	GLuint cube_id = 0;

	static PrimitiveShape& Get()
	{
		static PrimitiveShape instance;
		return instance;
	}

	inline GLuint SetupSphere_Impl(int& indexCount);
	inline GLuint SetupPlane_Impl(int& indexCount);
	inline GLuint SetupCube_Impl(int& indexCount);

public:
	static GLuint SetupSphere(int& indexCount) { return Get().SetupSphere_Impl(indexCount); }
	static GLuint SetupPlane(int& indexCount) { return Get().SetupPlane_Impl(indexCount); }
	static GLuint SetupCube(int& indexCount) { return Get().SetupCube_Impl(indexCount); }
	inline static void ComputeTangentBasis(
		//inputs
		TempVertex v1,
		TempVertex v2,
		TempVertex v3,
		// outputs
		glm::vec3& tangent,
		glm::vec3& bitangent
	);
};


void PrimitiveShape::ComputeTangentBasis(
	//inputs
	TempVertex v1,
	TempVertex v2,
	TempVertex v3,
	// outputs
	glm::vec3 &tangent,
	glm::vec3 &bitangent
) 
{
	// Edges of the triangle : position delta
	glm::vec3 deltaPos1 = v2.vector - v1.vector;
	glm::vec3 deltaPos2 = v3.vector - v1.vector;

	// UV delta
	glm::vec2 deltaUV1 = v2.uv - v1.uv;
	glm::vec2 deltaUV2 = v3.uv - v1.uv;

	float denominator = (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
	//if (denominator < 0.0001f && denominator > -0.0001f)
	//	denominator = 0.0001f;

	float r = 1.0f / denominator;
	tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
	bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r; 
}

GLuint PrimitiveShape::SetupSphere_Impl(int& indexCount)
{
	if (sphere_id == 0)
	{
		glGenVertexArrays(1, &sphere_id);

		unsigned int vbo, ebo;
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> uv;
		std::vector<glm::vec3> normals;
		std::vector<unsigned int> indices;

		const unsigned int X_SEGMENTS = 32;
		const unsigned int Y_SEGMENTS = 32;
		const float PI = 3.14159265359f;
		for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
		{
			for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
			{
				float xSegment = (float)x / (float)X_SEGMENTS;
				float ySegment = (float)y / (float)Y_SEGMENTS;
				float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
				float yPos = std::cos(ySegment * PI);
				float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

				positions.push_back(glm::vec3(xPos, yPos, zPos));
				uv.push_back(glm::vec2(xSegment, ySegment));

				normals.push_back(glm::vec3(xPos, yPos, zPos));
			}
		}

		bool oddRow = false;
		for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
		{
			if (!oddRow) // even rows: y == 0, y == 2; and so on
			{
				for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
				{
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
					indices.push_back(y       * (X_SEGMENTS + 1) + x);
				}
			}
			else
			{
				for (int x = X_SEGMENTS; x >= 0; --x)
				{
					indices.push_back(y		  * (X_SEGMENTS + 1) + x);
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
				}
			}
			oddRow = !oddRow;
		}
		indexCount = indices.size();

		// ************************* Calculating Tangents and Bitangents ******************************
		std::vector<glm::vec3> tangents(positions.size(), glm::vec3(0.0f));
		std::vector<glm::vec3> bitangents(positions.size(), glm::vec3(0.0f));
		TempVertex v1, v2, v3;
		glm::vec3 tmpTangent, tmpBitangent;
		int test = 2 * (X_SEGMENTS + 1);
		std::vector<int> duplicatePoints;

		for (int i = 0; i < indexCount - 2; i ++)
		{
			if ((i + 1) % (test) == 0 )
			{
				duplicatePoints.push_back(i);
			}
			else if ((i + 2) % (test) == 0)
			{
				duplicatePoints.push_back(i);
			}
			else
			{
				v1.vector = positions[indices[i]];
				v1.uv = uv[indices[i]];
				v2.vector = positions[indices[i + 1]];
				v2.uv = uv[indices[i + 1]];
				v3.vector = positions[indices[i + 2]];
				v3.uv = uv[indices[i + 2]];

				ComputeTangentBasis(v1, v2, v3, tmpTangent, tmpBitangent);
				tangents[indices[i]] += tmpTangent;
				tangents[indices[i + 1]] += tmpTangent;
				tangents[indices[i + 2]] += tmpTangent;
			}

		}
		//search duplicates
		for (size_t i = 0; i < duplicatePoints.size(); i++)
		{
			int index = duplicatePoints[i];
			int p = indices[index];
			int p2 = indices[index - X_SEGMENTS * 2];
			if (abs(positions[p].x - positions[p2].x) < 0.00001f)
			{
				//Add tangents of duplicate points to each other to average them
				tangents[p] += tangents[p2];
				tangents[p2] = tangents[p];
			}
		
		}

		//****************************************8****************************************************

		std::vector<float> data;
		for (std::size_t i = 0; i < positions.size(); ++i)
		{
			data.push_back(positions[i].x);
			data.push_back(positions[i].y);
			data.push_back(positions[i].z);
			if (uv.size() > 0)
			{
				data.push_back(uv[i].x);
				data.push_back(uv[i].y);
			}
			if (normals.size() > 0)
			{
				data.push_back(normals[i].x);
				data.push_back(normals[i].y);
				data.push_back(normals[i].z);
			}
			if (tangents.size() > 0)
			{
				data.push_back(tangents[i].x);
				data.push_back(tangents[i].y);
				data.push_back(tangents[i].z);
			}
		}

		glBindVertexArray(sphere_id);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
		float stride = (3 + 2 + 3 + 3) * sizeof(float);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (GLsizei)stride, (void*)0);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, (GLsizei)stride, (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, (GLsizei)stride, (void*)(5 * sizeof(float)));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, (GLsizei)stride, (void*)(8 * sizeof(float)));
	}
	return sphere_id;
}

GLuint PrimitiveShape::SetupPlane_Impl(int& indexCount)
{
	if (plane_id == 0)
	{
		// positions
		glm::vec3 pos1(-1.0f, 1.0f, 0.0f);
		glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
		glm::vec3 pos3(1.0f, -1.0f, 0.0f);
		glm::vec3 pos4(1.0f, 1.0f, 0.0f);
		// texture coordinates
		glm::vec2 uv1(0.0f, 1.0f);
		glm::vec2 uv2(0.0f, 0.0f);
		glm::vec2 uv3(1.0f, 0.0f);
		glm::vec2 uv4(1.0f, 1.0f);
		// normal vector
		glm::vec3 nm(0.0f, 0.0f, 1.0f);

		glm::vec3 tangent1, bitangent1;
		glm::vec3 tangent2, bitangent2;

		TempVertex v1, v2, v3;
		// Calculate tangent for triangle of positions 1, 2 and 3
		v1.vector = pos1;
		v1.uv = uv1;
		v2.vector = pos2;
		v2.uv = uv2;
		v3.vector = pos3;
		v3.uv = uv3;
		ComputeTangentBasis(v1, v2, v3, tangent1, bitangent1);
		// Calculate tangent for triangle of positions 1, 3 and 4
		v2.vector = pos3;
		v2.uv = uv3;
		v3.vector = pos4;
		v3.uv = uv4;
		ComputeTangentBasis(v1, v2, v3, tangent2, bitangent2);

		float quadVertices[] = {
			// positions            // normal         // texcoords  // tangent 
			pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z,
			pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z,
			pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z,

			/*pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z,
			pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z,*/
			pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z
		};

		int indices[6] = { 0, 1, 2, 0, 2, 3 };

		// configure plane VAO
		GLuint quadVBO, quadEBO;
		glGenVertexArrays(1, &plane_id);
		glGenBuffers(1, &quadVBO);
		glGenBuffers(1, &quadEBO);
		glBindVertexArray(plane_id);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0); // in_Pos
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(3); // in_Normal
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2); // in_TexCoord
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(4); // in_Tangent
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));


		indexCount = 6;
	}
	return plane_id;
}

GLuint PrimitiveShape::SetupCube_Impl(int& indexCount)
{
	if (cube_id == 0)
	{
		float vertices[] = {
			// pos				  // uv		   // normal		   // tangents

			 0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f, 0.0f, -1.0f,  -1.0f, 0.0f, 0.0f,		//bottom right (relative bottom left)  //back y
			-0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f, 0.0f, -1.0f,  -1.0f, 0.0f, 0.0f,		//bottom left (relative bottom right)
			-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f, 0.0f, -1.0f,  -1.0f, 0.0f, 0.0f,		//top left (relative top right)
			 0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f, 0.0f, -1.0f,  -1.0f, 0.0f, 0.0f,		//top right (relative top left)
											  
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f, 		//bottom left  //front
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f, 		//bottom right
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f, 		//top right
			-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f, 		//top left
											
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,		//left
			-0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  -1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, 	
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, 
								
			 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, -1.0f,		//right y
			 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, -1.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, -1.0f, 
			 0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, -1.0f,
											  
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f, -1.0f, 0.0f,  1.0f, 0.0f, 0.0f, 		//down
			 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f,  1.0f, 0.0f, 0.0f, 
			 0.5f, -0.5f,  0.5f,  1.0f, 1.0f,  0.0f, -1.0f, 0.0f,  1.0f, 0.0f, 0.0f, 
			-0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  0.0f, -1.0f, 0.0f,  1.0f, 0.0f, 0.0f, 

			-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, 0.0f,			//up y
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, 0.0f 		
		};

		int indices[36];
		for (unsigned int i = 0; i < 6; i++)
		{
			indices[6 * i]       = (i * 4) + 0;
			indices[(6 * i) + 1] = (i * 4) + 1;
			indices[(6 * i) + 2] = (i * 4) + 2;
			indices[(6 * i) + 3] = (i * 4) + 0;
			indices[(6 * i) + 4] = (i * 4) + 2;
			indices[(6 * i) + 5] = (i * 4) + 3;
		}

		//std::vector<glm::vec3> tangents(24, glm::vec3(0.0f));
		//glm::vec3 tmpTangent, tmpBitTangent;
		//TempVertex v1, v2, v3;
		//for (unsigned int i = 0; i < 6; i++)
		//{
		//	int indexNo = 0;
		//	indexNo = 11 * indices[i * 6];
		//	v1.vector = { vertices[indexNo + 0],vertices[indexNo + 1] ,vertices[indexNo + 2] };
		//	v1.uv = { vertices[indexNo + 3],vertices[indexNo + 4]};
		//	indexNo = 11 * indices[(i * 6) + 1];
		//	v2.vector = { vertices[indexNo + 0],vertices[indexNo + 1] ,vertices[indexNo + 2] };
		//	v2.uv = { vertices[indexNo + 3],vertices[indexNo + 4] };
		//	indexNo = 11 * indices[(i * 6) + 2];
		//	v3.vector = { vertices[indexNo + 0],vertices[indexNo + 1] ,vertices[indexNo + 2] };
		//	v3.uv = { vertices[indexNo + 3],vertices[indexNo + 4] };
		//	ComputeTangentBasis(v1, v2, v3, tmpTangent, tmpBitTangent);
		//	tangents[4 * i] = tmpTangent;
		//	tangents[(4 * i) + 1] = tmpTangent;
		//	tangents[(4 * i) + 2] = tmpTangent;
		//	tangents[(4 * i) + 3] = tmpTangent;
		//}


		GLuint cubeVBO, cubeEBO;
		glGenVertexArrays(1, &cube_id);
		glGenBuffers(1, &cubeVBO);
		glGenBuffers(1, &cubeEBO);
		glBindVertexArray(cube_id);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);


		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0); // in_Pos
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(2); // in_TexCoord
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(3); // in_Normal
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(5 * sizeof(float)));
		glEnableVertexAttribArray(4); // in_Tangent
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));

		indexCount = 36;
	}
	return cube_id;
}