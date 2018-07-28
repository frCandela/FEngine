#pragma once

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>
#include <vector>
#include <set>
#include <sstream>

#include "Window.h"

#include "vulkan/Vertex.h"
#include "vulkan/SwapChain.h"
#include "vulkan/Sampler.h"
#include "vulkan/Instance.h"
#include "vulkan/Mesh.h"
#include "vulkan/Shader.h"
#include "vulkan/Texture.h"
#include "vulkan/DepthImage.h"	
#include "vulkan/Descriptors.h"
#include "vulkan/CommandPool.h"
#include "vulkan/CommandBuffer.h"

#include "ImguiManager.h"
#include "Camera.h"

// Main class for rendering objects using Vulkan
class Renderer
{
public:
	Renderer(Window& rWindow, Camera& rCamera);
	~Renderer();

	// Draw a frame
	void DrawFrame();

	// Returns the size of the swapChain extent
	glm::vec2 GetSize() const; 

	// Render Imgui parameters window
	void RenderGUI();

private:

	// Setup the command buffers for drawing operations
	void CreateCommandBuffers();

	// Recreates the swap chain (necessary when the window is resized)
	void RecreateSwapChain();

	// Creates the graphics pipeline
	void CreateGraphicsPipeline1();
	void CreateGraphicsPipeline2();

	// Creates the render pass (describes the differents subpasses)
	void CreateRenderPass();

	// Create the descriptor pool
	void CreateDescriptorPool();
	void CreateDescriptorPool2();

	// Creates the sync objects (fences and semaphores)
	void CreateSyncObjects();

	// Creates meshes for test rendering
	void CreateTestMesh();

	// Clean Vulkan objects 
	void Cleanup();

	const uint32_t MAX_FRAMES_IN_FLIGHT = 2;
	size_t currentFrame = 0;

	Camera* m_pCamera;
	Window& m_window;

	ImguiManager * imGui = nullptr;

	vk::Texture* texture;
	vk::Sampler* textureSampler;

	vk::Descriptors* descriptors;

	vk::CommandPool* commandPool;
	vk::CommandBuffer* commandBuffers;
	vk::Instance* instance;
	vk::Device* device;
	vk::SwapChain* swapChain;
	
	vk::Shader* vertShader;
	vk::Shader* fragShader;

	VkPipeline graphicsPipeline1;
	VkPipelineLayout pipelineLayout1;

	VkPipeline graphicsPipeline2;
	VkPipelineLayout pipelineLayout2;

	VkDescriptorSet descriptorSet2;
	VkDescriptorSetLayout descriptorSetLayout2;
	vk::Buffer* projViewBuffer;
	struct { glm::mat4 projection; glm::mat4 view; } projView;
	VkDescriptorPool descriptorPool2;
	void CreateDescriptors2();

	VkRenderPass renderPass;
	VkDescriptorPool descriptorPool;

	std::vector<vk::Mesh*> buffers;
	std::vector<VkSemaphore> imageAvailableSemaphores;//Specifies that an image has been acquired and is ready for rendering
	std::vector<VkSemaphore> renderFinishedSemaphores;//Specifies that rendering has finished and presentation can happen
	std::vector<VkFence> inFlightFences;

public:

	// Manages the framerate selection
	class Framerate
	{
	public:
		// Returns the framerate delta
		float GetDelta() const { return deltas[index];  }
		
		// Try to match the refreshRate parameters with the default framerate values to set the framerate
		void TrySetRefreshRate(int refreshRate)
		{
			for (int i = 0; i < deltas.size(); ++i)
			{
				if ((int)(1.f / deltas[i]) == refreshRate)
				{
					index = i;
					return;
				}
				index = 1; //Default 60 fps
			}
		}

		// Create an ImGui combo box for selecting the framerate
		void RenderGui(){ ImGui::Combo("Max Framerate", &index, imguiComboText);}

	private:
		int index = 0;
		const std::array<float, 5> deltas =	{1.f/30.f,	1.f/60.f,	1.f/120.f,	1.f/144.f,	0.f};
		const char* imguiComboText = " 30 FPS \0 60 FPS \0 120 FPS \0 144 FPS \0 Unlimited";
	} framerate;
};
