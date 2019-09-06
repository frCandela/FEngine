#include "fanGlobalIncludes.h"

#include "core/input/fanKeyboard.h"

namespace fan {
	std::array< unsigned, 349 > Keyboard::m_keysPressed;
	std::array< unsigned, 349 > Keyboard::m_keysReleased;

	//================================================================================================================================
	//================================================================================================================================
	void Keyboard::KeyCallback(GLFWwindow* _window, int _key, int _scancode, int _action, int _mods)
	{
		(void)_scancode;
		(void)_window;

		//Imgui
		ImGuiIO& io = ImGui::GetIO();
		if (_action == GLFW_PRESS)
			io.KeysDown[_key] = true;
		if (_action == GLFW_RELEASE)
			io.KeysDown[_key] = false;

		(void)_mods; // Modifiers are not reliable across systems
		io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
		io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
		io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
		io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];

		//Keyboard
		if (_action == GLFW_PRESS)
			m_keysPressed[_key] = Input::GetFrameCount();
		else if (_action == GLFW_RELEASE)
			m_keysReleased[_key] = Input::GetFrameCount();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Keyboard::CharCallback(GLFWwindow* _window, unsigned int _c)
	{
		(void)_window;
		ImGuiIO& io = ImGui::GetIO();
		if (_c > 0 && _c < 0x10000)
			io.AddInputCharacter((unsigned short)_c);
	}
}