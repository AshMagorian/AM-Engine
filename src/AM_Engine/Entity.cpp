#include "AM_Engine.h"

Entity::Entity()
{

}

Entity::~Entity()
{

}

void Entity::OnSceneBegin()
{
	for (std::list < std::shared_ptr<Component>>::iterator i = m_components.begin(); i != m_components.end(); ++i)
	{
		if ((*i)->m_began == false)
		{
			(*i)->OnBegin();
			(*i)->m_began = true;
		}
		(*i)->OnSceneBegin();
	}
}

void Entity::Tick()
{
	for (std::list < std::shared_ptr<Component>>::iterator i = m_components.begin(); i != m_components.end(); ++i)
	{
		if ((*i)->m_began == false)
		{
			(*i)->OnBegin();
			(*i)->m_began = true;
		}
		(*i)->OnTick();
	}
}
/**
*\brief Runs through each component's display function
*/
void Entity::Display()
{
	for (std::list<std::shared_ptr<Component>>::iterator i = m_components.begin(); i != m_components.end(); ++i)
	{
		(*i)->OnDisplay();
	}
}
/**
*\brief Runs at the end of each frame
*/
void Entity::EndOfFrame()
{
	if (m_deleteComponent)
	{
		RemoveComponent_Impl(m_componentToDelete);
		m_deleteComponent = false;
	}
}

void Entity::RemoveComponent(std::shared_ptr<Component> _item) 
{
	m_deleteComponent = true;
	m_componentToDelete = _item;
}

std::shared_ptr<Component> Entity::AddComponentPrototype(std::string _name)
{
	std::shared_ptr<Component> rtn = Component::MakeProduct(_name);
	rtn->m_entity = m_self;
	rtn->m_began = false;
	m_components.push_back(rtn);
	rtn->OnInit();
	return rtn;
}