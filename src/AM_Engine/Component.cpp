#include "AM_Engine.h"


Component::Component() {}
Component::~Component() {}

std::shared_ptr<Entity> Component::GetEntity()
{
	return m_entity.lock();
}

std::shared_ptr<Application> Component::GetApplication()
{
	return GetEntity()->GetApplication();
}

std::shared_ptr<Component> Component::MakeProduct(std::string type)
{
	std::shared_ptr<Component> proto;

	// Find the prototype in the map
	std::map<std::string, std::shared_ptr<Component>>::const_iterator p;
	p = protoTable().find(type);
	if (p == protoTable().end())
	{
		std::cout << "Prototype not found" << std::endl;
		return nullptr;
	}

	proto = (*p).second;
	//return a clone of the prototype
	return proto->Clone();
}

std::shared_ptr<Component> Component::AddPrototype(std::string type, std::shared_ptr<Component> p)
{
	std::cout << "adding prototype for " << type << std::endl;
	protoTable()[type] = p;
	std::cout << "done\n";
	return p; // handy
}