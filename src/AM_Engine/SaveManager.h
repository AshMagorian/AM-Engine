#pragma once
#include <json.h>
#include <iostream>
#include <fstream>

class Application;
class SceneManager;

class SaveManager
{
	friend class Application;
private:
	std::weak_ptr<Application> m_app;
	std::shared_ptr<SceneManager> m_sceneManager;

	void Init(std::weak_ptr<Application> _app);

	void read_directory(const std::string& name, std::vector<std::string>& v);
	void SetVec3(Json::Value& _val, glm::vec3 _vec);
	glm::vec3 GetVec3(Json::Value& _val);

public:
	/*
	* Saves the current scene as a Json file
	*/
	void SaveScene();
	/*
	* Replaces the current scene with the scene loaded at src/saves/<_sceneName>.Json
	*/
	void LoadScene(std::string _sceneName);
	/*
	* Return a vactor of all of the file names in the src/saves folder
	*/
	std::vector<std::string> GetScenes();
};