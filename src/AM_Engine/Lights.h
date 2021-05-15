#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <memory>
#include <list>

class PointLight;
class SpotLight;
class ShaderProgram;
class Application;

struct DirectionalLight
{
	glm::vec3 colour;
	glm::vec3 direction;
	float intensity;
};

class Lights
{
	friend class Application;
	friend class SceneManager;
	friend class SaveManager;
private:
	std::shared_ptr<DirectionalLight> m_directionalLight = std::make_shared<DirectionalLight>();
	std::list<std::shared_ptr<PointLight>> m_pointLights;
	std::list<std::shared_ptr<SpotLight>> m_spotLights;

	static std::weak_ptr<Application> m_application;
public:
	Lights();
	std::shared_ptr<DirectionalLight> GetDirectionalLight() { return m_directionalLight; }
	void SetDirLightDirection(glm::vec3 _dir) { m_directionalLight->direction = _dir; }

	void SetLightingUniforms(std::shared_ptr<ShaderProgram> _shader);

	void AddPointLight(std::shared_ptr<Entity> _entity);
	void AddSpotLight(std::shared_ptr<Entity> _entity);

};