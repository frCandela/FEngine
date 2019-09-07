#pragma once

#include "core/fanSingleton.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	class Keyboard : public Singleton<Keyboard>
	{
		friend class Input;

	public:
		static int	IsKeyDown(int _GLFW_KEY);
		static bool IsKeyPressed(int _GLFW_KEY);
		static bool IsKeyReleased(int _GLFW_KEY);

		enum AzertyKey {
			D = GLFW_KEY_D,
			Q = GLFW_KEY_A,
			S = GLFW_KEY_S,
			Z = GLFW_KEY_W,
			A = GLFW_KEY_Q,
			E = GLFW_KEY_E
		};

	private:
		static void KeyCallback(GLFWwindow* _window, int _key, int _scancode, int _action, int _mods);
		static void CharCallback(GLFWwindow* _window, unsigned int _c);

		//Events
		std::array< unsigned, 349 > m_keysPressed;
		std::array< unsigned, 349 > m_keysReleased;
	};
}