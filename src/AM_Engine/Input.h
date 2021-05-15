#pragma once
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Input
{
	friend class Application;
private:
	static std::vector<int> m_keys; ///< Keys which are held down
	static std::vector<int> m_pressedKeys; ///<Keys which have been pressed this frame
	static std::vector<int> m_releasedKeys; ///< Keys which have been released this frame

	static std::vector<int> m_mouseButtons; ///< Mouse buttons which are held down
	static std::vector<int> m_pressedMouseButtons; ///<Mouse buttons which have been pressed this frame
	static std::vector<int> m_releasedMouseButtons; ///< Mouse buttons which have been released this frame

	void static KeyCallback(GLFWwindow* _window, int _key, int _scancode, int _action, int _mods);
	void static MouseCallback(GLFWwindow* _window, double xpos, double ypos);
	void static MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	void static ClearKeys();


	void ResetValues();

	static bool m_FPSMouseEnabled;
	static bool m_firstFrame;
	static float m_deltaMouseX, m_deltaMouseY;
	static double m_lastX, m_lastY;

	GLFWwindow* m_window = nullptr;
public:
	bool IsMouseButton(char* button);
	bool IsMouseButtonPressed(char* button);
	bool IsMouseButtonReleased(char* button);

	bool IsKey(int _key);
	bool IsKeyPressed(int _key);
	bool IsKeyReleased(int _key);

	void EnableFPSMouse(bool _value);

	float GetDeltaMouseX() { return m_deltaMouseX; }
	float GetDeltaMouseY() { return m_deltaMouseY; }

};