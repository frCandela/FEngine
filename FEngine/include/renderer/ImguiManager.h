#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <vector>
#include <array>
#include <algorithm>
#include <fstream>

#include "imgui/imgui.h"

#include "vulkan/Device.h"
#include "vulkan/Shader.h"
#include "vulkan/Buffer.hpp"
#include "vulkan/CommandPool.h"
#include "vulkan/Texture.h"
#include "vulkan/Sampler.h"


/// ImguiManager.h class
class ImguiManager
{
public:
	// Initialize styles, keys, Vulkan resources used by the ui, etc.
	ImguiManager(vk::Device* pdevice, vk::CommandPool* pCommandPool, glm::vec2 size, GLFWwindow* window, VkRenderPass renderPass);

	// Release all Vulkan resources required for rendering imGui
	~ImguiManager();	

	// Update vertex and index buffer containing the imGui elements when required
	void UpdateBuffers();

	// Draw current imGui frame into a command buffer
	void DrawFrame(VkCommandBuffer commandBuffer);

private:
	void InitImgui(float width, float height, GLFWwindow* window);

	void CreateFontAndSampler();

	void CreateDescriptors();

	void CreateGraphicsPipeline(VkRenderPass renderPass);

	// Sets the clipboard to the specified string.
	static void SetClipboardText(void* user_data, const char* text);
	
	//Retrieves the contents of the clipboard as a string
	static const char* GetClipboardText(void* user_data);

	// Vulkan resources for rendering the UI	
	vk::Buffer vertexBuffer;
	vk::Buffer indexBuffer;
	int32_t vertexCount = 0;
	int32_t indexCount = 0;

	vk::Sampler* sampler;
	vk::Texture* fontTexture;

	VkPipelineCache pipelineCache;
	VkPipelineLayout pipelineLayout;
	VkPipeline pipeline;
	VkDescriptorPool descriptorPool;
	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorSet descriptorSet;

	vk::Device *device;
	vk::CommandPool* commandPool;

	vk::Shader * fragShader;
	vk::Shader * vertShader;

	// UI params are set via push constants
	struct PushConstBlock
	{
		glm::vec2 scale;
		glm::vec2 translate;
	} pushConstBlock;
};
