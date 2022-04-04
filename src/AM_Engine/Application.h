#pragma once
#include <memory>
#include <list>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define DEBUG_ON 1
#define DEBUG_OFF 0

class Entity;
class Camera;
class Environment;
class Input;
class Resources;
class Skybox;
class SceneManager;
class SaveManager;
class OutlineRenderer;
class DebugUIManager;

class Application
{
	//friend class DebugUIManager;
private:
	bool running;
	std::list<std::shared_ptr<Entity>> entities; ///< A list of all of teh entities stored in the application
	std::weak_ptr<Application> self; ///< A Stores weak pointer to itself

	std::shared_ptr<Camera> m_mainCamera = std::make_shared<Camera>(); // A pointer to the main camera in the game
	std::shared_ptr<Environment> m_time = std::make_shared<Environment>(); // A pointer to the environment object so DeltaTime can be accessed
	std::shared_ptr<Input> m_input = std::make_shared<Input>();
	std::shared_ptr<Resources> m_resourceManager = std::make_shared<Resources>(); // A pointer to the resource manager for accesibility
	std::shared_ptr<Skybox> m_skybox = std::make_shared<Skybox>();
	std::shared_ptr<SceneManager> m_sceneManager = std::make_shared<SceneManager>();
	std::shared_ptr<SaveManager> m_saveManager = std::make_shared<SaveManager>();
	std::shared_ptr<OutlineRenderer> m_outlineRenderer = std::make_shared<OutlineRenderer>();
	std::shared_ptr<DebugUIManager> m_debugUIManager;

	GLFWwindow* m_window = nullptr;

	int m_windowWidth, m_windowHeight;

public:
	Application();
	~Application();
	static std::shared_ptr<Application> const Init(int _w, int _h, int _debugMode);
	void Run();
	void Stop();

	static void error_callback(int error, const char* description);

	std::shared_ptr<Camera> GetCamera() { return m_mainCamera; }
	std::shared_ptr<Resources> GetResourceManager() { return m_resourceManager; }
	std::shared_ptr<Input> GetInput() { return m_input; }
	std::shared_ptr<Skybox> GetSkybox() { return m_skybox; }
	std::shared_ptr<SceneManager> GetSceneManager() { return m_sceneManager; }
	std::shared_ptr<SaveManager> GetSaveManager() { return m_saveManager; }
	std::shared_ptr<OutlineRenderer> GetOutlineRenderer() { return m_outlineRenderer; }
	int GetWindowWidth() { return m_windowWidth; }
	int GetWindowHeight() { return m_windowHeight; }
	float GetDeltaTime();
};