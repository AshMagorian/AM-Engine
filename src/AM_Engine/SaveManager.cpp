#include "AM_Engine.h"
#include "SaveManager.h"
#include <list>
#include <algorithm>
#include <iterator>
#include <windows.h>

void SaveManager::Init(std::weak_ptr<Application> _app)
{
	m_app = _app;
	m_sceneManager = m_app.lock()->GetSceneManager();
}

void SaveManager::SaveScene()
{
	Json::StreamWriterBuilder builder;
	const std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());

	Json::Value json_root;
	Json::Value json_scene;

	// Scene Name
	json_scene["sceneName"] = m_sceneManager->GetCurrentScene()->sceneName;

	// Camera
	json_scene["camera"] = m_sceneManager->GetCurrentScene()->camera->GetName();

	// Entities
	Json::Value json_entities;
	std::list<std::shared_ptr<Entity>> entities = m_sceneManager->GetCurrentScene()->entities;
	int x = 0;
	for (std::list<std::shared_ptr<Entity>>::iterator i = entities.begin(); i != entities.end(); ++i)
	{
		json_entities[x]["entityName"] = (*i)->GetName();

		Json::Value json_components;
		std::list<std::shared_ptr<Component>> components = (*i)->GetComponents();
		int y = 0;
		for (std::list<std::shared_ptr<Component>>::iterator j = components.begin(); j != components.end(); ++j)
		{
			(*j)->OnSave(json_components[y]);
			y++;
		}
		json_entities[x]["components"] = json_components;
		x++;
	}
	json_scene["entities"] = json_entities;

	// Light Manager
	std::shared_ptr<Lights> lightManager = m_sceneManager->GetCurrentScene()->lightManager;
	Json::Value json_lightManager;
	Json::Value json_dirLight;
	Json::Value json_dirLightColour;
	SetVec3(json_dirLightColour, lightManager->GetDirectionalLight()->colour);
	Json::Value json_dirLightDirection;
	SetVec3(json_dirLightDirection, lightManager->GetDirectionalLight()->direction);
	Json::Value json_dirLightIntensity = lightManager->GetDirectionalLight()->intensity;
	json_dirLight["colour"] = json_dirLightColour;
	json_dirLight["direction"] = json_dirLightDirection;
	json_dirLight["intensity"] = json_dirLightIntensity;
	json_lightManager["dirLight"] = json_dirLight;
	json_scene["lightManager"] = json_lightManager;

	// Cubemap
	json_scene["cubeMapName"] = m_sceneManager->GetCurrentScene()->cubemapName;

	json_root["scene"] = json_scene;

	std::ofstream myfile;
	myfile.open("../src/saves/" + m_sceneManager->GetCurrentScene()->sceneName + ".Json");
	writer->write(json_root, &myfile);
	myfile.close();
}

void SaveManager::LoadScene(std::string _sceneName)
{
	std::ifstream ifs;

	ifs.open("../src/saves/" + _sceneName + ".Json");
	if (!ifs.is_open())
	{
		std::cout << "Save file " + _sceneName + ".Json not found" << std::endl;
	}
	else
	{
		Json::Value json_root;
		Json::CharReaderBuilder builder;
		builder["collectComments"] = true;
		JSONCPP_STRING errs;
		if (!parseFromStream(builder, ifs, &json_root, &errs)) {
			std::cout << errs << std::endl;
			return;
		}
		std::shared_ptr<Scene> tmp = std::make_shared<Scene>();
		tmp->self = tmp;
		tmp->app = m_app;
		Json::Value json_scene = json_root["scene"];
		// Scene Name
		m_sceneManager->NameScene(json_scene["sceneName"].asString(), tmp);

		// Light Manager
		tmp->lightManager = std::make_shared<Lights>();
		tmp->lightManager->m_application = m_app;
		Json::Value json_lightManager = json_scene["lightManager"];
		Json::Value json_dirLight = json_lightManager["dirLight"];
		tmp->lightManager->m_directionalLight->colour = GetVec3(json_dirLight["colour"]);
		tmp->lightManager->m_directionalLight->direction = GetVec3(json_dirLight["direction"]);
		tmp->lightManager->m_directionalLight->intensity = json_dirLight["intensity"].asFloat();

		// Entities
		Json::Value json_entities = json_scene["entities"];
		for (size_t i = 0; i < json_entities.size(); i++)
		{
			std::shared_ptr<Entity> entity = tmp->AddEntity(json_entities[i]["entityName"].asString());
			Json::Value& json_components = json_entities[i]["components"];
			for (size_t j = 0; j < json_components.size(); j++)
			{
				if (json_components[j] != NULL)
				{
					std::shared_ptr<Component> component;
					if (json_components[j]["componentType"].asString() != "Transform")
						component = entity->AddComponentPrototype(json_components[j]["componentType"].asString());
					else
						component = entity->GetTransform();
					component->OnLoad(json_components[j]);
				}
			}
		}

		// Camera
		tmp->camera = tmp->GetEntity(json_scene["camera"].asString());

		// Cubemap
		tmp->cubemapName = json_scene["cubeMapName"].asString();

		//Set the created scene
		m_sceneManager->SetCurrentScene(tmp);
	}
}
std::vector<std::string> SaveManager::GetScenes()
{
	std::vector<std::string> v;
	read_directory("../src/saves", v);
	return v;
}

void SaveManager::read_directory(const std::string& name, std::vector<std::string>& v)
{
	std::string pattern(name);
	pattern.append("\\*");
	WIN32_FIND_DATA data;
	HANDLE hFind;
	if ((hFind = FindFirstFile(pattern.c_str(), &data)) != INVALID_HANDLE_VALUE) {
		do {
			std::string in = data.cFileName;
			std::string out;
			for (size_t i = 0; i < in.length(); i++)
			{
				if (in.at(i) == '.')
				{
					if (out.length() > 0)
						v.push_back(out);
					break;
				}
				else
					out += in.at(i);
			}
		} while (FindNextFile(hFind, &data) != 0);
		FindClose(hFind);
	}
}

void SaveManager::SetVec3(Json::Value& _val, glm::vec3 _vec)
{
	_val[0] = _vec.x;
	_val[1] = _vec.y;
	_val[2] = _vec.z;
}

glm::vec3 SaveManager::GetVec3(Json::Value& _val)
{
	glm::vec3 rtn;
	rtn.x = _val[0].asFloat();
	rtn.y = _val[1].asFloat();
	rtn.z = _val[2].asFloat(); 
	return rtn;
}