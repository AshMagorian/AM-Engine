#pragma once

#include <memory>
#include <list>
#include <string>
#include <typeinfo>

class Application;
class Component;
class Transform;
struct Scene;

class Entity
{
	friend class Application;
	friend class DebugUIManager;
	friend class SceneManager;
	friend struct Scene;
	friend class SaveManager;
	friend class MasterRenderer;
private:
	std::string m_name;
	std::weak_ptr<Application> m_application; ///< A reference to the application
	std::list<std::shared_ptr<Component>> m_components; ///< The collection of components stored in the entity
	std::weak_ptr<Entity> m_self; ///< A reference to itself
	std::weak_ptr<Scene> m_scene; ///< A reference to the scene
	std::weak_ptr<Transform> m_transform; ///< The position, rotation adn scale of the entity

	bool m_deleteComponent = false;
	std::shared_ptr<Component> m_componentToDelete = nullptr;

	std::shared_ptr<Component> AddComponentPrototype(std::string _name);
	void RemoveComponent(std::shared_ptr<Component> _item);
	void RemoveComponent_Impl(std::shared_ptr<Component> _item) { m_components.remove(_item); }
	void OnSceneBegin();
	void Tick();
	void Display();
	void EndOfFrame();
public:
	Entity();
	~Entity();

	std::shared_ptr<Application> GetApplication() { return m_application.lock(); }
	std::shared_ptr<Scene> GetScene() { return m_scene.lock(); }
	


	template <typename T, typename... A>
	std::shared_ptr<T> AddComponent(A... args)
	{
		std::shared_ptr<T> rtn = std::make_shared<T>();
		rtn->m_entity = m_self;
		rtn->m_began = false;
		m_components.push_back(rtn);

		rtn->OnInit(args...);

		return rtn;
	}

	template <typename T>
	std::shared_ptr<T> GetComponent()
	{
		std::shared_ptr<T> rtn;
		try
		{
			for (std::list<std::shared_ptr<Component>>::iterator i = m_components.begin(); i != m_components.end(); ++i)
			{
				rtn = std::dynamic_pointer_cast<T>(*i);
				if (rtn)
				{
					return rtn;
				}
			}
			throw Exception("Componenet " + std::string(typeid(T).name()) + " could not be found");
		}
		catch (Exception& e) { std::cout << "AM_Engine Exception: " << e.what() << std::endl; }
		return nullptr;
	}

	std::list<std::shared_ptr<Component>> GetComponents() { return m_components; }
	std::shared_ptr<Transform> GetTransform() { return m_transform.lock(); }
	std::string GetName() { return m_name; }
	void SetName(std::string _name) { m_name = _name; }
};