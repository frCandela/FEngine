#pragma once

#include "AllInclude.h"

#include "vkDevice.h"

class SwapChain {
public:
	SwapChain( Device & _device, VkSurfaceKHR _surface, VkExtent2D _desiredSize ){
		SetDesiredPresentMode(VK_PRESENT_MODE_FIFO_KHR, _device.vkPhysicalDevice, _surface);
		SetNumberOfImages(_device.vkPhysicalDevice, _surface);
		SetImagesSize(_desiredSize);
		SetImagesUsage();
		SetDesiredSurfaceFormat(_device.vkPhysicalDevice, _surface, { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR });

		VkSwapchainKHR oldSwapchain = VK_NULL_HANDLE;
		VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
		swapchainCreateInfo.sType= VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchainCreateInfo.pNext= nullptr;
		swapchainCreateInfo.flags=0;
		swapchainCreateInfo.surface=_surface;
		swapchainCreateInfo.minImageCount= m_imagesCount;
		swapchainCreateInfo.imageFormat= m_surfaceFormat.format;
		swapchainCreateInfo.imageColorSpace= m_surfaceFormat.colorSpace;
		swapchainCreateInfo.imageExtent=m_size;
		swapchainCreateInfo.imageArrayLayers=1;
		swapchainCreateInfo.imageUsage=m_imageUsage;
		swapchainCreateInfo.imageSharingMode= VK_SHARING_MODE_EXCLUSIVE;
		swapchainCreateInfo.queueFamilyIndexCount=0;
		swapchainCreateInfo.pQueueFamilyIndices= nullptr;
		swapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		swapchainCreateInfo.compositeAlpha= VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapchainCreateInfo.presentMode = m_presentMode;
		swapchainCreateInfo.clipped= VK_TRUE;
		swapchainCreateInfo.oldSwapchain= oldSwapchain;

		vkCreateSwapchainKHR(_device.vkDevice, &swapchainCreateInfo, nullptr, &m_swapchain);		
	}

private:
	std::vector<VkPresentModeKHR> m_presentModes;
	std::vector<VkSurfaceFormatKHR> m_surfaceFormats;

	VkSwapchainKHR m_swapchain;
	VkPresentModeKHR m_presentMode;
	VkSurfaceCapabilitiesKHR m_surfaceCapabilities;
	uint32_t m_imagesCount;
	VkExtent2D m_size;
	VkImageUsageFlags m_imageUsage;
	VkSurfaceFormatKHR m_surfaceFormat;

	void SetDesiredPresentMode(const VkPresentModeKHR _desiredPresentMode, VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface) {
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, _surface, &presentModeCount, nullptr);
		m_presentModes.clear();
		m_presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, _surface, &presentModeCount, m_presentModes.data());

		for (int presentModeIndex = 0; presentModeIndex < m_presentModes.size(); presentModeIndex++)
		{
			if (m_presentModes[presentModeIndex] == _desiredPresentMode) {
				m_presentMode = _desiredPresentMode;
				return;
			}
		}

		m_presentMode = VK_PRESENT_MODE_FIFO_KHR;		
	}
	void SetNumberOfImages( VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface ) {
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_physicalDevice, _surface, &m_surfaceCapabilities);
		m_imagesCount = m_surfaceCapabilities.minImageCount + 1;
		if ((m_surfaceCapabilities.maxImageCount > 0) && (m_imagesCount > m_surfaceCapabilities.maxImageCount)) {
			m_imagesCount = m_surfaceCapabilities.maxImageCount;
		}
	}
	void SetImagesSize( const VkExtent2D _desiredSize ) {
		if (m_surfaceCapabilities.currentExtent.height == 0xFFFFFFFF) {//means the size of images determines the size of the window
			m_size = _desiredSize;
			if (m_size.width > m_surfaceCapabilities.maxImageExtent.width) {
				m_size.width = m_surfaceCapabilities.maxImageExtent.width;
			}
			if (m_size.height > m_surfaceCapabilities.maxImageExtent.height) {
				m_size.height = m_surfaceCapabilities.maxImageExtent.height;
			}
		}
		else {
			m_size = m_surfaceCapabilities.currentExtent;
		}
	}
	void SetImagesUsage() {
		m_imageUsage = 0;
		if (m_surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) {
			m_imageUsage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		}
	}
	void SetDesiredSurfaceFormat(VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface, VkSurfaceFormatKHR _desiredSurfaceFormat) {
		uint32_t formatsCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, _surface, &formatsCount, nullptr);
		m_surfaceFormats.resize(formatsCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, _surface, &formatsCount, m_surfaceFormats.data());

		if (m_surfaceFormats.size() == 1 && m_surfaceFormats[0].format == VK_FORMAT_UNDEFINED) { // no restriction
			m_surfaceFormat = _desiredSurfaceFormat;
			return;
		}

		for (int formatIndex = 0; formatIndex < m_surfaceFormats.size(); formatIndex++) {
			if (m_surfaceFormats[formatIndex].colorSpace == _desiredSurfaceFormat.colorSpace &&
				m_surfaceFormats[formatIndex].format == _desiredSurfaceFormat.format) {
				m_surfaceFormat = _desiredSurfaceFormat;
				return;
			}
		}

		for (int formatIndex = 0; formatIndex < m_surfaceFormats.size(); formatIndex++) {
			if (m_surfaceFormats[formatIndex].format == _desiredSurfaceFormat.format) {
				m_surfaceFormat = m_surfaceFormats[formatIndex];
				return;
			}
		}

		m_surfaceFormat = m_surfaceFormats[0];
	}

};