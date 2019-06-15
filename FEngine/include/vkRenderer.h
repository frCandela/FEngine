#pragma once


#include "AllInclude.h"

#include "vkInstance.h"
#include "vkDevice.h"
#include "vkWindow.h"
#include "vkSwapChain.h"
#include "vkBuffer.h"
#include "vkImage.h"
#include "vkImageView.h"

class Renderer {
public:
	Renderer(const VkExtent2D _size ) :
		m_instance( new Instance()),
		m_window( new Window("Vulkan", _size, m_instance->vkInstance)),
		m_device( new Device( m_instance, m_window->GetSurface())),
		m_swapchain( new SwapChain( m_device, m_window->GetSurface(), _size, VK_NULL_HANDLE)) 
	{
		CreateCommandPool();
		CreateCommandBuffers();
		RecordCommandBuffers();		
		CreateRenderPass();
		CreateDepthRessources();
		CreateFramebuffers();
	}

	~Renderer() {
		DestroyFramebuffers();
		DestroyDepthRessources();
		DestroyRenderPass();
		FreeCommandBuffers();
		DestroyCommandPool();		

		delete m_swapchain;
		delete m_device;		
		delete m_window;
		delete m_instance;		
	}

	void Run() 
	{
		while ( ! glfwWindowShouldClose(m_window->GetWindow()))
		{
			glfwPollEvents();
			
			VkResult result = m_swapchain->AcquireNextImage();
			if (result == VK_ERROR_OUT_OF_DATE_KHR) {
				std::cout << "suboptimal swapchain" << std::endl;
				vkDeviceWaitIdle( m_device->vkDevice);

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

private:
	Instance * m_instance;
	Window *  m_window;
	Device *  m_device;
	SwapChain  * m_swapchain;

	VkCommandPool m_commandPool;
	VkRenderPass m_renderPass;

	std::vector<VkCommandBuffer> m_commandBuffers;
	std::vector< FrameBuffer * > m_swapChainframeBuffers;

	Image * m_depthImage;
	ImageView  * m_depthImageView;

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
		std::cout << std::hex << "VkCommandPool\t" << m_commandPool << std::dec << std::endl;
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

		for (int cmdBufferIndex = 0; cmdBufferIndex < m_commandBuffers.size() ; cmdBufferIndex++)
		{
			vkBeginCommandBuffer(m_commandBuffers[cmdBufferIndex], &commandBufferBeginInfo);
			if (vkEndCommandBuffer(m_commandBuffers[cmdBufferIndex]) != VK_SUCCESS) {
				std::cout << "Could not record command buffer " << cmdBufferIndex << "." << std::endl;
			}
		}
	}
	bool SubmitCommandBuffers() {
		
		std::vector<VkPipelineStageFlags> waitSemaphoreStages = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		VkSubmitInfo submitInfo;
		submitInfo.sType= VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext= nullptr;
		submitInfo.waitSemaphoreCount=1;
		submitInfo.pWaitSemaphores = m_swapchain->GetCurrentImageAvailableSemaphore();
		submitInfo.pWaitDstStageMask = waitSemaphoreStages.data();
		submitInfo.commandBufferCount=1;
		submitInfo.pCommandBuffers= &m_commandBuffers[m_swapchain->GetCurrentImageIndex()];
		submitInfo.signalSemaphoreCount=1;
		submitInfo.pSignalSemaphores= m_swapchain->GetCurrentRenderFinishedSemaphore();

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
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

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
		std::cout << std::hex << "VkRenderPass\t" << m_renderPass << std::dec << std::endl;	

		return true;
	}
	void DestroyRenderPass() {
		if ( m_renderPass != VK_NULL_HANDLE ) { 
			vkDestroyRenderPass( m_device->vkDevice, m_renderPass, nullptr);  
			m_renderPass = VK_NULL_HANDLE; 
		}
	}
	bool CreateDepthRessources() {
		VkFormat depthFormat = m_device->FindDepthFormat();
		m_depthImage = new Image(m_device);
		m_depthImageView = new ImageView(m_device);
		m_depthImage->Create(depthFormat, m_swapchain->GetSwapchainExtent(), VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
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
		m_swapChainframeBuffers.resize(m_swapchain->GetSwapchainImagesCount() );
		for (int framebufferIndex = 0; framebufferIndex < m_swapChainframeBuffers.size(); framebufferIndex++) {
			std::vector<VkImageView> attachments =
			{
				m_swapchain->GetImageView(framebufferIndex),
				m_depthImageView->GetImageView()
			};

			m_swapChainframeBuffers[framebufferIndex] = new FrameBuffer(m_device);
			m_swapChainframeBuffers[framebufferIndex]->Create(m_renderPass, attachments, m_swapchain->GetSwapchainExtent());

		}
	}
	void DestroyFramebuffers() {
		for (int framebufferIndex = 0; framebufferIndex < m_swapChainframeBuffers.size(); framebufferIndex++) {
			delete m_swapChainframeBuffers[framebufferIndex];
		}
		m_swapChainframeBuffers.clear();
	}
};