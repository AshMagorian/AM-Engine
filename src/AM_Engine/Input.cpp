#include "AM_Engine.h"

std::vector<int> Input::m_keys, Input::m_pressedKeys, Input::m_releasedKeys;
std::vector<int> Input::m_mouseButtons, Input::m_pressedMouseButtons, Input::m_releasedMouseButtons;
float Input::m_deltaMouseX = 0.0f, Input::m_deltaMouseY = 0.0f;
double Input::m_lastX = 0.0, Input::m_lastY = 0.0;
bool Input::m_firstFrame = true, Input::m_FPSMouseEnabled = false;

void Input::KeyCallback(GLFWwindow* _window, int _key, int _scancode, int _action, int _mods)
{
	switch (_action)
	{
	case GLFW_PRESS:
		if (_key == GLFW_KEY_ESCAPE)
			glfwSetWindowShouldClose(_window, GLFW_TRUE);
		if(glfwGetKey(_window, _key))
			//std::cout << "Key pressed: " << static_cast<char>(_key) << std::endl;
		m_pressedKeys.push_back(_key);
		if (std::find(m_keys.begin(), m_keys.end(), _key) != m_keys.end())
		{
			//std::cout << "Ignoring key: " << static_cast<char>(_key) << std::endl;
		}
		else
			m_keys.push_back(_key);
		break;
	case GLFW_RELEASE:
		//std::cout << "Key released: " << static_cast<char>(_key) << std::endl;
		m_releasedKeys.push_back(_key);
		for (unsigned int i = 0; i < m_keys.size(); i++)
		{
			if (m_keys.at(i) == _key)
				m_keys.erase(m_keys.begin() + i);
		}
		break;
	default: break;
	}
}

void Input::MouseCallback(GLFWwindow* _window, double xpos, double ypos)
{
	if (m_FPSMouseEnabled)
	{
		if (m_firstFrame)
		{
			m_lastX = xpos;
			m_lastY = ypos;
			m_firstFrame = false;
		}
		m_deltaMouseX = float(xpos - m_lastX);
		m_deltaMouseY = float(m_lastY - ypos);

		m_lastX = xpos;
		m_lastY = ypos;
	}
}

void Input::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	switch (action)
	{
	case GLFW_PRESS:

		m_pressedMouseButtons.push_back(button);
		m_mouseButtons.push_back(button);
		break;
	case GLFW_RELEASE:
		m_releasedMouseButtons.push_back(button);
		for (unsigned int i = 0; i < m_mouseButtons.size(); i++)
		{
			if (m_mouseButtons.at(i) == button)
				m_mouseButtons.erase(m_mouseButtons.begin() + i);
		}
		break;
	default: break;
	}

}

void Input::EnableFPSMouse(bool _value)
{
	m_FPSMouseEnabled = _value;
	if (_value)
	{
		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 
		m_firstFrame = true;
	}
	else
		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Input::ClearKeys()
{
	m_pressedKeys.clear();
	m_releasedKeys.clear();
}

bool Input::IsKey(int _key)
{
	if (_key >= 97 && _key <= 122)
	{
		int key = _key - 32;
		for (auto& k : m_keys)
			if (k == key)
				return true;
	}
	else
	{
		for (auto& k : m_keys)
			if (k == _key)
				return true;
	}
	return false;
}

bool Input::IsKeyPressed(int _key)
{
	if (_key >= 97 && _key <= 122)
	{
		int key = _key - 32;
		for (auto& k : m_pressedKeys)
			if (k == key)
				return true;
	}
	else
	{
		for (auto& k : m_pressedKeys)
			if (k == _key)
				return true;
	}
	return false;
}

bool Input::IsKeyReleased(int _key)
{
	if (_key >= 97 && _key <= 122)
	{
		int key = _key - 32;
		for (auto& k : m_releasedKeys)
			if (k == key)
				return true;
	}
	else 
	{
		for (auto& k : m_releasedKeys)
			if (k == _key)
				return true;
	}
	return false;
}

bool Input::IsMouseButton(char* button)
{
	int value = -1;
	if (strcmp(button, "left") == 0) { value = GLFW_MOUSE_BUTTON_LEFT; }
	if (strcmp(button, "right") == 0) { value = GLFW_MOUSE_BUTTON_RIGHT; }
	if (strcmp(button, "middle") == 0) { value = GLFW_MOUSE_BUTTON_MIDDLE; }
	if (value == -1)
	{
		std::cout << button << " not recognised" << std::endl;
		return false;
	}
	for (auto& b : m_mouseButtons)
		if (b == value)
			return true;
	return false;
}
bool Input::IsMouseButtonPressed(char* button)
{
	int value = -1;
	if (strcmp(button, "left") == 0) { value = GLFW_MOUSE_BUTTON_LEFT; }
	if (strcmp(button, "right") == 0) { value = GLFW_MOUSE_BUTTON_RIGHT; }
	if (strcmp(button, "middle") == 0) { value = GLFW_MOUSE_BUTTON_MIDDLE; }
	if (value == -1)
	{
		std::cout << button << " not recognised" << std::endl;
		return false;
	}
	for (auto& b : m_pressedMouseButtons)
		if (b == value)
			return true;
	return false;
}
bool Input::IsMouseButtonReleased(char* button)
{
	int value = -1;
	if (strcmp(button, "left") == 0) { value = GLFW_MOUSE_BUTTON_LEFT; }
	if (strcmp(button, "right") == 0) { value = GLFW_MOUSE_BUTTON_RIGHT; }
	if (strcmp(button, "middle") == 0) { value = GLFW_MOUSE_BUTTON_MIDDLE; }
	if (value == -1)
	{
		std::cout << button << " not recognised" << std::endl;
		return false;
	}
	for (auto& b : m_releasedMouseButtons)
		if (b == value)
			return true;
	return false;
}

void Input::ResetValues() 
{ 
	m_deltaMouseX = 0.0f; 
	m_deltaMouseY = 0.0f; 
	m_pressedKeys.clear();
	m_releasedKeys.clear();
	m_pressedMouseButtons.clear();
	m_releasedMouseButtons.clear();
}