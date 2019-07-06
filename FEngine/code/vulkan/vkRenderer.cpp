#include "fanIncludes.h"

#include "vkRenderer.h"

#include "scene/components/fanCamera.h"
#include "util/fanTime.h"
#include "util/fanInput.h"
#include "util/fanInput.h"
#include "vulkan/core/vkInstance.h"
#include "vulkan/core/vkDevice.h"
#include "vulkan/core/vkSwapChain.h"
#include "vulkan/core/vkBuffer.h"
#include "vulkan/core/vkImage.h"
#include "vulkan/core/vkImageView.h"
#include "vulkan/core/vkShader.h"
#include "vulkan/core/vkFrameBuffer.h"
#include "vulkan/pipelines/vkImguiPipeline.h"
#include "vulkan/pipelines/vkPostprocessPipeline.h"
#include "vulkan/pipelines/vkForwardPipeline.h"
#include "vulkan/pipelines/vkDebugPipeline.h"
#include "vulkan/util/vkWindow.h"

namespace vk {

	Renderer * Renderer::ms_globalRenderer = nullptr;

	//================================================================================================================================
	//================================================================================================================================
	Renderer::Renderer(const VkExtent2D _size) :
		m_instance(new Instance())
		, m_window(new Window("Vulkan", _size, m_instance->vkInstance))
		, m_device( * new Device(m_instance, m_window->GetSurface()))
		, m_swapchain(new SwapChain(m_device))
		, m_mainCamera(nullptr)
	{
		ms_globalRenderer = this;
		m_clearColor = glm::vec4(0.5, 0.5, 0.5, 1.f);

		m_swapchain->Create(m_window->GetSurface(), _size);
		Input::Setup(m_window->GetWindow());

		CreateCommandPool();
		CreateRenderPass();
		CreateRenderPassPostprocess();

		m_forwardPipeline = new ForwardPipeline(m_device, m_renderPass);
		m_forwardPipeline->Create( m_swapchain->GetExtent());

		m_postprocessPipeline = new PostprocessPipeline(m_device, m_renderPassPostprocess);
		m_postprocessPipeline->Create(m_swapchain->GetSurfaceFormat().format, m_swapchain->GetExtent());
		
		m_debugPipeline = new DebugPipeline(m_device, m_renderPassPostprocess, m_swapchain->GetSwapchainImagesCount());
		m_debugPipeline->Create(m_swapchain->GetExtent());
		
		m_imguiPipeline = new ImguiPipeline(m_device, m_swapchain->GetSwapchainImagesCount());
		m_imguiPipeline->Create(m_renderPassPostprocess, m_window->GetWindow(), m_swapchain->GetExtent());

		CreateSwapchainFramebuffers();
		CreateForwardFramebuffers();
		CreateCommandBuffers();
		RecordAllCommandBuffers();

		ImGui::NewFrame();
	}
	
	//================================================================================================================================
	//================================================================================================================================	
	Renderer::~Renderer() {
		vkDeviceWaitIdle(m_device.vkDevice);

		ImGui::EndFrame();

		delete m_imguiPipeline;
		delete m_forwardPipeline;
		delete m_debugPipeline;

		DeleteForwardFramebuffers();
		DeleteRenderPass();
		DeleteRenderPassPostprocess();
		DeleteCommandPool();
		DeleteSwapchainFramebuffers();

		delete m_postprocessPipeline;
		delete m_swapchain;
		delete &m_device;
		delete m_window;
		delete m_instance;
	}
	
