#include "fanGlobalIncludes.h"

#include "core/input/fanKeyboard.h"
#include "core/input/fanInput.h"


namespace fan {

	//================================================================================================================================
	//================================================================================================================================
	int	Keyboard::IsKeyDown(int _GLFW_KEY) { return glfwGetKey(Input::GetWindow(), _GLFW_KEY) == GLFW_PRESS; }
	
	//================================================================================================================================
	//================================================================================================================================
	bool Keyboard::IsKeyPressed(int _GLFW_KEY) { return Get().m_keysPressed[_GLFW_KEY] == Input::GetFrameCount(); }
	
	//================================================================================================================================
	//================================================================================================================================
	bool Keyboard::IsKeyReleased(int _GLFW_KEY) { return Get().m_keysReleased[_GLFW_KEY] == Input::GetFrameCount(); }
	
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
			Get().m_keysPressed[_key] = Input::GetFrameCount();
		else if (_action == GLFW_RELEASE)
			Get().m_keysReleased[_key] = Input::GetFrameCount();
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