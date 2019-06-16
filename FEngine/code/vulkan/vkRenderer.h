#pragma once

#include "AllInclude.h"

#include "vkInstance.h"
#include "vkDevice.h"
#include "vkWindow.h"
#include "vkSwapChain.h"
#include "vkBuffer.h"
#include "vkImage.h"
#include "vkImageView.h"
#include "vkShader.h"
#include "vkVertex.h"

namespace vk {
	class Renderer {
	public:
		Renderer(const VkExtent2D _size) :
			m_instance(new Instance()),
			m_window(new Window("Vulkan", _size, m_instance->vkInstance)),
			m_device(new Device(m_instance, m_window->GetSurface())),
			m_swapchain(new SwapChain(m_device, m_window->GetSurface(), _size, VK_NULL_HANDLE))
		{
			CreateCommandPool();
			CreateRenderPass();
			CreateDepthRessources();
			CreateFramebuffers();
			CreateCommandBuffers();		
			CreatePipeline();
			RecordCommandBuffers();
			

		}

		~Renderer() {
			DestroyPipeline();
			DestroyFramebuffers();
			DestroyDepthRessources();
			DestroyRenderPass();
			FreeCommandBuffers();
			DestroyCommandPool();

			delete m_fragmentShader;
			delete m_vertexShader;
			delete m_swapchain;
			delete m_device;
			delete m_window;
			delete m_instance;

		}

		void Run()
		{
			while (!glfwWindowShouldClose(m_window->GetWindow()))
			{
				glfwPollEvents();

				VkResult result = m_swapchain->AcquireNextImage();
				if (result == VK_ERROR_OUT_OF_DATE_KHR) {
					std::cout << "suboptimal swapchain" << std::endl;
					vkDeviceWaitIdle(m_device->vkDevice);

					SwapChain * newSwapchain = new SwapChain(m_device, m_window->GetSurface(), m_window->GetFramebufferSize(), m_swapchain->GetVkSwapChain());
					delete m_swapchain;
					m_swapchain = newSwapchain;

					m_swapchain->AcquireNextImage();
				}
				else if (result != VK_SUCCESS) {
					std::cout << "Could not acquire next image" << std::endl;
				}

				uint32_t index = m_swapchain->GetCurrentImageIndex();
				uint32_t frame = m_swapchain->GetCurrentFrame();
				(void)index;
				(void)frame;
				// rebuild swap chain?
				SubmitCommandBuffers();
				m_swapchain->PresentImage();

				vkWaitForFences(m_device->vkDevice, 1, m_swapchain->GetCurrentInFlightFence(), VK_TRUE, std::numeric_limits<uint64_t>::max());
				vkResetFences(m_device->vkDevice, 1, m_swapchain->GetCurrentInFlightFence());
				m_swapchain->StartNextFrame();
			}
		}
	private:

		Instance * m_instance;
		Window *  m_window;
		Device *  m_device;
		SwapChain  * m_swapchain;

		VkCommandPool m_commandPool;
		VkRenderPass m_renderPass;
		VkPipelineLayout m_pipelineLayout;
		VkPipeline	m_pipeline;

		std::vector<VkCommandBuffer> m_commandBuffers;
		std::vector< FrameBuffer * > m_swapChainframeBuffers;

		Image * m_depthImage;
		ImageView  * m_depthImageView;

		Shader * m_fragmentShader;
		Shader * m_vertexShader;

		VkCommandBuffer BeginSingleTimeCommands()
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
		void EndSingleTimeCommands(VkCommandBuffer commandBuffer)
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

