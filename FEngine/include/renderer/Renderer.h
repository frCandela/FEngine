#pragma once

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>
#include <vector>
#include <set>
#include <sstream>

#include "Window.h"
#include "DebugPipeline.h"
#include "ForwardPipeline.h"
#include "Mesh.h"
#include "DebugRender.h"
#include "ImguiManager.h"
#include "Camera.h"


#include "vulkan/SwapChain.h"
#include "vulkan/Instance.h"
#include "vulkan/Shader.h"
#include "vulkan/Texture.h"
#include "vulkan/DepthImage.h"	
#include "vulkan/CommandPool.h"
#include "vulkan/CommandBuffer.h"

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

	inline void DebugLine(glm::vec3 start, glm::vec3 end, glm::vec4 color = glm::vec4(1.f, 0.f, 0.f, 1.f)) { renderDebug->DebugLine(start, end, color); }

private:
	// Setup the command buffers for drawing operations
	void CreateCommandBuffers();

	// Recreates the swap chain (necessary when the window is resized)
	void RecreateSwapChain();

	// Creates the render pass (describes the differents subpasses)
	void CreateRenderPass();

	// Creates the sync objects (fences and semaphores)
	void CreateSyncObjects();

	// Creates test vertices data
	void CreateTestMesh();


	const uint32_t MAX_FRAMES_IN_FLIGHT = 2;
	size_t currentFrame = 0;

	Camera* m_pCamera;
	DebugPipeline* m_pDebugPipeline;
	ForwardPipeline* m_pForwardPipeline;
	Window& m_window;
	ImguiManager * imGui = nullptr;
	RenderDebug * renderDebug = nullptr;

	vk::Texture* texture;
	vk::Sampler* textureSampler;	

	vk::CommandPool* commandPool;
	vk::CommandBuffer* commandBuffers;
	vk::Instance* instance;
	vk::Device* device;
	vk::SwapChain* swapChain;

	VkRenderPass renderPass;

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
