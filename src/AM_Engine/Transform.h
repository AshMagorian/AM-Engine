#pragma once
#include "Component.h"
#include "glm/glm.hpp"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

/**
*The transform component is included in every entity. About 90% of entities will need to access transform and there
*is negligible computational cost in the remaining 10% of entities accessing it. It stores all of the necessary data
*to create the model matrix
*/
class Transform : public Component
{
	IMPLEMENT_CLONE(Transform)
private:

	glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f); ///< The position of the entity
	glm::vec3 m_rotation = glm::vec3(0.0f, 0.0f, 0.0f);///< The rotation of the entity
	glm::vec3 m_scale = glm::vec3(1.0f, 1.0f, 1.0f); ///< The scale of the entity
public:

	Transform() {}
	~Transform() {}

	void OnTick();
	void OnShowUI();
	void OnSave(Json::Value& val);
	void OnLoad(Json::Value& val);

	glm::vec3 GetPos() { return m_position; }
	glm::vec3 GetRotation() { return m_rotation; }
	glm::vec3 GetScale() { return m_scale; }

	void SetPos(glm::vec3 _pos) { m_position = _pos; }
	void SetRotation(glm::vec3 _rot) { m_rotation = _rot; }
	void SetScale(glm::vec3 _sca) { m_scale = _sca; }

	glm::mat4 GetModelMatrix();
	glm::mat4 GetRotationMatrix();
	glm::mat4 GetNormalMatrix();

protected:
	glm::mat4 m_model; ///< The model matrix
};