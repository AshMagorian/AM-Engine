#pragma once
#include <glm/glm.hpp>
class WaterTile
{
private:
	float m_height;
	float m_xPos;
	float m_zPos;
	float m_size;

	float m_moveFactor = 0.0f;
	float m_waveSpeed = 0.03f;
	float m_offset = 0.01f;

	glm::vec4 m_reflectionPlane;
	glm::vec4 m_refractionPlane;

public:
	WaterTile(float _x, float _z, float _height, float _size);
	float GetHeight() { return m_height; }
	float GetX() { return m_xPos; }
	float GetZ() { return m_zPos; }
	float GetSize() { return m_size; }
	glm::mat4 GetModelMatrix();
	float GetUpdatedWaveMoveFactor(const float& _deltaTime);

	glm::vec4 GetReflectionPlane();
	glm::vec4 GetRefractionPlane();

};