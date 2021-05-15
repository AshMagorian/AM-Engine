#pragma once

#include <list>
#include <memory>

class ShaderProgram;
class Entity;
class Application;

class OutlineRenderer
{
	friend class Renderer;
	friend class Application;
private:
	std::list<std::shared_ptr<Entity>> m_list;
	std::weak_ptr<Application> m_app;
	std::shared_ptr<ShaderProgram> m_outlineShader;
	void Init(std::weak_ptr<Application> _app);
	void AddToList(std::shared_ptr<Entity> _entity);
public:

	void StencilDraw();
	void RenderOutlines();
	std::list<std::shared_ptr<Entity>> GetList() { return m_list; }
};