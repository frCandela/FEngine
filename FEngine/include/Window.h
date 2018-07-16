#pragma once

#include <cstdint>
#include <string>
#include <iostream>

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#endif // !GLFW_INCLUDE_VULKAN

#include "Input.h"

class Window
{
public:
	Window(uint32_t width, uint32_t height, std::string name);
	virtual ~Window();

	bool WindowOpen() const;
	void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
	VkExtent2D GetExtend2D() const;

private:
	uint32_t m_width;
	uint32_t m_height;
	GLFWwindow * m_window;

};
