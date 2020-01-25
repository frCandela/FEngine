#include <iostream>

#define GLFW_INCLUDE_VULKAN

#include "glfw/glfw3.h"  
#include "vulkan/vulkan.h" 
 
int main(int argc, char* argv[]) 
{ 
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	GLFWwindow * window  = glfwCreateWindow(800, 600, "Vulkan", nullptr, nullptr);
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

	}
	glfwDestroyWindow(window);

	glfwTerminate();

	return 0;
}