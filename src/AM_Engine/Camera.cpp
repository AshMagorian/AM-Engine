#include <AM_Engine/AM_Engine.h>

Camera::Camera()
{
	m_viewAngle = 45.0f;
}

Camera::~Camera()
{

}
/**
*\brief Throws an exception if the camera wasn't initialised. Calculates the new matrix values
*/
void Camera::UpdateMatrix(int _w, int _h)
{
	try
	{
		if (!m_currentCamera)
		{
			throw Exception("Camera not found");
		}

		UpdateCameraVectors();

		m_projectionMatrix = glm::perspective(glm::radians(m_viewAngle),
			(float)_w / (float)_h, 0.1f, 100.f);
		m_viewMatrix = glm::inverse(m_currentCamera->GetTransform()->GetModelMatrix());
	}
	catch (Exception& e)
	{
		std::cout << "myEngine Exception: " << e.what() << std::endl;
	}
}

void Camera::UpdateCameraVectors()
{
	float yaw = m_currentCamera->GetTransform()->GetRotation().y;
	float pitch = m_currentCamera->GetTransform()->GetRotation().x;

	if (m_isFPSCamera)
		UpdateMouseCameraRotation(&yaw, &pitch);

	m_upVector = glm::vec3(0.0f, 1.0f, 0.0f);
	m_forwardVector.x = cos(glm::radians(-yaw + 90.0f)) * cos(glm::radians(-pitch));
	m_forwardVector.y = sin(glm::radians(-pitch));
	m_forwardVector.z = sin(glm::radians(-yaw + 90.0f)) * cos(glm::radians(-pitch));
	m_forwardVector = glm::normalize(m_forwardVector);
	m_rightVector = glm::normalize(glm::cross(m_forwardVector, m_upVector));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	m_upVector = glm::normalize(glm::cross(m_rightVector, m_forwardVector));

	if (m_isWASDMovement)
		UpdateWASDMovement();
}

void Camera::UpdateMouseCameraRotation(float* _yaw, float* _pitch)
{
	float offsetX = m_app.lock()->GetInput()->GetDeltaMouseX() * m_sensitivity;
	float offsetY = m_app.lock()->GetInput()->GetDeltaMouseY() * m_sensitivity;

	*_yaw -= offsetX;
	*_pitch += offsetY;

	if (*_pitch > 89.0f) { *_pitch = 89.0f; }
	if (*_pitch < -89.0f) { *_pitch = -89.0f; }

	m_currentCamera->GetTransform()->SetRotation(glm::vec3(*_pitch, *_yaw, 0.0f));
}

void Camera::UpdateWASDMovement()
{
	glm::vec3 totalMovement = glm::vec3(0.0f, 0.0f, 0.0f);
	bool movedFwd = false;
	bool movedHor = false;
	std::shared_ptr<Input> input = m_app.lock()->GetInput();
	if (input->IsKey('w'))
	{
		movedFwd = !movedFwd;
		totalMovement -= m_forwardVector;
	}
	if (input->IsKey('s'))
	{
		movedFwd = !movedFwd;
		totalMovement += m_forwardVector;
	}
	if (input->IsKey('a')) // move left
	{
		movedHor = !movedHor;
		totalMovement += m_rightVector;
	}
	if (input->IsKey('d')) // move right
	{
		movedHor = !movedHor;
		totalMovement -= m_rightVector;
	}
	if (movedFwd == true || movedHor == true)
	{
		m_currentCamera->GetTransform()->SetPos(m_currentCamera->GetTransform()->GetPos() + (m_movementSpeed * m_app.lock()->GetDeltaTime() * glm::normalize(totalMovement)));
	}
}

void Camera::SetFPSCamera(bool _value)
{
	m_app.lock()->GetInput()->EnableFPSMouse(_value);
	m_isFPSCamera = _value;
}

void Camera::SetWASD(bool _value)
{
	m_isWASDMovement = _value;
}