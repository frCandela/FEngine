#include "fanIncludes.h"

#include "util/fanInput.h"

//================================================================================================================================
// INPUT
//================================================================================================================================
unsigned Input::m_count = 0;
GLFWwindow * Input::m_window = nullptr;
glm::ivec2  Input::m_windowSize;

//================================================================================================================================
//================================================================================================================================
void Input::Setup(GLFWwindow * _window)
{
	m_window = _window;
	glfwGetWindowSize(_window, &(m_windowSize.x), &(m_windowSize.y));

	double x, y;
	glfwGetCursorPos(m_window, &x, &y);

	glfwSetFramebufferSizeCallback(m_window, Input::WindowSizeCallback);
	glfwSetCursorPosCallback(m_window, Mouse::MouseCallback);

	glfwSetMouseButtonCallback(m_window, Mouse::MouseButtonCallback);
	glfwSetScrollCallback(_window, Mouse::ScrollCallback);
	glfwSetKeyCallback(m_window, Keyboard::KeyCallback);
	glfwSetCharCallback(_window, Keyboard::CharCallback);
}

//================================================================================================================================
//================================================================================================================================
void Input::WindowSizeCallback(GLFWwindow* _window, int _width, int _height)
{
	(void)_window;
	(void)_width;
	(void)_height;
}

//================================================================================================================================
//================================================================================================================================
void Input::NewFrame()
{
	++m_count;
	glfwPollEvents();

	Mouse::Update();

	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ImVec2(Mouse::GetPosition().x, Mouse::GetPosition().y);
	io.MouseDown[0] = Mouse::IsKeyDown(Mouse::button0);
	io.MouseDown[1] = Mouse::IsKeyDown(Mouse::button1);
}

//================================================================================================================================
// KEYBOARD
//================================================================================================================================
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


//================================================================================================================================
// MOUSE
//================================================================================================================================
std::array< unsigned, 11 > Mouse::m_buttonsPressed;
std::array< unsigned, 11 > Mouse::m_buttonsReleased;
glm::vec2 Mouse::m_lockPosition;
bool Mouse::m_lockCursor = false;
glm::vec2 Mouse::m_oldPosition;
glm::vec2 Mouse::m_position;
glm::vec2 Mouse::m_delta;
glm::ivec2 Mouse::m_deltaScroll;

//================================================================================================================================
//================================================================================================================================
glm::vec2 Mouse::GetScreenSpacePosition(glm::vec2 _screenSize)
{
	glm::vec2 ratio = 2.f * Mouse::GetPosition() / _screenSize - glm::vec2(1.f, 1.f);
	ratio.x = std::clamp(ratio.x, -1.f, 1.f);
	ratio.y = std::clamp(ratio.y, -1.f, 1.f);
	return ratio;
}

//================================================================================================================================
//================================================================================================================================
void Mouse::LockCursor(bool _state, glm::vec2 _position)
{
	if (m_lockCursor != _state)
	{
		m_lockPosition = _position;
		m_lockCursor = _state;
		glfwSetCursorPos(Input::GetWindow(), m_lockPosition.x, m_lockPosition.y);
	}
}

//================================================================================================================================
//================================================================================================================================
void Mouse::ScrollCallback(GLFWwindow* _window, double _xoffset, double _yoffset)
{
	(void)_window;

	ImGuiIO& io = ImGui::GetIO();
	io.MouseWheelH += (float)_xoffset;
	io.MouseWheel += (float)_yoffset;

	m_deltaScroll += glm::ivec2(_xoffset, _yoffset);
}

//================================================================================================================================
//================================================================================================================================
void Mouse::MouseButtonCallback(GLFWwindow* _window, int _button, int _action, int _mods)
{
	(void)_mods;
	(void)_window;

	if (_action == GLFW_PRESS)
	{
		m_buttonsPressed[_button] = Input::GetFrameCount();
	}
	else if (_action == GLFW_RELEASE)
	{
		m_buttonsReleased[_button] = Input::GetFrameCount();
	}
}

//================================================================================================================================
//================================================================================================================================
void Mouse::Update()
{
	//Mouse
	double x, y;
	glfwGetCursorPos(Input::GetWindow(), &x, &y);

	if (m_lockCursor)
	{
		GLFWwindow * window = Input::GetWindow();
		glm::ivec2 size = Input::GetWindowSize();
		glfwSetCursorPos(window, m_lockPosition.x, m_lockPosition.y);

		m_position = glm::vec2(x, y);
		m_delta = m_position - m_lockPosition;
	}
	else
	{
		m_oldPosition = m_position;
		m_position = glm::vec2(x, y);
		m_delta = m_position - m_oldPosition;
	}

	m_deltaScroll = glm::ivec2();
}