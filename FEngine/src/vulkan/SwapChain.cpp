#include "vulkan/SwapChain.h"

#include <algorithm>
#include <array>

namespace vk
{

	SwapChain::SwapChain(Device& device, CommandPool& rCommandPool) :
		m_device(device),
		m_rCommandPool(rCommandPool)
	{
		depthImage = new DepthImage(device, m_rCommandPool);
	}

	SwapChain::~SwapChain()
	{
		CleanupSwapChain();
	}

	void SwapChain::BuildSwapChain(Window& window)
	{
		CreateSwapChain(window);
		//CreateImageViews();
		depthImage->CreateDepthResources(swapChainExtent.width, swapChainExtent.height);
	}

	void SwapChain::CreateSwapChain(Window& window)
	{
		m_device.QuerySwapChainSupport(m_device.physicalDevice, m_device.surface);

		VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(m_device.swapChainSupportDetails.formats);
		VkPresentModeKHR presentMode = ChooseSwapPresentMode(m_device.swapChainSupportDetails.presentModes);
		VkExtent2D extent = ChooseSwapExtent(m_device.swapChainSupportDetails.capabilities, window);

		uint32_t imageCount = m_device.swapChainSupportDetails.capabilities.minImageCount + 1; //one more frame to implement triple buffering
		if (m_device.swapChainSupportDetails.capabilities.maxImageCount > 0 && imageCount > m_device.swapChainSupportDetails.capabilities.maxImageCount)
			imageCount = m_device.swapChainSupportDetails.capabilities.maxImageCount;

		//create swap chain info structure
		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = m_device.surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;// amount of layers each image consists of
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		uint32_t queueFamilyIndices[] = { (uint32_t)m_device.queueFamilyIndices.graphicsFamily, (uint32_t)m_device.queueFamilyIndices.presentFamily };
		if (m_device.queueFamilyIndices.graphicsFamily != m_device.queueFamilyIndices.presentFamily)
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0; // Optional
			createInfo.pQueueFamilyIndices = nullptr; // Optional
		}

		createInfo.preTransform = m_device.swapChainSupportDetails.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		//Create the swapchain
		if (vkCreateSwapchainKHR(m_device.device, &createInfo, nullptr, &swapChain) != VK_SUCCESS)
			throw std::runtime_error("failed to create swap chain!");

		//retrieve the swap chain images
		vkGetSwapchainImagesKHR(m_device.device, swapChain, &imageCount, nullptr);		

		std::vector<VkImage> imagesArray;
		imagesArray.resize(imageCount);

		vkGetSwapchainImagesKHR(m_device.device, swapChain, &imageCount, imagesArray.data());
		
		images.resize(imageCount);

		swapChainImageFormat = surfaceFormat.format;
		swapChainExtent = extent;

		for (uint32_t i = 0; i < imageCount; ++i)
		{
			images[i] = new Image(m_device, m_rCommandPool);
			images[i]->image = imagesArray[i];			
		}

		//Create image views
		for (uint32_t i = 0; i < images.size(); i++)
			images[i]->CreateImageView(swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	}
	
	VkSurfaceFormatKHR SwapChain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		//surface has no preferred format
		if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED)
			return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };

		//else look for a good format combination
		for (const auto& availableFormat : availableFormats)
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				return availableFormat;

		//No good format found
		return availableFormats[0];
	}

	VkPresentModeKHR SwapChain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes)
	{
		VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;//Default always available

		for (const auto& availablePresentMode : availablePresentModes)
		{
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) //Prefered mode
				return availablePresentMode;
			else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) //Second choice
				bestMode = availablePresentMode;
		}

		return bestMode;
	}

	VkExtent2D SwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, Window& window)
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			return capabilities.currentExtent;
		else
		{
			VkExtent2D actualExtent = window.GetExtend2D();

			//Clamp the values between minImageExtent and maxImageExtent		
			actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

			return actualExtent;
		}
	}

	void SwapChain::CreateFramebuffers(VkRenderPass& renderPass)
	{
		//A framebuffer object references all of the VkImageView objects that represent the attachments specified during the render pass creation
		swapChainFramebuffers.resize(images.size());
		for (size_t i = 0; i < images.size(); i++)
		{
			std::array<VkImageView, 2> attachments =
			{
				images[i]->imageView,
				depthImage->imageView
			};

			//Specify the differents attachements
			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderPass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = swapChainExtent.width;
			framebufferInfo.height = swapChainExtent.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(m_device.device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS)
				throw std::runtime_error("failed to create framebuffer!");
		}
	}

	void SwapChain::CleanupSwapChain()
	{
		vkDestroyImageView(m_device.device, depthImage->imageView, nullptr);
		vkDestroyImage(m_device.device, depthImage->image, nullptr);
		vkFreeMemory(m_device.device, depthImage->deviceMemory, nullptr);

		for (size_t i = 0; i < swapChainFramebuffers.size(); i++)
			vkDestroyFramebuffer(m_device.device, swapChainFramebuffers[i], nullptr);

		for (size_t i = 0; i < images.size(); i++)
			vkDestroyImageView(m_device.device, images[i]->imageView, nullptr);

		vkDestroySwapchainKHR(m_device.device, swapChain, nullptr);
	}
}