	//================================================================================================================================
	//================================================================================================================================	
	bool Renderer::WindowIsOpen() { 
		return ! glfwWindowShouldClose(m_window->GetWindow()); 
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Renderer::DrawFrame( ) {
			const VkResult result = m_swapchain->AcquireNextImage();
			if (result == VK_ERROR_OUT_OF_DATE_KHR) {
				if (m_window->GetExtent().width == 0 && m_window->GetExtent().height == 0) {
					return;
				}

				std::cout << "suboptimal swapchain" << std::endl;
				vkDeviceWaitIdle(m_device.vkDevice);

				DeleteForwardFramebuffers();
				DeleteSwapchainFramebuffers();

				m_swapchain->Resize(m_window->GetExtent());
				m_postprocessPipeline->Resize(m_window->GetExtent());
				m_forwardPipeline->Resize(m_window->GetExtent());
				m_debugPipeline->Resize(m_window->GetExtent());

				CreateSwapchainFramebuffers();
				CreateForwardFramebuffers();
				RecordAllCommandBuffers();
				vkResetFences(m_device.vkDevice, 1, m_swapchain->GetCurrentInFlightFence());
				m_swapchain->AcquireNextImage();
			}
			else if (result != VK_SUCCESS) {
				std::cout << "Could not acquire next image" << std::endl;
			}
			else {
				vkWaitForFences(m_device.vkDevice, 1, m_swapchain->GetCurrentInFlightFence(), VK_TRUE, std::numeric_limits<uint64_t>::max());
				vkResetFences(m_device.vkDevice, 1, m_swapchain->GetCurrentInFlightFence());
			}
			
			ImGui::GetIO().DisplaySize = ImVec2(static_cast<float>(m_swapchain->GetExtent().width), static_cast<float>(m_swapchain->GetExtent().height));
			{
				ImGui::Begin("Debug");

				/*std::stringstream ssFramerate;
				ssFramerate << 1.f / updateDelta;
				ImGui::Text(ssFramerate.str().c_str());*/
				//ImGui::Text("ARKANEUUUUH!");
				UpdateUniformBuffer();
				ImGui::End();
			}
			ImGui::EndFrame();
			ImGui::Render();

			m_debugPipeline->DebugLine({ 0,0,0 }, { 1,0,0 }, { 1,0,0,1 });
			m_debugPipeline->DebugLine({ 0,0,0 }, { 0,1,0 }, { 0,1,0,1 });
			m_debugPipeline->DebugLine({ 0,0,0 }, { 0,0,1 }, { 0,0,1,1 });

			RecordCommandBufferDebug(m_swapchain->GetCurrentFrame());
			RecordCommandBufferImgui(m_swapchain->GetCurrentFrame());
			RecordPrimaryCommandBuffer(m_swapchain->GetCurrentFrame());
			SubmitCommandBuffers();

			m_swapchain->PresentImage();
			m_swapchain->StartNextFrame();
			Input::NewFrame();
			ImGui::NewFrame();
			m_debugPipeline->Clear();
	}

	//================================================================================================================================
	//================================================================================================================================
	VkCommandBuffer Renderer::BeginSingleTimeCommands()
	{
		// Allocate a temporary command buffer for memory transfer operations
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = m_commandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(m_device.vkDevice, &allocInfo, &commandBuffer);

		// Start recording the command buffer
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		return commandBuffer;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::EndSingleTimeCommands(VkCommandBuffer commandBuffer)
	{
		vkEndCommandBuffer(commandBuffer);

		// Execute the command buffer to complete the transfer
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(m_device.GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(m_device.GetGraphicsQueue());

		// Cleaning
		vkFreeCommandBuffers(m_device.vkDevice, m_commandPool, 1, &commandBuffer);
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Renderer::ResetCommandPool() {
		VkCommandPoolResetFlags releaseResources = VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT;

		if (vkResetCommandPool(m_device.vkDevice, m_commandPool, releaseResources) != VK_SUCCESS) {
			std::cout << "Could not reset command pool." << std::endl;
			return false;
		}
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::UpdateUniformBuffer()
	{
		// Ui
		static float s_speed = 1.f;

		ImGui::SliderFloat("rotation speed", &s_speed, 0.f, 1000.f);		

		ForwardPipeline::Uniforms ubo = {};
		assert(m_mainCamera != nullptr);
		if ( m_mainCamera->IsModified()) {
			m_mainCamera->SetAspectRatio(static_cast<float>(m_swapchain->GetExtent().width) /m_swapchain->GetExtent().height);
			ubo.view = m_mainCamera->GetView();
			ubo.proj = m_mainCamera->GetProjection();
			ubo.proj[1][1] *= -1;
		}		   


		glm::mat3 model = glm::rotate(glm::mat4(1.f), Time::ElapsedSinceStartup() * glm::radians(s_speed), glm::vec3(0, 1, 0));
		m_forwardPipeline->SetUniforms(ubo, { {model},{glm::mat4(1.0) } });

		DebugPipeline::Uniforms debugUniforms;
		debugUniforms.model = glm::mat4(1.0);
		debugUniforms.view = ubo.view;
		debugUniforms.proj = ubo.proj;
		debugUniforms.color = glm::vec4(1, 1, 1, 1);
		m_debugPipeline->SetUniforms(debugUniforms);
	}
	
	//================================================================================================================================
	//================================================================================================================================
	void Renderer::RecordAllCommandBuffers() {
		for (int cmdBufferIndex = 0; cmdBufferIndex < m_imguiCommandBuffers.size(); cmdBufferIndex++) {
			RecordCommandBufferImgui(cmdBufferIndex);
		}
		for (int cmdBufferIndex = 0; cmdBufferIndex < m_geometryCommandBuffers.size(); cmdBufferIndex++) {
			RecordCommandBufferGeometry(cmdBufferIndex);
		}
		for (int cmdBufferIndex = 0; cmdBufferIndex < m_debugCommandBuffers.size(); cmdBufferIndex++) {
			RecordCommandBufferDebug(cmdBufferIndex);
		}
		for (int cmdBufferIndex = 0; cmdBufferIndex < m_swapchainFramebuffers.size(); cmdBufferIndex++) {
			RecordCommandBufferPostProcess( cmdBufferIndex );
		}

		for (int cmdBufferIndex = 0; cmdBufferIndex < m_primaryCommandBuffers.size(); cmdBufferIndex++) {
			RecordPrimaryCommandBuffer(cmdBufferIndex);
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::RecordPrimaryCommandBuffer(const int _index) {
		VkCommandBuffer commandBuffer = m_primaryCommandBuffers[_index];

		VkCommandBufferBeginInfo commandBufferBeginInfo;
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.pNext = nullptr;
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		commandBufferBeginInfo.pInheritanceInfo = nullptr;

		std::vector<VkClearValue> clearValues(2);
		clearValues[0].color = { m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a };
		clearValues[1].depthStencil = { 1.0f, 0 };

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.pNext = nullptr;
		renderPassInfo.renderPass = m_renderPass;
		renderPassInfo.framebuffer = m_forwardFrameBuffers[_index]->GetFrameBuffer();
		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent.width = m_swapchain->GetExtent().width;
		renderPassInfo.renderArea.extent.height = m_swapchain->GetExtent().height;
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		VkRenderPassBeginInfo renderPassInfoPostprocess = {};
		renderPassInfoPostprocess.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfoPostprocess.pNext = nullptr;
		renderPassInfoPostprocess.renderPass = m_renderPassPostprocess;
		renderPassInfoPostprocess.framebuffer = m_swapchainFramebuffers[_index]->GetFrameBuffer();
		renderPassInfoPostprocess.renderArea.offset = { 0,0 };
		renderPassInfoPostprocess.renderArea.extent.width = m_swapchain->GetExtent().width;
		renderPassInfoPostprocess.renderArea.extent.height = m_swapchain->GetExtent().height;
		renderPassInfoPostprocess.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfoPostprocess.pClearValues = clearValues.data();

		if (vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo) == VK_SUCCESS) {
			
			vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS); {
				vkCmdExecuteCommands(commandBuffer, 1, &m_geometryCommandBuffers[_index]);

			} vkCmdEndRenderPass(commandBuffer);
			
			vkCmdBeginRenderPass(commandBuffer, &renderPassInfoPostprocess, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS); {
				vkCmdExecuteCommands( commandBuffer, 1, &m_postprocessCommandBuffers[_index]	);
				if (m_debugPipeline->HasNothingToDraw() == false) {
					vkCmdExecuteCommands(commandBuffer, 1, &m_debugCommandBuffers[_index]);
				}
				vkCmdExecuteCommands( commandBuffer, 1, &m_imguiCommandBuffers[_index]			);
			} vkCmdEndRenderPass(commandBuffer);
			
			if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
				std::cout << "Could not record command buffer " << _index << "." << std::endl;
			}
		}
		else {
			std::cout << "Could not record command buffer " << _index << "." << std::endl;
		}

	}
	
	//================================================================================================================================
	//================================================================================================================================
	void Renderer::RecordCommandBufferPostProcess( const int _index ) {

		VkCommandBuffer commandBuffer = m_postprocessCommandBuffers[_index];

		VkCommandBufferInheritanceInfo commandBufferInheritanceInfo = {};
		commandBufferInheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
		commandBufferInheritanceInfo.pNext = nullptr;
		commandBufferInheritanceInfo.renderPass = m_renderPassPostprocess;
		commandBufferInheritanceInfo.subpass = 0;
		commandBufferInheritanceInfo.framebuffer = m_swapchainFramebuffers[_index]->GetFrameBuffer();
		commandBufferInheritanceInfo.occlusionQueryEnable = VK_FALSE;
		//commandBufferInheritanceInfo.queryFlags				=;
		//commandBufferInheritanceInfo.pipelineStatistics		=;

		VkCommandBufferBeginInfo commandBufferBeginInfo;
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.pNext = nullptr;
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
		commandBufferBeginInfo.pInheritanceInfo = &commandBufferInheritanceInfo;

		if (vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo) == VK_SUCCESS) {
			m_postprocessPipeline->Draw(commandBuffer);

			if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
				std::cout << "Could not record command buffer " << commandBuffer << "." << std::endl;
			}
		}
		else {
			std::cout << "Could not record command buffer " << commandBuffer << "." << std::endl;
		}

	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::RecordCommandBufferImgui(const int _index) {

		m_imguiPipeline->UpdateBuffer(_index);

		VkCommandBuffer commandBuffer = m_imguiCommandBuffers[_index];

		VkCommandBufferInheritanceInfo commandBufferInheritanceInfo = {};
		commandBufferInheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
		commandBufferInheritanceInfo.pNext = nullptr;
		commandBufferInheritanceInfo.renderPass = m_renderPassPostprocess;
		commandBufferInheritanceInfo.subpass = 0;
		commandBufferInheritanceInfo.framebuffer = m_swapchainFramebuffers[_index]->GetFrameBuffer();
		commandBufferInheritanceInfo.occlusionQueryEnable = VK_FALSE;
		//commandBufferInheritanceInfo.queryFlags				=;
		//commandBufferInheritanceInfo.pipelineStatistics		=;

		VkCommandBufferBeginInfo commandBufferBeginInfo;
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.pNext = nullptr;
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
		commandBufferBeginInfo.pInheritanceInfo = &commandBufferInheritanceInfo;

		if (vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo) == VK_SUCCESS) {
			m_imguiPipeline->DrawFrame(commandBuffer, _index);

			if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
				std::cout << "Could not record command buffer " << _index << "." << std::endl;
			}
		}
		else {
			std::cout << "Could not record command buffer " << _index << "." << std::endl;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::RecordCommandBufferDebug(const int _index) {
		if (m_debugPipeline->HasNothingToDraw() == false) {
			m_debugPipeline->UpdateBuffer(_index);

			VkCommandBuffer commandBuffer = m_debugCommandBuffers[_index];

			VkCommandBufferInheritanceInfo commandBufferInheritanceInfo = {};
			commandBufferInheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
			commandBufferInheritanceInfo.pNext = nullptr;
			commandBufferInheritanceInfo.renderPass = m_renderPassPostprocess;
			commandBufferInheritanceInfo.subpass = 0;
			commandBufferInheritanceInfo.framebuffer = m_swapchainFramebuffers[_index]->GetFrameBuffer();
			commandBufferInheritanceInfo.occlusionQueryEnable = VK_FALSE;
			//commandBufferInheritanceInfo.queryFlags				=;
			//commandBufferInheritanceInfo.pipelineStatistics		=;

			VkCommandBufferBeginInfo commandBufferBeginInfo;
			commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			commandBufferBeginInfo.pNext = nullptr;
			commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
			commandBufferBeginInfo.pInheritanceInfo = &commandBufferInheritanceInfo;

			if (vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo) == VK_SUCCESS) {
				m_debugPipeline->Draw(commandBuffer, _index);
				if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
					std::cout << "Could not record command buffer " << _index << "." << std::endl;
				}
			}
			else {
				std::cout << "Could not record command buffer " << _index << "." << std::endl;
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::RecordCommandBufferGeometry(const int _index) {

		VkCommandBuffer commandBuffer = m_geometryCommandBuffers[_index];

		VkCommandBufferInheritanceInfo commandBufferInheritanceInfo = {};
		commandBufferInheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
		commandBufferInheritanceInfo.pNext = nullptr;
		commandBufferInheritanceInfo.renderPass = m_renderPass;
		commandBufferInheritanceInfo.subpass = 0;
		commandBufferInheritanceInfo.framebuffer = m_forwardFrameBuffers[_index]->GetFrameBuffer();
		commandBufferInheritanceInfo.occlusionQueryEnable = VK_FALSE;
		//commandBufferInheritanceInfo.queryFlags				=;
		//commandBufferInheritanceInfo.pipelineStatistics		=;

		VkCommandBufferBeginInfo commandBufferBeginInfo;
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.pNext = nullptr;
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
		commandBufferBeginInfo.pInheritanceInfo = &commandBufferInheritanceInfo;

		if (vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo) == VK_SUCCESS) {
			m_forwardPipeline->Draw(commandBuffer);
			if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
				std::cout << "Could not record command buffer " << _index << "." << std::endl;
			}
		}
		else {
			std::cout << "Could not record command buffer " << _index << "." << std::endl;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Renderer::SubmitCommandBuffers() {

		std::vector<VkPipelineStageFlags> waitSemaphoreStages = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		std::vector< VkCommandBuffer> commandBuffers = {
			m_primaryCommandBuffers[m_swapchain->GetCurrentImageIndex()]
			//, m_imguiCommandBuffers[m_swapchain->GetCurrentImageIndex()]
		};

		VkSubmitInfo submitInfo;
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext = nullptr;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = m_swapchain->GetCurrentImageAvailableSemaphore();
		submitInfo.pWaitDstStageMask = waitSemaphoreStages.data();
		submitInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
		submitInfo.pCommandBuffers = commandBuffers.data();
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = m_swapchain->GetCurrentRenderFinishedSemaphore();

		VkResult result = vkQueueSubmit(m_device.GetGraphicsQueue(), 1, &submitInfo, *m_swapchain->GetCurrentInFlightFence());
		if (result != VK_SUCCESS) {
			std::cout << "Could not submit draw command buffer " << std::endl;
			return false;
		}

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::ReloadShaders() {
		vkDeviceWaitIdle(m_device.vkDevice);

		m_postprocessPipeline->ReloadShaders();
		m_forwardPipeline->ReloadShaders();
		m_debugPipeline->ReloadShaders();
		m_postprocessPipeline->Resize(m_swapchain->GetExtent());
		m_forwardPipeline->Resize(m_swapchain->GetExtent());
		m_debugPipeline->Resize(m_swapchain->GetExtent());

		DeleteForwardFramebuffers();
		CreateForwardFramebuffers();
		RecordAllCommandBuffers();
	}	

	//================================================================================================================================
	//================================================================================================================================
	bool Renderer::CreateCommandBuffers() {
		VkCommandBufferAllocateInfo commandBufferAllocateInfo;
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.pNext = nullptr;
		commandBufferAllocateInfo.commandPool = m_commandPool;
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocateInfo.commandBufferCount = m_swapchain->GetSwapchainImagesCount();

		m_primaryCommandBuffers.resize(m_swapchain->GetSwapchainImagesCount());

		if (vkAllocateCommandBuffers(m_device.vkDevice, &commandBufferAllocateInfo, m_primaryCommandBuffers.data()) != VK_SUCCESS) {
			std::cout << "Could not allocate command buffers." << std::endl;
			return false;
		}

		VkCommandBufferAllocateInfo secondaryCommandBufferAllocateInfo;
		secondaryCommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		secondaryCommandBufferAllocateInfo.pNext = nullptr;
		secondaryCommandBufferAllocateInfo.commandPool = m_commandPool;
		secondaryCommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
		secondaryCommandBufferAllocateInfo.commandBufferCount = m_swapchain->GetSwapchainImagesCount();

		m_geometryCommandBuffers.resize(m_swapchain->GetSwapchainImagesCount());
		if (vkAllocateCommandBuffers(m_device.vkDevice, &secondaryCommandBufferAllocateInfo, m_geometryCommandBuffers.data()) != VK_SUCCESS) {
			std::cout << "Could not allocate command buffers." << std::endl;
			return false;
		}

		m_debugCommandBuffers.resize(m_swapchain->GetSwapchainImagesCount());
		if (vkAllocateCommandBuffers(m_device.vkDevice, &secondaryCommandBufferAllocateInfo, m_debugCommandBuffers.data()) != VK_SUCCESS) {
			std::cout << "Could not allocate debug command buffers." << std::endl;
			return false;
		}

		m_imguiCommandBuffers.resize(m_swapchain->GetSwapchainImagesCount());
		if (vkAllocateCommandBuffers(m_device.vkDevice, &secondaryCommandBufferAllocateInfo, m_imguiCommandBuffers.data()) != VK_SUCCESS) {
			std::cout << "Could not allocate command buffers." << std::endl;
			return false;
		}

		m_postprocessCommandBuffers.resize(m_swapchain->GetSwapchainImagesCount());
		if (vkAllocateCommandBuffers(m_device.vkDevice, &secondaryCommandBufferAllocateInfo, m_postprocessCommandBuffers.data()) != VK_SUCCESS) {
			std::cout << "Could not allocate command buffers." << std::endl;
			return false;
		}

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Renderer::CreateCommandPool() {
		VkCommandPoolCreateInfo commandPoolCreateInfo;
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.pNext = nullptr;
		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		commandPoolCreateInfo.queueFamilyIndex = m_device.GetGraphicsQueueFamilyIndex();

		if (vkCreateCommandPool(m_device.vkDevice, &commandPoolCreateInfo, nullptr, &m_commandPool) != VK_SUCCESS) {
			std::cout << "Could not allocate command pool." << std::endl;
			return false;
		}
		std::cout << std::hex << "VkCommandPool\t\t" << m_commandPool << std::dec << std::endl;
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Renderer::CreateRenderPass() {
		VkAttachmentDescription colorAttachment;
		colorAttachment.flags = 0;
		colorAttachment.format = m_swapchain->GetSurfaceFormat().format;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkAttachmentDescription depthAttachment;
		depthAttachment.flags = 0;
		depthAttachment.format = m_device.FindDepthFormat();
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef;
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef;
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		std::vector<VkAttachmentReference>   inputAttachments = {};
		std::vector<VkAttachmentReference>   colorAttachments = { colorAttachmentRef };

		VkSubpassDescription subpassDescription;
		subpassDescription.flags = 0;
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.inputAttachmentCount = static_cast<uint32_t>(inputAttachments.size());
		subpassDescription.pInputAttachments = inputAttachments.data();
		subpassDescription.colorAttachmentCount = static_cast<uint32_t>(colorAttachments.size());
		subpassDescription.pColorAttachments = colorAttachments.data();
		subpassDescription.pResolveAttachments = nullptr;
		subpassDescription.pDepthStencilAttachment = &depthAttachmentRef;
		subpassDescription.preserveAttachmentCount = 0;
		subpassDescription.pPreserveAttachments = nullptr;

		VkSubpassDependency dependency;
		dependency.srcSubpass = 0;
		dependency.dstSubpass = VK_SUBPASS_EXTERNAL;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		dependency.dependencyFlags = 0;


		std::vector<VkAttachmentDescription> attachmentsDescriptions = { colorAttachment, depthAttachment };
		std::vector<VkSubpassDescription> subpassDescriptions = { subpassDescription };
		std::vector<VkSubpassDependency> subpassDependencies = { dependency };

		VkRenderPassCreateInfo renderPassCreateInfo;
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.pNext = nullptr;
		renderPassCreateInfo.flags = 0;
		renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachmentsDescriptions.size());
		renderPassCreateInfo.pAttachments = attachmentsDescriptions.data();
		renderPassCreateInfo.subpassCount = static_cast<uint32_t>(subpassDescriptions.size());;
		renderPassCreateInfo.pSubpasses = subpassDescriptions.data();
		renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(subpassDependencies.size());;
		renderPassCreateInfo.pDependencies = subpassDependencies.data();

		if (vkCreateRenderPass(m_device.vkDevice, &renderPassCreateInfo, nullptr, &m_renderPass) != VK_SUCCESS) {
			std::cout << "Could not create render pass;" << std::endl;
			return false;
		}
		std::cout << std::hex << "VkRenderPass\t\t" << m_renderPass << std::dec << std::endl;

		return true;
	}	

	//================================================================================================================================
	//================================================================================================================================
	bool Renderer::CreateRenderPassPostprocess() {
		VkAttachmentDescription colorAttachment;
		colorAttachment.flags = 0;
		colorAttachment.format = m_swapchain->GetSurfaceFormat().format;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef;
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		std::vector<VkAttachmentReference>   inputAttachments = {};
		std::vector<VkAttachmentReference>   colorAttachments = { colorAttachmentRef };

		VkSubpassDescription subpassDescription;
		subpassDescription.flags = 0;
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.inputAttachmentCount = static_cast<uint32_t>(inputAttachments.size());
		subpassDescription.pInputAttachments = inputAttachments.data();
		subpassDescription.colorAttachmentCount = static_cast<uint32_t>(colorAttachments.size());
		subpassDescription.pColorAttachments = colorAttachments.data();
		subpassDescription.pResolveAttachments = nullptr;
		subpassDescription.pDepthStencilAttachment = nullptr;
		subpassDescription.preserveAttachmentCount = 0;
		subpassDescription.pPreserveAttachments = nullptr;

		VkSubpassDependency dependency;
		dependency.srcSubpass = 0;
		dependency.dstSubpass = VK_SUBPASS_EXTERNAL;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		dependency.dependencyFlags = 0;

		std::vector<VkAttachmentDescription> attachmentsDescriptions = { colorAttachment };
		std::vector<VkSubpassDescription> subpassDescriptions = { subpassDescription };
		std::vector<VkSubpassDependency> subpassDependencies = { dependency };

		VkRenderPassCreateInfo renderPassCreateInfo;
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.pNext = nullptr;
		renderPassCreateInfo.flags = 0;
		renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachmentsDescriptions.size());
		renderPassCreateInfo.pAttachments = attachmentsDescriptions.data();
		renderPassCreateInfo.subpassCount = static_cast<uint32_t>(subpassDescriptions.size());;
		renderPassCreateInfo.pSubpasses = subpassDescriptions.data();
		renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(subpassDependencies.size());;
		renderPassCreateInfo.pDependencies = subpassDependencies.data();

		if (vkCreateRenderPass(m_device.vkDevice, &renderPassCreateInfo, nullptr, &m_renderPassPostprocess) != VK_SUCCESS) {
			std::cout << "Could not create render pass pp;" << std::endl;
			return false;
		}
		std::cout << std::hex << "VkRenderPass pp\t\t" << m_renderPassPostprocess << std::dec << std::endl;

		return true;
	}

	//================================================================================================================================
//================================================================================================================================
	bool Renderer::CreateRenderPassUI() {
		VkAttachmentDescription colorAttachment;
		colorAttachment.flags = 0;
		colorAttachment.format = m_swapchain->GetSurfaceFormat().format;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef;
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		std::vector<VkAttachmentReference>   inputAttachments = {};
		std::vector<VkAttachmentReference>   colorAttachments = { colorAttachmentRef };

		VkSubpassDescription subpassDescription;
		subpassDescription.flags = 0;
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.inputAttachmentCount = static_cast<uint32_t>(inputAttachments.size());
		subpassDescription.pInputAttachments = inputAttachments.data();
		subpassDescription.colorAttachmentCount = static_cast<uint32_t>(colorAttachments.size());
		subpassDescription.pColorAttachments = colorAttachments.data();
		subpassDescription.pResolveAttachments = nullptr;
		subpassDescription.pDepthStencilAttachment = nullptr;
		subpassDescription.preserveAttachmentCount = 0;
		subpassDescription.pPreserveAttachments = nullptr;

		VkSubpassDependency dependency;
		dependency.srcSubpass = 0;
		dependency.dstSubpass = VK_SUBPASS_EXTERNAL;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		dependency.dependencyFlags = 0;

		std::vector<VkAttachmentDescription> attachmentsDescriptions = { colorAttachment };
		std::vector<VkSubpassDescription> subpassDescriptions = { subpassDescription };
		std::vector<VkSubpassDependency> subpassDependencies = { dependency };

		VkRenderPassCreateInfo renderPassCreateInfo;
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.pNext = nullptr;
		renderPassCreateInfo.flags = 0;
		renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachmentsDescriptions.size());
		renderPassCreateInfo.pAttachments = attachmentsDescriptions.data();
		renderPassCreateInfo.subpassCount = static_cast<uint32_t>(subpassDescriptions.size());;
		renderPassCreateInfo.pSubpasses = subpassDescriptions.data();
		renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(subpassDependencies.size());;
		renderPassCreateInfo.pDependencies = subpassDependencies.data();

		if (vkCreateRenderPass(m_device.vkDevice, &renderPassCreateInfo, nullptr, &m_renderPassPostprocess) != VK_SUCCESS) {
			std::cout << "Could not create render pass pp;" << std::endl;
			return false;
		}
		std::cout << std::hex << "VkRenderPass pp\t\t" << m_renderPassPostprocess << std::dec << std::endl;

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::CreateForwardFramebuffers() {
		m_forwardFrameBuffers.resize(m_swapchain->GetSwapchainImagesCount());
		for (int framebufferIndex = 0; framebufferIndex < m_forwardFrameBuffers.size(); framebufferIndex++) {
			std::vector<VkImageView> attachments =
			{
				m_postprocessPipeline->GetImageView(),			
				m_forwardPipeline->GetDepthImageView()
			};

			m_forwardFrameBuffers[framebufferIndex] = new FrameBuffer(m_device);
			m_forwardFrameBuffers[framebufferIndex]->Create(m_renderPass, attachments, m_swapchain->GetExtent());
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::CreateSwapchainFramebuffers() {		

		m_swapchainFramebuffers.resize(m_swapchain->GetSwapchainImagesCount());
		for (int framebufferIndex = 0; framebufferIndex < m_swapchainFramebuffers.size(); framebufferIndex++) {
			std::vector<VkImageView> attachments =
			{
				m_swapchain->GetImageView(framebufferIndex),
			};

			m_swapchainFramebuffers[framebufferIndex] = new FrameBuffer(m_device);
			m_swapchainFramebuffers[framebufferIndex]->Create( m_renderPassPostprocess, attachments, m_swapchain->GetExtent());

		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::DeleteSwapchainFramebuffers() {
		for (int framebufferIndex = 0; framebufferIndex < m_swapchainFramebuffers.size(); framebufferIndex++) {
			delete m_swapchainFramebuffers[framebufferIndex];
		}
		m_swapchainFramebuffers.clear();
	}	
	   	
	//================================================================================================================================
	//================================================================================================================================
	void Renderer::DeleteCommandPool() {
		vkDestroyCommandPool(m_device.vkDevice, m_commandPool, nullptr);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::DeleteRenderPass() {
		if (m_renderPass != VK_NULL_HANDLE) {
			vkDestroyRenderPass(m_device.vkDevice, m_renderPass, nullptr);
			m_renderPass = VK_NULL_HANDLE;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::DeleteRenderPassPostprocess() {
		if (m_renderPassPostprocess != VK_NULL_HANDLE) {
			vkDestroyRenderPass(m_device.vkDevice, m_renderPassPostprocess, nullptr);
			m_renderPassPostprocess = VK_NULL_HANDLE;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::DeleteForwardFramebuffers() {
		for (int framebufferIndex = 0; framebufferIndex < m_forwardFrameBuffers.size(); framebufferIndex++) {
			delete m_forwardFrameBuffers[framebufferIndex];
		}
		m_forwardFrameBuffers.clear();
	}
}