#pragma once

#include <vector>
#include <string>
#include <list>
#include <memory>

class Entity;

class DebugUIManager
{
	friend class Application;
private:

	bool m_entityWindow = false;
	std::shared_ptr<Entity> m_currentEntity;
	std::shared_ptr<SceneManager> m_sceneManager;
	std::weak_ptr<Application> m_app;

	bool m_newScene = true;
	bool m_resetEntityWindowSize = false;

	void Init(GLFWwindow* _window, std::weak_ptr<Application> _app);
	void NewFrame();
	void Tick(std::list<std::shared_ptr<Entity>> _entities, int _width, int _height);
	void Display();
	void End();
public:

};