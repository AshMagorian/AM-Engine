#include "AM_Engine.h"

std::weak_ptr<Application> Lights::m_application;

Lights::Lights()
{
	m_directionalLight->colour = glm::vec3(1.0f, 1.0f, 1.0f);
	m_directionalLight->direction = glm::vec3(-0.2f, -1.0f, -0.3f);
	m_directionalLight->intensity = 2.0f;
}

void Lights::SetLightingUniforms(std::shared_ptr<ShaderProgram> _shader)
{
	_shader->SetUniform("in_ViewPos", m_application.lock()->GetCamera()->GetCurrentCamera()->GetTransform()->GetPos());

	_shader->SetUniform("in_DirLight.direction", m_directionalLight->direction);
	_shader->SetUniform("in_DirLight.color", m_directionalLight->colour * m_directionalLight->intensity);

	if (std::distance(m_pointLights.begin(), m_pointLights.end()) > 0)
	{
		for (std::list<std::shared_ptr<PointLight>>::iterator j = m_pointLights.begin(); j != m_pointLights.end(); ++j)
		{
			char buffer[64];
			int index = std::distance(m_pointLights.begin(), j);

			snprintf(buffer, sizeof(buffer), "in_PointLights[%i].position", index);
			_shader->SetUniform(buffer, (*j)->m_position);

			snprintf(buffer, sizeof(buffer), "in_PointLights[%i].color", index);
			_shader->SetUniform(buffer, (*j)->m_colour);
		}
	}
	_shader->SetUniform("in_NoPointLights", (int)m_pointLights.size());
}

void Lights::AddPointLight(std::shared_ptr<Entity> _entity)
{
	try
	{
		if ((int)m_pointLights.size() > 49) { throw Exception("Point light not created, Can't have more than 50 point lights"); }
		std::shared_ptr<PointLight> rtn = _entity->GetComponent<PointLight>();
		m_pointLights.push_back(rtn);
	}
	catch (Exception& e)
	{
		std::cout << "AM_Engine Exception: " << e.what() << std::endl;
	}
}

void Lights::AddSpotLight(std::shared_ptr<Entity> _entity)
{
	try
	{
		if ((int)m_spotLights.size() > 49) { throw Exception("Spotlight not created, Can't have more than 50 spotlights"); }
		std::shared_ptr<SpotLight> rtn = _entity->GetComponent<SpotLight>();
		m_spotLights.push_back(rtn);
	}
	catch (Exception& e)
	{
		std::cout << "AM_Engine Exception: " << e.what() << std::endl;
	}
}