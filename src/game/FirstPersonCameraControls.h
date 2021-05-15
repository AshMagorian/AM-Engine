#pragma once
#include <AM_Engine/AM_Engine.h>

class FirstPersonCameraControls : public Component
{
	IMPLEMENT_CLONE(FirstPersonCameraControls)
private:
	std::shared_ptr<Transform> m_Transform; ///< The transform component 

	glm::vec3 m_startPos;
	glm::vec3 m_startRot;

	float m_movementSpeed = 3.0f; ///< The speed in which the player moves forward

public:
	void OnBegin();
	void OnSceneBegin();
	void OnTick();
	void OnSave(Json::Value& val);
	void OnLoad(Json::Value& val);
	virtual std::shared_ptr<Component> Clone();
};