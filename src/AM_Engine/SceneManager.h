#pragma once

#include <string>
#include <vector>
#include <list>
#include <memory>

class Lights;
class Entity;
class TerrainGrid;

struct Scene
{
	std::string sceneName;
	std::shared_ptr<Entity> camera;
	std::list<std::shared_ptr<Entity>> entities;
	std::shared_ptr<Lights> lightManager;
	std::shared_ptr<TerrainGrid> terrain;
	std::string cubemapName;
	std::weak_ptr<Scene> self;
	std::weak_ptr<Application> app;

	std::shared_ptr<Entity> AddEntity(std::string _name);
	/*
	Gives the entity a new name. If the name already exists in the current scene, a number is added to the end of the name
	*/
	void NameEntity(std::string _name, std::shared_ptr<Entity> _entity);
	std::shared_ptr<Entity> GetEntity(std::string _name);
};

class SceneManager
{
	friend class Application;
	friend struct Scene;
	friend class DebugUIManager;
	friend class SaveManager;
private:
	std::vector<std::shared_ptr<Scene>> m_scenes;

	std::shared_ptr<Scene> m_currentScene;
	std::shared_ptr<Scene> m_nextScene;
	bool m_isChangingScene = false;


	void Init(std::weak_ptr<Application>_app);
	void UpdateScene();
	void EndOfFrame();
	void ChangeScene();
	void NameScene(std::string _name, std::shared_ptr<Scene> _scene);

public:

	std::shared_ptr<Scene> CreateScene(std::string _name);
	std::shared_ptr<Scene> GetCurrentScene() { return m_currentScene; }
	std::shared_ptr<Scene> GetScene(std::string _name);
	void SetStartupScene(std::string _name);
	void SetCurrentScene(std::string _name);
	void SetCurrentScene(std::shared_ptr<Scene> _scene);

protected:
	std::weak_ptr<Application> m_app;
};