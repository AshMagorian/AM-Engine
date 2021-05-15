#include "AssimpMesh.h"
#include "ShaderProgram.h"
#include "PBR_Material.h"
#include <iostream>

AssimpMesh::AssimpMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<AssimpTexture> textures)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	SetupMesh();
}

void AssimpMesh::SetupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
		&indices[0], GL_STATIC_DRAW);

	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	//Vertex Tangents
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

	glBindVertexArray(0);
}

void AssimpMesh::Draw(std::shared_ptr<ShaderProgram> shader)
{
	glUseProgram(shader->GetId());

	unsigned int diffuseNr = 1;
	unsigned int normalNr = 1;
	unsigned int metallicNr = 1;
	unsigned int roughnessNr = 1;
	unsigned int aoNr = 1;

	unsigned int matBinary = 0;
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
		// retrieve texture number
		std::string number;
		std::string name = textures[i].type;
		if (name == "texture_diffuse")
		{
			if (diffuseNr == 1) { matBinary += 16; }
			number = std::to_string(diffuseNr++);
		}
		else if (name == "texture_normal")
		{
			if (normalNr == 1) { matBinary += 8; }
			number = std::to_string(normalNr++);
		}
		else if (name == "texture_metallic")
		{
			if (metallicNr == 1) { matBinary += 4; }
			number = std::to_string(metallicNr++);
		}
		else if (name == "texture_roughness")
		{
			if (roughnessNr == 1) { matBinary += 2; }
			number = std::to_string(roughnessNr++);
		}
		else if (name == "texture_ao")
		{
			if (aoNr == 1) { matBinary += 1; }
			number = std::to_string(aoNr++);
		}

		glUniform1i(glGetUniformLocation(shader->GetId(), ("in_Material." + name + number).c_str()), i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}
	glActiveTexture(GL_TEXTURE0);

	// Send data telling the shader which texture maps are being used
	glUniform1i(glGetUniformLocation(shader->GetId(), "in_MatValue"), matBinary);
	// The texture coordinate scale for assimp models will always be 1.0
	glUniform1f(glGetUniformLocation(shader->GetId(), "in_TexCoordScale"), 1.0f);
	

	// draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	for (unsigned int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glBindVertexArray(0);
}

void AssimpMesh::DrawOutline(std::shared_ptr<ShaderProgram> shader)
{
	glUseProgram(shader->GetId());
	// draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}