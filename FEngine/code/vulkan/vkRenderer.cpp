#include "Includes.h"

#include "vkRenderer.h"

#include "util/Time.h"
#include "util/glfwInput.h"
#include "util/glfwInput.h"
#include "vulkan/vkInstance.h"
#include "vulkan/vkDevice.h"
#include "vulkan/vkWindow.h"
#include "vulkan/vkSwapChain.h"
#include "vulkan/vkBuffer.h"
#include "vulkan/vkImage.h"
#include "vulkan/vkImageView.h"
#include "vulkan/vkShader.h"
#include "vulkan/vkVertex.h"
#include "vulkan/vkImguiPipeline.h"
#include "vulkan/vkFrameBuffer.h"
#include "vulkan/vkPostprocessPipeline.h"

namespace vk {

	Renderer * Renderer::ms_globalRenderer = nullptr;

	//================================================================================================================================
	//================================================================================================================================
	Renderer::Renderer(const VkExtent2D _size) :
		m_instance(new Instance())
		, m_window(new Window("Vulkan", _size, m_instance->vkInstance))
		, m_device(new Device(m_instance, m_window->GetSurface()))
		, m_swapchain(new SwapChain(m_device ))
	{
		m_swapchain->Create(m_window->GetSurface(), _size);

		Input::Setup(m_window->GetWindow());

		ms_globalRenderer = this;
		m_clearColor = glm::vec4(0.5, 0.5, 0.5, 1.f);


		CreateCommandPool();
		m_postprocessPipeline = new PostprocessPipeline(m_device);
		m_postprocessPipeline->Create(m_swapchain->GetSurfaceFormat().format, m_swapchain->GetExtent());
		CreateSwapchainFramebuffers();
		CreateShaders();
		
		CreateRenderPass();
		CreateDepthRessources();
		CreateFramebuffers();
		CreateCommandBuffers();
		CreateDescriptors();
		CreatePipeline();
		CreateVertexBuffers();

		m_imguiPipeline = new ImguiPipeline(m_device, m_swapchain->GetSwapchainImagesCount());
		glm::vec2 size = { m_swapchain->GetExtent().width, m_swapchain->GetExtent().height };
		m_imguiPipeline->Create(size, m_window->GetWindow(), m_renderPass);

		RecordAllCommandBuffers();

	}
	