		bool CreateCommandPool() {
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
		bool ResetCommandPool() {
			VkCommandPoolResetFlags releaseResources = VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT;

			if (vkResetCommandPool(m_device->vkDevice, m_commandPool, releaseResources) != VK_SUCCESS) {
				std::cout << "Could not reset command pool." << std::endl;
				return false;
			}
			return true;
		}
		void DestroyCommandPool() {
			vkDestroyCommandPool(m_device->vkDevice, m_commandPool, nullptr);
		}
		bool CreateCommandBuffers() {
			VkCommandBufferAllocateInfo commandBufferAllocateInfo;
			commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			commandBufferAllocateInfo.pNext = nullptr;
			commandBufferAllocateInfo.commandPool = m_commandPool;
			commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			commandBufferAllocateInfo.commandBufferCount = m_swapchain->GetSwapchainImagesCount();

			m_commandBuffers.resize(m_swapchain->GetSwapchainImagesCount());

			if (vkAllocateCommandBuffers(m_device->vkDevice, &commandBufferAllocateInfo, m_commandBuffers.data()) != VK_SUCCESS) {
				std::cout << "Could not allocate command buffers." << std::endl;
				return false;
			}
			return true;
		}
		void FreeCommandBuffers() {
			vkFreeCommandBuffers(m_device->vkDevice, m_commandPool, static_cast<uint32_t>(m_commandBuffers.size()), m_commandBuffers.data());
			m_commandBuffers.clear();
		}
		void RecordCommandBuffers() {
			VkCommandBufferBeginInfo commandBufferBeginInfo;
			commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			commandBufferBeginInfo.pNext = nullptr;
			commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
			commandBufferBeginInfo.pInheritanceInfo = nullptr;

			std::vector<VkClearValue> clearValues(2);
			clearValues[0].color = { 1.0f, 0.0f, 0.0f, 1.0f };
			clearValues[1].depthStencil = { 1.0f, 0 };

			for (int cmdBufferIndex = 0; cmdBufferIndex < m_commandBuffers.size(); cmdBufferIndex++)
			{
				VkRenderPassBeginInfo renderPassInfo = {};
				renderPassInfo.sType= VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
				renderPassInfo.pNext=nullptr;
				renderPassInfo.renderPass= m_renderPass;
				renderPassInfo.framebuffer= m_swapChainframeBuffers[cmdBufferIndex]->GetFrameBuffer();
				renderPassInfo.renderArea.offset = { 0,0 };
				renderPassInfo.renderArea.extent.width = m_swapchain->GetExtent().width;
				renderPassInfo.renderArea.extent.height = m_swapchain->GetExtent().height;
				renderPassInfo.clearValueCount= static_cast<uint32_t>(clearValues.size());
				renderPassInfo.pClearValues= clearValues.data();

				if (vkBeginCommandBuffer(m_commandBuffers[cmdBufferIndex], &commandBufferBeginInfo) != VK_SUCCESS) {
					std::cout << "Could not record command buffer " << cmdBufferIndex << "." << std::endl;
				}

				vkCmdBeginRenderPass(m_commandBuffers[cmdBufferIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
				vkCmdBindPipeline(m_commandBuffers[cmdBufferIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
				
				vkCmdEndRenderPass(m_commandBuffers[cmdBufferIndex]);


				if (vkEndCommandBuffer(m_commandBuffers[cmdBufferIndex]) != VK_SUCCESS) {
					std::cout << "Could not record command buffer " << cmdBufferIndex << "." << std::endl;
				}
			}
		}
		bool SubmitCommandBuffers() {

			std::vector<VkPipelineStageFlags> waitSemaphoreStages = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

			VkSubmitInfo submitInfo;
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.pNext = nullptr;
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = m_swapchain->GetCurrentImageAvailableSemaphore();
			submitInfo.pWaitDstStageMask = waitSemaphoreStages.data();
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &m_commandBuffers[m_swapchain->GetCurrentImageIndex()];
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = m_swapchain->GetCurrentRenderFinishedSemaphore();

			if (vkQueueSubmit(m_device->GetGraphicsQueue(), 1, &submitInfo, *m_swapchain->GetCurrentInFlightFence()) != VK_SUCCESS) {
				std::cout << "Could not submit draw command buffer " << std::endl;
				return false;
			}

			return true;
		}
		bool CreateRenderPass() {
			VkAttachmentDescription colorAttachment;
			colorAttachment.flags = 0;
			colorAttachment.format = m_swapchain->GetSurfaceFormat().format;
			colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
			colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

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
		void DestroyRenderPass() {
			if (m_renderPass != VK_NULL_HANDLE) {
				vkDestroyRenderPass(m_device->vkDevice, m_renderPass, nullptr);
				m_renderPass = VK_NULL_HANDLE;
			}
		}
		bool CreateDepthRessources() {
			VkFormat depthFormat = m_device->FindDepthFormat();
			m_depthImage = new Image(m_device);
			m_depthImageView = new ImageView(m_device);
			m_depthImage->Create(depthFormat, m_swapchain->GetExtent(), VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			m_depthImageView->Create(m_depthImage->GetImage(), depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

			VkCommandBuffer cmd = BeginSingleTimeCommands();
			m_depthImage->TransitionImageLayout(cmd, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);
			EndSingleTimeCommands(cmd);

			return true;
		}
		void DestroyDepthRessources() {
			delete m_depthImageView;
			delete m_depthImage;
		}
		void CreateFramebuffers() {
			m_swapChainframeBuffers.resize(m_swapchain->GetSwapchainImagesCount());
			for (int framebufferIndex = 0; framebufferIndex < m_swapChainframeBuffers.size(); framebufferIndex++) {
				std::vector<VkImageView> attachments =
				{
					m_swapchain->GetImageView(framebufferIndex),
					m_depthImageView->GetImageView()
				};

				m_swapChainframeBuffers[framebufferIndex] = new FrameBuffer(m_device);
				m_swapChainframeBuffers[framebufferIndex]->Create(m_renderPass, attachments, m_swapchain->GetExtent());

			}
		}
		void DestroyFramebuffers() {
			for (int framebufferIndex = 0; framebufferIndex < m_swapChainframeBuffers.size(); framebufferIndex++) {
				delete m_swapChainframeBuffers[framebufferIndex];
			}
			m_swapChainframeBuffers.clear();
		}
		bool CreatePipeline() {
			m_fragmentShader = new Shader(m_device);
			m_fragmentShader->Create("shaders/shader.frag");
			m_vertexShader = new Shader(m_device);
			m_vertexShader->Create("shaders/shader.vert");

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
				VK_DYNAMIC_STATE_VIEWPORT
			};

			VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
			dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			dynamicStateCreateInfo.pNext = nullptr;
			dynamicStateCreateInfo.flags = 0;
			dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
			dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();

			std::vector<VkDescriptorSetLayout> descriptorSetLayouts(0);
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
		void DestroyPipeline() {
			if (m_pipelineLayout != VK_NULL_HANDLE) {
				vkDestroyPipelineLayout(m_device->vkDevice, m_pipelineLayout, nullptr);
				m_pipelineLayout = VK_NULL_HANDLE;
			}

			if (m_pipeline != VK_NULL_HANDLE ) {
				vkDestroyPipeline( m_device->vkDevice, m_pipeline, nullptr);  
				m_pipeline = VK_NULL_HANDLE;
			}
		}
};
}