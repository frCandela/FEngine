#include "Input.h"

#include <iostream>

////////////Input////////////
unsigned Input::m_count = 0;

void Input::Setup(GLFWwindow * window)
{	
	m_window = window;	
	glfwGetWindowSize(window, &(m_windowSize.x), &(m_windowSize.y));

	double x, y;
	glfwGetCursorPos(m_window, &x, &y);

	glfwSetFramebufferSizeCallback(m_window, Input::window_size_callback);
	glfwSetCursorPosCallback(m_window, Mouse::mouse_callback);
	
	glfwSetMouseButtonCallback(m_window, Mouse::mouse_button_callback);
	glfwSetScrollCallback(window, Mouse::scroll_callback);
	glfwSetKeyCallback(m_window, Keyboard::key_callback);
	glfwSetCharCallback(window, Keyboard::char_callback);	
}


void Input::window_size_callback(GLFWwindow* window, int width, int height)
{

}

GLFWwindow * Input::GetWindow()	{return m_window;}
glm::ivec2 Input::GetWindowSize(){ return m_windowSize; }

GLFWwindow * Input::m_window = nullptr;
glm::ivec2  Input::m_windowSize;

unsigned Input::FrameCount() { return m_count;  }

bool Input::ShuttingDown()
{
	return glfwWindowShouldClose(m_window);
}

void Input::Update()
{
	++m_count;
	Mouse::Update();
	glfwPollEvents();

	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ImVec2(Mouse::Position().x, Mouse::Position().y);
	io.MouseDown[0] = Mouse::KeyDown(Mouse::left);
	io.MouseDown[1] = Mouse::KeyDown(Mouse::right);
}

////////////Keyboard////////////
std::array< unsigned, 349 > Keyboard::m_keysPressed;
std::array< unsigned, 349 > Keyboard::m_keysReleased;

int Keyboard::KeyDown(int GLFW_KEY){return glfwGetKey(Input::GetWindow(), GLFW_KEY) == GLFW_PRESS;}
bool Keyboard::KeyPressed(int GLFW_KEY) { return m_keysPressed[GLFW_KEY] == Input::FrameCount();}

bool Keyboard::KeyReleased(int GLFW_KEY) { return m_keysReleased[GLFW_KEY] == Input::FrameCount(); }

void Keyboard::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	//Imgui
	ImGuiIO& io = ImGui::GetIO();
	if (action == GLFW_PRESS)
		io.KeysDown[key] = true;
	if (action == GLFW_RELEASE)
		io.KeysDown[key] = false;

	(void)mods; // Modifiers are not reliable across systems
	io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
	io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
	io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
	io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];

	//Keyboard
	if (action == GLFW_PRESS)
		m_keysPressed[key] = Input::FrameCount();
	else if (action == GLFW_RELEASE)
		m_keysReleased[key] = Input::FrameCount();
}
void Keyboard::char_callback(GLFWwindow*, unsigned int c)
{
	ImGuiIO& io = ImGui::GetIO();
	if (c > 0 && c < 0x10000)
		io.AddInputCharacter((unsigned short)c);
}

////////////Mouse////////////
std::array< unsigned, 11 > Mouse::m_buttonsPressed;
std::array< unsigned, 11 > Mouse::m_buttonsReleased;
glm::vec2 Mouse::m_lockPosition;
bool Mouse::m_lockCursor = false;
glm::vec2 Mouse::m_oldPosition;
glm::vec2 Mouse::m_position;
glm::vec2 Mouse::m_delta;
glm::ivec2 Mouse::m_deltaScroll;

glm::vec2 Mouse::Position() { return m_position; }
glm::vec2 Mouse::Delta() { return m_delta; }
glm::ivec2 Mouse::DeltaScroll() { return m_deltaScroll; }

void Mouse::SetCursor(CursorState state)
{
	glfwSetInputMode(Input::GetWindow(), GLFW_CURSOR, state);
}

void Mouse::LockCursor(bool state, glm::vec2 position)
{
	if (m_lockCursor != state)
	{
		m_lockPosition = position;
		m_lockCursor = state;
		glfwSetCursorPos(Input::GetWindow(), m_lockPosition.x, m_lockPosition.y);
	}
}

bool Mouse::KeyDown(int  GLFW_MOUSE_BUTTON)
{
	return glfwGetMouseButton(Input::GetWindow(), GLFW_MOUSE_BUTTON) == GLFW_PRESS;
}

bool Mouse::ButtonPressed(int GLFW_MOUSE_BUTTON) { return m_buttonsPressed[GLFW_MOUSE_BUTTON] == Input::FrameCount(); }
bool Mouse::ButtonReleased(int GLFW_MOUSE_BUTTON) { return m_buttonsReleased[GLFW_MOUSE_BUTTON] == Input::FrameCount(); }

void Mouse::mouse_callback(GLFWwindow* window, double x, double y)
{

}

void Mouse::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	ImGuiIO& io = ImGui::GetIO();
	io.MouseWheelH += (float)xoffset;
	io.MouseWheel += (float)yoffset;

	m_deltaScroll += glm::ivec2(xoffset, yoffset);
}

void Mouse::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS)
		m_buttonsPressed[button] = Input::FrameCount();
	else if (action == GLFW_RELEASE)
		m_buttonsReleased[button] = Input::FrameCount();
}

void Mouse::Update()
{
	//Mouse
	double x, y;
	glfwGetCursorPos( Input::GetWindow(), &x, &y);

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