	//================================================================================================================================
	//================================================================================================================================	
	Renderer::~Renderer() {
		vkDeviceWaitIdle(m_device->vkDevice);

		delete m_imguiPipeline;

		DeletePipeline();
		DeleteFramebuffers();
		DeleteDepthRessources();
		DeleteRenderPass();
		DeleteCommandPool();
		DeleteDescriptors();
		DeleteSwapchainFramebuffers();

		delete m_postprocessPipeline;
		delete m_indexBuffer;
		delete m_vertexBuffer;
		delete m_fragmentShader;
		delete m_vertexShader;
		delete m_swapchain;
		delete m_device;
		delete m_window;
		delete m_instance;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::Run()
	{
		ImGuiIO& io = ImGui::GetIO();

		float lastUpdateTime = Time::ElapsedSinceStartup();

		while ( glfwWindowShouldClose(m_window->GetWindow()) == false)
		{
			const float time = Time::ElapsedSinceStartup();
			Input::NewFrame();

			const float updateDelta = time - lastUpdateTime;
			if (updateDelta > 1.f / Time::GetFPS() ) {
				lastUpdateTime = time;

				const VkResult result = m_swapchain->AcquireNextImage();
				if (result == VK_ERROR_OUT_OF_DATE_KHR) {
					if (m_window->GetFramebufferSize().width == 0 && m_window->GetFramebufferSize().height == 0) {
						continue;
					}

					std::cout << "suboptimal swapchain" << std::endl;
					vkDeviceWaitIdle(m_device->vkDevice);

					DeletePipeline();
					DeleteFramebuffers();
					DeleteDepthRessources();
					DeleteSwapchainFramebuffers();

					delete m_postprocessPipeline;

					m_swapchain->Resize( m_window->GetFramebufferSize() );

					m_postprocessPipeline = new PostprocessPipeline(m_device);
					m_postprocessPipeline->Create(m_swapchain->GetSurfaceFormat().format, m_swapchain->GetExtent());
					
					CreateSwapchainFramebuffers();
					CreateDepthRessources();		
					CreateFramebuffers();
					CreatePipeline();
					RecordAllCommandBuffers();
					vkResetFences(m_device->vkDevice, 1, m_swapchain->GetCurrentInFlightFence());
					m_swapchain->AcquireNextImage();
				}
				else if (result != VK_SUCCESS) {
					std::cout << "Could not acquire next image" << std::endl;
				}
				else {
					vkWaitForFences(m_device->vkDevice, 1, m_swapchain->GetCurrentInFlightFence(), VK_TRUE, std::numeric_limits<uint64_t>::max());
					vkResetFences(m_device->vkDevice, 1, m_swapchain->GetCurrentInFlightFence());
				}

				ImGui::NewFrame();

				lastUpdateTime = time;
				io.DisplaySize = ImVec2(static_cast<float>(m_swapchain->GetExtent().width), static_cast<float>(m_swapchain->GetExtent().height));
				{
					ImGui::ShowDemoWindow();
					ImGui::Begin("Debug");

					float tmpFps = Time::GetFPS();
					if (ImGui::DragFloat("Framerate", &tmpFps, 1.f, Time::minFps, 144.f)) {
						Time::SetFPS(tmpFps);
					}

					std::stringstream ssFramerate;
					ssFramerate << 1.f / updateDelta;
					ImGui::Text(ssFramerate.str().c_str());
					//ImGui::Text("ARKANEUUUUH!");
					if (ImGui::Button("reload shaders")) {
						ReloadShaders();
					}
					UpdateUniformBuffer();
					ImGui::End();
				}				
				ImGui::EndFrame();
				ImGui::Render();

				RecordCommandBufferImgui(m_swapchain->GetCurrentFrame());
				RecordPrimaryCommandBuffer(m_swapchain->GetCurrentFrame());
				SubmitCommandBuffers();

				m_swapchain->PresentImage();
				m_swapchain->StartNextFrame();
			}	

		}
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
		vkAllocateCommandBuffers(m_device->vkDevice, &allocInfo, &commandBuffer);

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

		vkQueueSubmit(m_device->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(m_device->GetGraphicsQueue());

		// Cleaning
		vkFreeCommandBuffers(m_device->vkDevice, m_commandPool, 1, &commandBuffer);
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Renderer::ResetCommandPool() {
		VkCommandPoolResetFlags releaseResources = VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT;

		if (vkResetCommandPool(m_device->vkDevice, m_commandPool, releaseResources) != VK_SUCCESS) {
			std::cout << "Could not reset command pool." << std::endl;
			return false;
		}
		return true;
	}

	static void ShowHelpMarker(const char* desc)
	{
		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(desc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::UpdateUniformBuffer()
	{
		// Ui
		static float s_speed = 1.f;

		ImGui::SliderFloat("rotation speed", &s_speed, 0.f, 1000.f);		

		UniformsForward ubo = {};
		ubo.model = glm::rotate(glm::mat4(1.0f), Time::ElapsedSinceStartup() * glm::radians(s_speed), glm::vec3(0.0f, 1.0f, 0.0f));
		ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		ubo.proj = glm::perspective(glm::radians(45.0f), m_swapchain->GetExtent().width / (float)m_swapchain->GetExtent().height, 0.1f, 10.0f);
		ubo.proj[1][1] *= -1; 			//the Y coordinate of the clip coordinates is inverted

		m_uniformBuffer->SetData(&ubo, sizeof(ubo));		

		int misc_flags = 
			 ImGuiColorEditFlags_PickerHueWheel
			| ImGuiColorEditFlags_AlphaBar;

		
		PostprocessPipeline::UniformsPostprocess uniforms = m_postprocessPipeline->GetUniforms();

		ImGui::ColorEdit3("PostprocessColor##1", &uniforms.color.r, misc_flags);
		ImGui::SameLine(); ShowHelpMarker("Click on the colored square to open a color picker.\nCTRL+click on individual component to input value.\n");
		
		ImGui::ColorEdit3("ClearColor##1", &m_clearColor.r, misc_flags);
		ImGui::SameLine(); ShowHelpMarker("Click on the colored square to open a color picker.\nCTRL+click on individual component to input value.\n");
		
		m_postprocessPipeline->SetUniforms(uniforms);

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
		for (int cmdBufferIndex = 0; cmdBufferIndex < m_swapchainFramebuffers.size(); cmdBufferIndex++) {
			m_postprocessPipeline->RecordCommandBufferPostProcess(m_postprocessCommandBuffers[cmdBufferIndex],m_swapchainFramebuffers[cmdBufferIndex]->GetFrameBuffer());
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
		renderPassInfoPostprocess.renderPass = m_postprocessPipeline->GetRenderPass();
		renderPassInfoPostprocess.framebuffer = m_swapchainFramebuffers[_index]->GetFrameBuffer();
		renderPassInfoPostprocess.renderArea.offset = { 0,0 };
		renderPassInfoPostprocess.renderArea.extent.width = m_swapchain->GetExtent().width;
		renderPassInfoPostprocess.renderArea.extent.height = m_swapchain->GetExtent().height;
		renderPassInfoPostprocess.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfoPostprocess.pClearValues = clearValues.data();

		if (vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo) == VK_SUCCESS) {
			
			vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS); {
				vkCmdExecuteCommands(commandBuffer, 1, &m_geometryCommandBuffers[_index]);
				vkCmdExecuteCommands(commandBuffer, 1, &m_imguiCommandBuffers[_index]);
			} vkCmdEndRenderPass(commandBuffer);
			
			vkCmdBeginRenderPass(commandBuffer, &renderPassInfoPostprocess, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS); {
				vkCmdExecuteCommands(commandBuffer, 1, &m_postprocessCommandBuffers[_index] );
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
	void Renderer::RecordCommandBufferImgui(const int _index) {

		m_imguiPipeline->UpdateBuffer(_index);

		VkCommandBuffer commandBuffer = m_imguiCommandBuffers[_index];

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
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);

			VkBuffer vertexBuffers[] = { m_vertexBuffer->GetBuffer() };

			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
			vkCmdBindIndexBuffer(commandBuffer, m_indexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1, &m_descriptorSet, 0, nullptr);
			vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(m_indices.size()), 1, 0, 0, 0);

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

		VkResult result = vkQueueSubmit(m_device->GetGraphicsQueue(), 1, &submitInfo, *m_swapchain->GetCurrentInFlightFence());
		if (result != VK_SUCCESS) {
			std::cout << "Could not submit draw command buffer " << std::endl;
			return false;
		}

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::ReloadShaders() {
		vkDeviceWaitIdle(m_device->vkDevice);

		CreateShaders();
		DeletePipeline();
		delete m_postprocessPipeline;
		CreatePipeline();
		m_postprocessPipeline = new PostprocessPipeline(m_device);
		m_postprocessPipeline->Create(m_swapchain->GetSurfaceFormat().format, m_swapchain->GetExtent());
		ResetCommandPool();
		CreateCommandBuffers();
		RecordAllCommandBuffers();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::CreateShaders() {
		delete m_fragmentShader;
		delete m_vertexShader;

		m_fragmentShader = new Shader(m_device);
		m_fragmentShader->Create("shaders/forward.frag");

		m_vertexShader = new Shader(m_device);
		m_vertexShader->Create("shaders/forward.vert");
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Renderer::CreateDescriptors() {
		VkDescriptorSetLayoutBinding uboLayoutBinding;
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr;

		std::vector< VkDescriptorSetLayoutBinding > layoutBindings = {
			uboLayoutBinding
		};

		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
		descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutCreateInfo.pNext = nullptr;
		descriptorSetLayoutCreateInfo.flags = 0;
		descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
		descriptorSetLayoutCreateInfo.pBindings = layoutBindings.data();

		if (vkCreateDescriptorSetLayout(m_device->vkDevice, &descriptorSetLayoutCreateInfo, nullptr, &m_descriptorSetLayout) != VK_SUCCESS) {
			std::cout << "Could not allocate descriptor set layout." << std::endl;
			return false;
		}
		std::cout << std::hex << "VkDescriptorSetLayout\t" << m_descriptorSetLayout << std::dec << std::endl;

		std::vector< VkDescriptorPoolSize > poolSizes(1);
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount = 1;

		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
		descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolCreateInfo.pNext = nullptr;
		descriptorPoolCreateInfo.flags = 0;
		descriptorPoolCreateInfo.maxSets = 1;
		descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		descriptorPoolCreateInfo.pPoolSizes = poolSizes.data();

		if (vkCreateDescriptorPool(m_device->vkDevice, &descriptorPoolCreateInfo, nullptr, &m_descriptorPool) != VK_SUCCESS) {
			std::cout << "Could not allocate descriptor pool." << std::endl;
			return false;
		}
		std::cout << std::hex << "VkDescriptorPool\t" << m_descriptorPool << std::dec << std::endl;

		std::vector< VkDescriptorSetLayout > descriptorSetLayouts = {
			m_descriptorSetLayout
		};

		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo;
		descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorSetAllocateInfo.pNext = nullptr;
		descriptorSetAllocateInfo.descriptorPool = m_descriptorPool;
		descriptorSetAllocateInfo.descriptorSetCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		descriptorSetAllocateInfo.pSetLayouts = descriptorSetLayouts.data();

		std::vector<VkDescriptorSet> descriptorSets(descriptorSetLayouts.size());
		if (vkAllocateDescriptorSets(m_device->vkDevice, &descriptorSetAllocateInfo, descriptorSets.data()) != VK_SUCCESS) {
			std::cout << "Could not allocate descriptor set." << std::endl;
			return false;
		}
		m_descriptorSet = descriptorSets[0];
		std::cout << std::hex << "VkDescriptorSet\t\t" << m_descriptorSet << std::dec << std::endl;

		m_uniformBuffer = new Buffer(m_device);
		m_uniformBuffer->Create(
			sizeof(UniformsForward),
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
		);

		VkDescriptorBufferInfo uboDescriptorBufferInfo = {};
		uboDescriptorBufferInfo.buffer = m_uniformBuffer->GetBuffer();
		uboDescriptorBufferInfo.offset = 0;
		uboDescriptorBufferInfo.range = sizeof(UniformsForward);

		VkWriteDescriptorSet uboWriteDescriptorSet = {};
		uboWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		uboWriteDescriptorSet.pNext = nullptr;
		uboWriteDescriptorSet.dstSet = m_descriptorSet;
		uboWriteDescriptorSet.dstBinding = 0;
		uboWriteDescriptorSet.dstArrayElement = 0;
		uboWriteDescriptorSet.descriptorCount = 1;
		uboWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboWriteDescriptorSet.pImageInfo = nullptr;
		uboWriteDescriptorSet.pBufferInfo = &uboDescriptorBufferInfo;
		//uboWriteDescriptorSet.pTexelBufferView = nullptr;

		std::vector<VkWriteDescriptorSet> writeDescriptors = { uboWriteDescriptorSet };

		vkUpdateDescriptorSets(
			m_device->vkDevice,
			static_cast<uint32_t>(writeDescriptors.size()),
			writeDescriptors.data(),
			0,
			nullptr
		);

		return true;
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

		if (vkAllocateCommandBuffers(m_device->vkDevice, &commandBufferAllocateInfo, m_primaryCommandBuffers.data()) != VK_SUCCESS) {
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
		if (vkAllocateCommandBuffers(m_device->vkDevice, &secondaryCommandBufferAllocateInfo, m_geometryCommandBuffers.data()) != VK_SUCCESS) {
			std::cout << "Could not allocate command buffers." << std::endl;
			return false;
		}

		m_imguiCommandBuffers.resize(m_swapchain->GetSwapchainImagesCount());
		if (vkAllocateCommandBuffers(m_device->vkDevice, &secondaryCommandBufferAllocateInfo, m_imguiCommandBuffers.data()) != VK_SUCCESS) {
			std::cout << "Could not allocate command buffers." << std::endl;
			return false;
		}

		m_postprocessCommandBuffers.resize(m_swapchain->GetSwapchainImagesCount());
		if (vkAllocateCommandBuffers(m_device->vkDevice, &secondaryCommandBufferAllocateInfo, m_postprocessCommandBuffers.data()) != VK_SUCCESS) {
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
		commandPoolCreateInfo.queueFamilyIndex = m_device->GetGraphicsQueueFamilyIndex();

		if (vkCreateCommandPool(m_device->vkDevice, &commandPoolCreateInfo, nullptr, &m_commandPool) != VK_SUCCESS) {
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
		depthAttachment.format = m_device->FindDepthFormat();
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

		if (vkCreateRenderPass(m_device->vkDevice, &renderPassCreateInfo, nullptr, &m_renderPass) != VK_SUCCESS) {
			std::cout << "Could not create render pass;" << std::endl;
			return false;
		}
		std::cout << std::hex << "VkRenderPass\t\t" << m_renderPass << std::dec << std::endl;

		return true;
	}	

	//================================================================================================================================
	//================================================================================================================================
	bool Renderer::CreateDepthRessources() {
		VkFormat depthFormat = m_device->FindDepthFormat();
		m_depthImage = new Image(m_device);
		m_depthImageView = new ImageView(m_device);
		m_depthImage->Create(depthFormat, m_swapchain->GetExtent(), VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		m_depthImageView->Create(m_depthImage->GetImage(), depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_VIEW_TYPE_2D);

		VkCommandBuffer cmd = BeginSingleTimeCommands();
		m_depthImage->TransitionImageLayout(cmd, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);
		EndSingleTimeCommands(cmd);

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::CreateFramebuffers() {
		m_forwardFrameBuffers.resize(m_swapchain->GetSwapchainImagesCount());
		for (int framebufferIndex = 0; framebufferIndex < m_forwardFrameBuffers.size(); framebufferIndex++) {
			std::vector<VkImageView> attachments =
			{
				m_postprocessPipeline->GetImageView(),			
				m_depthImageView->GetImageView()
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
			m_swapchainFramebuffers[framebufferIndex]->Create(m_postprocessPipeline->GetRenderPass(), attachments, m_swapchain->GetExtent());

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
	bool Renderer::CreatePipeline() {
		VkPipelineShaderStageCreateInfo vertshaderStageCreateInfos = {};
		vertshaderStageCreateInfos.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertshaderStageCreateInfos.pNext = nullptr;
		vertshaderStageCreateInfos.flags = 0;
		vertshaderStageCreateInfos.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertshaderStageCreateInfos.module = m_vertexShader->GetModule();
		vertshaderStageCreateInfos.pName = "main";
		vertshaderStageCreateInfos.pSpecializationInfo = nullptr;

		VkPipelineShaderStageCreateInfo fragShaderStageCreateInfos = {};
		fragShaderStageCreateInfos.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageCreateInfos.pNext = nullptr;
		fragShaderStageCreateInfos.flags = 0;
		fragShaderStageCreateInfos.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageCreateInfos.module = m_fragmentShader->GetModule();
		fragShaderStageCreateInfos.pName = "main";
		fragShaderStageCreateInfos.pSpecializationInfo = nullptr;

		std::vector < VkPipelineShaderStageCreateInfo> shaderStages = { vertshaderStageCreateInfos, fragShaderStageCreateInfos };
		std::vector < VkVertexInputBindingDescription > bindingDescription = Vertex::GetBindingDescription();
		std::vector < VkVertexInputAttributeDescription > attributeDescriptions = Vertex::GetAttributeDescriptions();

		VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {};
		vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputStateCreateInfo.pNext = nullptr;
		vertexInputStateCreateInfo.flags = 0;
		vertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescription.size());
		vertexInputStateCreateInfo.pVertexBindingDescriptions = bindingDescription.data();
		vertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());;
		vertexInputStateCreateInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = {};
		inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyStateCreateInfo.pNext = nullptr;
		inputAssemblyStateCreateInfo.flags = 0;
		inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

		std::vector< VkViewport > viewports(1);
		viewports[0].x = 0.f;
		viewports[0].y = 0.f;
		viewports[0].width = static_cast<float> (m_swapchain->GetExtent().width);
		viewports[0].height = static_cast<float> (m_swapchain->GetExtent().height);
		viewports[0].minDepth = 0.0f;
		viewports[0].maxDepth = 1.0f;

		std::vector<VkRect2D> scissors(1);
		scissors[0].offset = { 0, 0 };
		scissors[0].extent = m_swapchain->GetExtent();

		VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
		viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportStateCreateInfo.pNext = nullptr;
		viewportStateCreateInfo.flags = 0;
		viewportStateCreateInfo.viewportCount = static_cast<uint32_t> (viewports.size());
		viewportStateCreateInfo.pViewports = viewports.data();
		viewportStateCreateInfo.scissorCount = static_cast<uint32_t> (scissors.size());;
		viewportStateCreateInfo.pScissors = scissors.data();

		VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {};
		rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizationStateCreateInfo.pNext = nullptr;
		rasterizationStateCreateInfo.flags = 0;
		rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
		rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
		rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
		rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
		rasterizationStateCreateInfo.depthBiasClamp = 0.0f;
		rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;
		rasterizationStateCreateInfo.lineWidth = 1.0f;

		VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = {};
		multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampleStateCreateInfo.pNext = nullptr;
		multisampleStateCreateInfo.flags = 0;
		multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
		//multisampleStateCreateInfo.minSampleShading=;
		//multisampleStateCreateInfo.pSampleMask=;
		//multisampleStateCreateInfo.alphaToCoverageEnable=;
		//multisampleStateCreateInfo.alphaToOneEnable=;

		VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo = {};
		depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencilStateCreateInfo.pNext = nullptr;
		depthStencilStateCreateInfo.flags = 0;
		depthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
		depthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
		depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
		depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
		//depthStencilStateCreateInfo.front=;
		//depthStencilStateCreateInfo.back=;
		//depthStencilStateCreateInfo.minDepthBounds=;
		//depthStencilStateCreateInfo.maxDepthBounds=;

		std::vector<VkPipelineColorBlendAttachmentState> attachmentBlendStates(1);
		attachmentBlendStates[0].blendEnable = VK_FALSE;
		attachmentBlendStates[0].srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		attachmentBlendStates[0].dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		attachmentBlendStates[0].colorBlendOp = VK_BLEND_OP_ADD;
		attachmentBlendStates[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		attachmentBlendStates[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		attachmentBlendStates[0].alphaBlendOp = VK_BLEND_OP_ADD;
		attachmentBlendStates[0].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

		VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {};
		colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendStateCreateInfo.pNext = nullptr;
		colorBlendStateCreateInfo.flags = 0;
		colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
		colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
		colorBlendStateCreateInfo.attachmentCount = static_cast<uint32_t>(attachmentBlendStates.size());
		colorBlendStateCreateInfo.pAttachments = attachmentBlendStates.data();
		colorBlendStateCreateInfo.blendConstants[0] = 0.0f;
		colorBlendStateCreateInfo.blendConstants[1] = 0.0f;
		colorBlendStateCreateInfo.blendConstants[2] = 0.0f;
		colorBlendStateCreateInfo.blendConstants[3] = 0.0f;

		std::vector<VkDynamicState> dynamicStates = {
		};

		VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
		dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateCreateInfo.pNext = nullptr;
		dynamicStateCreateInfo.flags = 0;
		dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts = {
			m_descriptorSetLayout
		};
		std::vector<VkPushConstantRange> pushConstantRanges(0);

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.pNext = nullptr;
		pipelineLayoutCreateInfo.flags = 0;
		pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutCreateInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
		pipelineLayoutCreateInfo.pPushConstantRanges = pushConstantRanges.data();

		if (vkCreatePipelineLayout(m_device->vkDevice, &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
			std::cout << "Could not allocate command pool." << std::endl;
			return false;
		}
		std::cout << std::hex << "VkPipelineLayout\t" << m_pipelineLayout << std::dec << std::endl;

		VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
		graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		graphicsPipelineCreateInfo.pNext = nullptr;
		graphicsPipelineCreateInfo.flags = 0;
		graphicsPipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
		graphicsPipelineCreateInfo.pStages = shaderStages.data();
		graphicsPipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
		graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
		graphicsPipelineCreateInfo.pTessellationState = nullptr;
		graphicsPipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
		graphicsPipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
		graphicsPipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
		graphicsPipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;
		graphicsPipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
		graphicsPipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
		graphicsPipelineCreateInfo.layout = m_pipelineLayout;
		graphicsPipelineCreateInfo.renderPass = m_renderPass;
		graphicsPipelineCreateInfo.subpass = 0;
		graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
		graphicsPipelineCreateInfo.basePipelineIndex = -1;

		std::vector<VkGraphicsPipelineCreateInfo> graphicsPipelineCreateInfos = {
			graphicsPipelineCreateInfo
		};

		std::vector<VkPipeline> graphicsPipelines(graphicsPipelineCreateInfos.size());

		if (vkCreateGraphicsPipelines(
			m_device->vkDevice,
			VK_NULL_HANDLE,
			static_cast<uint32_t>(graphicsPipelineCreateInfos.size()),
			graphicsPipelineCreateInfos.data(),
			nullptr,
			graphicsPipelines.data()
		) != VK_SUCCESS) {
			std::cout << "Could not allocate graphicsPipelines." << std::endl;
			return false;
		}

		m_pipeline = graphicsPipelines[0];

		for (int pipelineIndex = 0; pipelineIndex < graphicsPipelines.size(); pipelineIndex++) {
			std::cout << std::hex << "VkPipeline\t\t" << graphicsPipelines[pipelineIndex] << std::dec << std::endl;
		}
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::CreateVertexBuffers() {

		delete m_indexBuffer;
		delete m_vertexBuffer;

		{
			m_indices = { //cube
				 0,1,2	,1,3,2	// top
				,6,5,4	,7,5,6	// bot
				,7,6,2	,7,2,3
				,6,4,0	,6,0,2
				,4,5,0	,5,1,0
				,7,1,5	,7,3,1
			};

			const VkDeviceSize size = sizeof(m_indices[0]) * m_indices.size();

			m_indexBuffer = new Buffer(m_device);
			m_indexBuffer->Create(
				size,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);
			Buffer stagingBuffer(m_device);
			stagingBuffer.Create(
				size,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			);
			stagingBuffer.SetData(m_indices.data(), size);
			VkCommandBuffer cmd = BeginSingleTimeCommands();
			stagingBuffer.CopyBufferTo(cmd, m_indexBuffer->GetBuffer(), size);
			EndSingleTimeCommands(cmd);
		}
		{
			glm::vec3 color(0.f, 0.2, 0.f);

			Vertex v0 = { { +0.5,+0.5,+0.5},	color,{} };
			Vertex v1 = { { +0.5,+0.5,-0.5},	color,{} };
			Vertex v2 = { { -0.5,+0.5,+0.5},	color,{} };
			Vertex v3 = { { -0.5,+0.5,-0.5},	color,{} };
			Vertex v4 = { { +0.5,-0.5,+0.5},	color,{} };
			Vertex v5 = { { +0.5,-0.5,-0.5},	color,{} };
			Vertex v6 = { { -0.5,-0.5,+0.5},	color,{} };
			Vertex v7 = { { -0.5,-0.5,-0.5},	color,{} };
			m_vertices = { v0, v1 ,v2 ,v3 ,v4 ,v5 ,v6 ,v7 };

			const VkDeviceSize size = sizeof(m_vertices[0]) * m_vertices.size();
			m_vertexBuffer = new Buffer(m_device);
			m_vertexBuffer->Create(
				size,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);
			Buffer stagingBuffer2(m_device);
			stagingBuffer2.Create(
				size,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			);
			stagingBuffer2.SetData(m_vertices.data(), size);
			VkCommandBuffer cmd2 = BeginSingleTimeCommands();
			stagingBuffer2.CopyBufferTo(cmd2, m_vertexBuffer->GetBuffer(), size);
			EndSingleTimeCommands(cmd2);
		}
	}
	
	//================================================================================================================================
	//================================================================================================================================
	void Renderer::DeleteCommandPool() {
		vkDestroyCommandPool(m_device->vkDevice, m_commandPool, nullptr);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::DeleteRenderPass() {
		if (m_renderPass != VK_NULL_HANDLE) {
			vkDestroyRenderPass(m_device->vkDevice, m_renderPass, nullptr);
			m_renderPass = VK_NULL_HANDLE;
		}
	}


	//================================================================================================================================
	//================================================================================================================================
	void Renderer::DeleteDepthRessources() {
		delete m_depthImageView;
		delete m_depthImage;
	}


	//================================================================================================================================
	//================================================================================================================================
	void Renderer::DeleteFramebuffers() {
		for (int framebufferIndex = 0; framebufferIndex < m_forwardFrameBuffers.size(); framebufferIndex++) {
			delete m_forwardFrameBuffers[framebufferIndex];
		}
		m_forwardFrameBuffers.clear();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::DeletePipeline() {
		if (m_pipelineLayout != VK_NULL_HANDLE) {
			vkDestroyPipelineLayout(m_device->vkDevice, m_pipelineLayout, nullptr);
			m_pipelineLayout = VK_NULL_HANDLE;
		}

		if (m_pipeline != VK_NULL_HANDLE) {
			vkDestroyPipeline(m_device->vkDevice, m_pipeline, nullptr);
			m_pipeline = VK_NULL_HANDLE;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Renderer::DeleteDescriptors() {

		if (m_descriptorPool != VK_NULL_HANDLE) {
			vkDestroyDescriptorPool(m_device->vkDevice, m_descriptorPool, nullptr);
			m_descriptorPool = VK_NULL_HANDLE;
		}

		if (m_descriptorSetLayout != VK_NULL_HANDLE) {
			vkDestroyDescriptorSetLayout(m_device->vkDevice, m_descriptorSetLayout, nullptr);
			m_descriptorSetLayout = VK_NULL_HANDLE;
		}
		delete m_uniformBuffer;
	}
}