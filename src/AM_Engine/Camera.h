#pragma once
#include "Component.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
	/**
	*This camera class stores the vital data of the main camera of the scene. It stores a pointer to a camera defined
	*in the game itself.
	*/
class Camera
{
	friend class Application;
private:
	float m_viewAngle; ///< The viewing angle of the camera
	bool m_isFPSCamera = false;
	bool m_isWASDMovement = false;
	float m_sensitivity = 0.1f;
	float m_movementSpeed = 3.0f;

	glm::mat4 m_viewMatrix; ///< The view matrix of the main camera
	glm::mat4 m_projectionMatrix; ///< The projection matrix of the main camera

	std::shared_ptr<Entity> m_currentCamera; ///< A pointer to the main camera set from the main game

	void UpdateCameraVectors();
	void UpdateMouseCameraRotation(float* _yaw, float* _pitch);
	void UpdateWASDMovement();

	glm::vec3 m_upVector;
	glm::vec3 m_rightVector;
	glm::vec3 m_forwardVector;

	std::weak_ptr<Application> m_app;

public:
	Camera();
	~Camera();
	/**
	*\brief Updates the 2 matrix values
	*/
	void UpdateMatrix(int _w, int _h);
	/**
	*\brief Sets the new view angle
	*/
	void SetViewAngle(float _angle) { m_viewAngle = _angle; }
	/**
	*\brief Sets which camera to render from
	*/
	void SetCurrentCamera(std::shared_ptr<Entity> _cam) { m_currentCamera = _cam; }
	/**
	*\brief Gets the main camera
	*/
	std::shared_ptr<Entity> GetCurrentCamera() { return m_currentCamera; }
	/**
	*\brief Returns the view matrix
	*/
	glm::mat4 GetViewMatrix() { return m_viewMatrix; }
	/**
	*\brief Returns the projection matrix
	*/
	glm::mat4 GetProjectionMatrix() { return m_projectionMatrix; }

	glm::vec3 GetUpVector() { return m_upVector; }
	glm::vec3 GetRightVector() { return m_rightVector; }
	glm::vec3 GetForwardVector() { return m_forwardVector; }

	void SetFPSCamera(bool _value);
	void SetWASD(bool _value);
	void SetRotationSensitivity(float _value) { m_sensitivity = _value; }
	void SetMovementSpeed(float _value) { m_movementSpeed = _value; }
};