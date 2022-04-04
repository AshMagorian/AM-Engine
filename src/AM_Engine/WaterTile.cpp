#include "WaterTile.h"
#include <glm/ext.hpp>

WaterTile::WaterTile(float _x, float _z, float _height, float _size)
	: m_xPos(_x), m_zPos(_z), m_height(_height), m_size(_size)
{
	m_reflectionPlane = glm::vec4(0.0f, 1.0f, 0.0f, -m_height - m_offset);
	m_refractionPlane = glm::vec4(0.0f, -1.0f, 0.0f, m_height + m_offset);
}

glm::mat4 WaterTile::GetModelMatrix()
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(m_xPos, m_height, m_zPos));
	model = glm::scale(model, glm::vec3(m_size));
	return model;
}

float WaterTile::GetUpdatedWaveMoveFactor(const float& _deltaTime)
{
	m_moveFactor += m_waveSpeed * _deltaTime;
	if (m_moveFactor > 1.0f)
		m_moveFactor -= 1.0f;
	return m_moveFactor;
}

glm::vec4 WaterTile::GetReflectionPlane()
{
	if (m_reflectionPlane.w != -m_height - m_offset)
	{
		m_reflectionPlane.w = -m_height - m_offset;
	}
	return m_reflectionPlane;
}
glm::vec4 WaterTile::GetRefractionPlane()
{
	if (m_refractionPlane.w != m_height + m_offset)
	{
		m_refractionPlane.w = m_height + m_offset;
	}
	return m_refractionPlane;
}