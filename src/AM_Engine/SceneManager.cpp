#include <AM_Engine/AM_Engine.h>

void SceneManager::Init(std::weak_ptr<Application>_app)
{
	m_app = _app;
}

std::shared_ptr<Scene> SceneManager::CreateScene(std::string _name)
{
	std::shared_ptr<Scene> scene = std::make_shared<Scene>();
	scene->app = m_app;
	scene->self = scene;
	NameScene(_name, scene);
	scene->camera = scene->AddEntity("MainCamera");
	scene->lightManager = std::make_shared<Lights>();
	scene->lightManager->m_application = m_app;

	m_scenes.push_back(scene);
	return scene;
}

std::shared_ptr<Scene> SceneManager::GetScene(std::string _name)
{
	for (unsigned int i = 0; i < m_scenes.size(); ++i)
	{
		if (_name == m_scenes.at(i)->sceneName)
		{
			return m_scenes.at(i);
		}
	}
	std::cout << "Could not find scene! Scene: " << _name << " not found" << std::endl;
	return nullptr;
}

void SceneManager::SetCurrentScene(std::string _name)
{
	if (_name == m_currentScene->sceneName) { return; }
	for (unsigned int i = 0; i < m_scenes.size(); ++i)
	{
		if (_name == m_scenes.at(i)->sceneName)
		{
			m_nextScene = m_scenes.at(i);
			m_isChangingScene = true;
			return;
		}
	}
	std::cout << "Could not change scene! Scene: " << _name << " not found" << std::endl;
}

void SceneManager::SetCurrentScene(std::shared_ptr<Scene> _scene)
{
	m_nextScene = _scene;
	m_isChangingScene = true;

	for (unsigned int i = 0; i < m_scenes.size(); ++i)
	{
		if (m_currentScene->sceneName == m_scenes.at(i)->sceneName)
		{
			m_scenes.at(i) = _scene;
			return;
		}
	}
	
}

void SceneManager::SetStartupScene(std::string _name)
{
	for (unsigned int i = 0; i < m_scenes.size(); ++i)
	{
		if (_name == m_scenes.at(i)->sceneName)
		{
			m_currentScene = m_scenes.at(i);
			m_app.lock()->GetSkybox()->SetSkybox(m_currentScene->cubemapName);
			m_app.lock()->GetCamera()->SetCurrentCamera(m_currentScene->camera);
			m_nextScene = m_currentScene;
			m_isChangingScene = true;
			return;
		}
	}
	std::cout << "Could not set Startup scene! Scene: " << _name << " not found" << std::endl;
}

void SceneManager::UpdateScene()
{
	for (std::list<std::shared_ptr<Entity>>::iterator i = m_currentScene->entities.begin(); i != m_currentScene->entities.end(); ++i)
	{
		(*i)->Tick();
	}
}

void SceneManager::EndOfFrame()
{
	for (std::list<std::shared_ptr<Entity>>::iterator i = m_currentScene->entities.begin(); i != m_currentScene->entities.end(); ++i)
	{
		(*i)->EndOfFrame();
	}

	if (m_isChangingScene == true)
		ChangeScene();
}

std::shared_ptr<Entity> Scene::AddEntity(std::string _name)
{
	std::shared_ptr<Entity> entity = std::make_shared<Entity>();
	entity->m_self = entity;
	entity->m_scene = self;
	entity->m_application = app;
	entity->m_transform = entity->AddComponent<Transform>();
	NameEntity(_name, entity);
	entities.push_back(entity);
	return entity;
}

void Scene::NameEntity(std::string _name, std::shared_ptr<Entity> _entity)
{
	std::list<std::shared_ptr<Entity>>::iterator i;
	int count = 0;
	std::string name = _name;
	do
	{
		for (i = entities.begin(); i != entities.end(); ++i)
		{
			if ((*i)->m_name == name)
			{
				count++;
				name = _name + "(" + std::to_string(count) + ")";
				break;
			}
		}

	} while (i != entities.end());
	_entity->m_name = name;
}
void SceneManager::NameScene(std::string _name, std::shared_ptr<Scene> _scene)
{
	unsigned int i = 0;
	std::string name = _name;
	do
	{
		for (i = 0; i < m_scenes.size(); ++i)
		{
			if (m_scenes[i]->sceneName == name)
			{
				name = _name + "(" + std::to_string(i + 1) + ")";
				break;
			}
		}

	} while (i < m_scenes.size());
	_scene->sceneName = name;
}

std::shared_ptr<Entity> Scene::GetEntity(std::string _name)
{
	for (std::list<std::shared_ptr<Entity>>::iterator i = entities.begin(); i != entities.end(); ++i)
	{
		if ((*i)->m_name == _name)
			return (*i);
	}
	std::cout << "Entity '" << _name << "' not found" << std::endl;
	return nullptr;
}

void SceneManager::ChangeScene()
{
	m_currentScene = m_nextScene;
	m_app.lock()->GetSkybox()->SetSkybox(m_currentScene->cubemapName);
	m_app.lock()->GetCamera()->SetCurrentCamera(m_currentScene->camera);
	for (std::list<std::shared_ptr<Entity>>::iterator i = m_currentScene->entities.begin(); i != m_currentScene->entities.end(); ++i)
	{
		(*i)->OnSceneBegin();
	}
	m_isChangingScene = false;
}