#include "Transform.h"
#include "Entity.h"
#include <glm/ext.hpp>
MAKE_PROTOTYPE(Transform)

void Transform::OnTick()
{
	if (m_rotation.x > 360.0f || m_rotation.x < -360.0f)
	{
		m_rotation.x = 0.0f;
	}
	if (m_rotation.y > 360.0f || m_rotation.y < -360.0f)
	{
		m_rotation.y = 0.0f;
	}
	if (m_rotation.z > 360.0f || m_rotation.z < -360.0f)
	{
		m_rotation.z = 0.0f;
	}
}

/**
*\brief Uses the position, rotation and scale to create a matrix and then returns it as the model matrix
*/
glm::mat4 Transform::GetModelMatrix()
{

	m_model = glm::mat4(1.0f);

	m_model = glm::translate(m_model, m_position);
	m_model = glm::rotate(m_model, glm::radians(m_rotation.y), glm::vec3(0, 1, 0));
	m_model = glm::rotate(m_model, glm::radians(m_rotation.x), glm::vec3(1, 0, 0));
	m_model = glm::rotate(m_model, glm::radians(m_rotation.z), glm::vec3(0, 0, 1));
	m_model = glm::scale(m_model, m_scale);
	return m_model;
}

glm::mat4 Transform::GetRotationMatrix()
{
	m_model = glm::mat4(1.0f);
	m_model = glm::rotate(m_model, glm::radians(m_rotation.y), glm::vec3(0, 1, 0));
	m_model = glm::rotate(m_model, glm::radians(m_rotation.x), glm::vec3(1, 0, 0));
	m_model = glm::rotate(m_model, glm::radians(m_rotation.z), glm::vec3(0, 0, 1));
	return m_model;
}

glm::mat4 Transform::GetNormalMatrix()
{
	return glm::transpose(glm::inverse(m_model));
}

void Transform::OnShowUI()
{

	ImGui::Text("    x        y         z");
	ImGui::DragFloat3("Position", &m_position.x, 0.005f);
	ImGui::DragFloat3("Rotation", &m_rotation.x, 0.05f);
	ImGui::DragFloat3("Scale", &m_scale.x, 0.005f);
	float scaleIncrement = m_scale.x;
	ImGui::DragFloat("Uniform Scale", &scaleIncrement, 0.005f);
	if (scaleIncrement != m_scale.x)
	{
		scaleIncrement = scaleIncrement - m_scale.x;
		m_scale.x += scaleIncrement;
		m_scale.y += scaleIncrement;
		m_scale.z += scaleIncrement;
	}

}

void Transform::OnSave(Json::Value& val)
{
	SAVE_TYPE(Transform);
	SAVE_VEC3("position", m_position);
	SAVE_VEC3("rotation", m_rotation);
	SAVE_VEC3("scale", m_scale);
}

void Transform::OnLoad(Json::Value& val)
{
	LOAD_VEC3("position", m_position);
	LOAD_VEC3("rotation", m_rotation);
	LOAD_VEC3("scale", m_scale);
}