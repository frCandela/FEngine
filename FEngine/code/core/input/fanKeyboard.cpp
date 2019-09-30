#include "fanGlobalIncludes.h"

#include "core/input/fanKeyboard.h"
#include "core/input/fanInput.h"


namespace fan {

	//================================================================================================================================
	//================================================================================================================================
	Keyboard::Keyboard() {
		assert( m_keysPressed.size() == m_keysReleased.size() );
		const uint64_t max = std::numeric_limits<uint64_t>::max();
		for ( int buttonIndex = 0; buttonIndex < m_keysPressed.size(); buttonIndex++ ) {
			m_keysPressed[buttonIndex] = max;
			m_keysReleased[buttonIndex] = max;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Keyboard::IsKeyDown( const int _GLFW_KEY) {
		if (ImGui::GetIO().WantCaptureKeyboard ) {
			return false;
		} else {
			return glfwGetKey(Input::GetWindow(), _GLFW_KEY) == GLFW_PRESS;
		}
	}
	
	//================================================================================================================================
	//================================================================================================================================

	bool Keyboard::IsKeyPressed(const int _GLFW_KEY) {
		if ( ImGui::GetIO().WantCaptureKeyboard) {
			return false;
		}
		else {
			return Get().m_keysPressed[_GLFW_KEY] == Input::GetFrameCount();
		}
	}
	
	//================================================================================================================================
	//================================================================================================================================
	bool Keyboard::IsKeyReleased(const int _GLFW_KEY) {
		if ( ImGui::GetIO().WantCaptureKeyboard) {
			return false;
		}
		else {
			return Get().m_keysReleased[_GLFW_KEY] == Input::GetFrameCount();
		}
	}
	
	//================================================================================================================================
	// Modifiers are not reliable across systems
	//================================================================================================================================
	void Keyboard::KeyCallback(GLFWwindow* /*_window*/, int _key, int /*_scancode*/, int _action, int /*_mods*/)
	{
		ImGuiIO& io = ImGui::GetIO();

		// Dirty hack so that pressing KP_ENTER is considered by IMGUI
		if (_key == GLFW_KEY_KP_ENTER) {
			_key = GLFW_KEY_ENTER;
		}

		//Imgui
		if (_action == GLFW_PRESS)
			io.KeysDown[_key] = true;
		if (_action == GLFW_RELEASE)
			io.KeysDown[_key] = false;

		io.KeyCtrl =	io.KeysDown[GLFW_KEY_LEFT_CONTROL] ||	io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
		io.KeyShift =	io.KeysDown[GLFW_KEY_LEFT_SHIFT] ||		io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
		io.KeyAlt =		io.KeysDown[GLFW_KEY_LEFT_ALT] ||		io.KeysDown[GLFW_KEY_RIGHT_ALT];
		io.KeySuper =	io.KeysDown[GLFW_KEY_LEFT_SUPER] ||		io.KeysDown[GLFW_KEY_RIGHT_SUPER];

		if (_action == GLFW_PRESS)
			Get().m_keysPressed[_key] = Input::GetFrameCount();
		else if (_action == GLFW_RELEASE)
			Get().m_keysReleased[_key] = Input::GetFrameCount();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Keyboard::CharCallback(GLFWwindow* /*_window*/, unsigned int _c)
	{
		ImGuiIO& io = ImGui::GetIO();
		if (_c > 0 && _c < 0x10000) {
			io.AddInputCharacter((unsigned short)_c);
		}
	}
}