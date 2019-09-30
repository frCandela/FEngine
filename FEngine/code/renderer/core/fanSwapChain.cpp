#include "fanGlobalIncludes.h"

#include "renderer/core/fanSwapChain.h"
#include "renderer/core/fanDevice.h"
#include "renderer/core/fanImage.h"
#include "renderer/core/fanImageView.h"
#include "renderer/core/fanFrameBuffer.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	SwapChain::SwapChain(Device & _device) :
		m_device(_device) {
	}

	//================================================================================================================================
	//================================================================================================================================
	void SwapChain::Create(VkSurfaceKHR _surface, VkExtent2D _desiredSize) {
		m_surface = _surface;

		SetDesiredPresentMode(VK_PRESENT_MODE_MAILBOX_KHR);
		SetNumberOfImages();
		SetImagesSize(_desiredSize);
		SetImagesUsage();
		SetDesiredSurfaceFormat({ VK_FORMAT_R8G8B8A8_UNORM , VK_COLOR_SPACE_SRGB_NONLINEAR_KHR });
		CreateSwapChain();
		CreateSemaphores();
		CreateImageViews();
	}

	//================================================================================================================================
	//================================================================================================================================
	void SwapChain::Resize(VkExtent2D _desiredSize) {
		m_currentFrame = 0;

		SetNumberOfImages();
		SetImagesSize(_desiredSize);
		CreateSwapChain();
		for ( int imageIndex = 0; imageIndex < m_imageViews.size(); imageIndex++ ) {
			m_imageViews[imageIndex]->SetImage( m_images[imageIndex] );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	SwapChain::~SwapChain() {
		DestroyImageViews();

		for (int semaphoreIndex = 0; semaphoreIndex < MAX_FRAMES_IN_FLIGHT; semaphoreIndex++) {
			vkDestroySemaphore(m_device.vkDevice, m_imagesAvailableSemaphores[semaphoreIndex], nullptr);
			vkDestroySemaphore(m_device.vkDevice, m_renderFinishedSemaphores[semaphoreIndex], nullptr);
			vkDestroyFence(m_device.vkDevice, m_inFlightFences[semaphoreIndex], nullptr);
		}
		m_imagesAvailableSemaphores.clear();

		vkDestroySwapchainKHR(m_device.vkDevice, m_swapchain, nullptr);
		m_swapchain = VK_NULL_HANDLE;
	}

	//================================================================================================================================
	//================================================================================================================================
	std::vector< ImageView * > SwapChain::GetImageViews() {
		return m_imageViews;
	}

	//================================================================================================================================
	//================================================================================================================================
	VkResult SwapChain::AcquireNextImage() {
		return vkAcquireNextImageKHR(m_device.vkDevice, m_swapchain, std::numeric_limits<uint64_t>::max(), m_imagesAvailableSemaphores[m_currentFrame], VK_NULL_HANDLE, &m_currentImageIndex);
	}

	//================================================================================================================================
	//================================================================================================================================
	bool SwapChain::PresentImage() {
		VkPresentInfoKHR presentInfo;
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext = nullptr;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = GetCurrentRenderFinishedSemaphore();
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &m_swapchain;
		presentInfo.pImageIndices = &m_currentImageIndex;
		presentInfo.pResults = nullptr;

		if (vkQueuePresentKHR(m_device.GetGraphicsQueue(), &presentInfo) != VK_SUCCESS) {
			Debug::Warning("Could not present image to graphics queue");
			return false;
		}
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void SwapChain::SetDesiredPresentMode(const VkPresentModeKHR _desiredPresentMode) {
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(m_device.vkPhysicalDevice, m_surface, &presentModeCount, nullptr);
		m_supportedPresentModes.clear();
		m_supportedPresentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(m_device.vkPhysicalDevice, m_surface, &presentModeCount, m_supportedPresentModes.data());

		for (int presentModeIndex = 0; presentModeIndex < m_supportedPresentModes.size(); presentModeIndex++)
		{
			if (m_supportedPresentModes[presentModeIndex] == _desiredPresentMode) {
				m_presentMode = _desiredPresentMode;
				return;
			}
		}

		m_presentMode = VK_PRESENT_MODE_FIFO_KHR;
	}

	//================================================================================================================================
	//================================================================================================================================
	void SwapChain::SetNumberOfImages() {
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_device.vkPhysicalDevice, m_surface, &m_surfaceCapabilities);
		m_imagesCount = m_surfaceCapabilities.minImageCount + 1;
		if ((m_surfaceCapabilities.maxImageCount > 0) && (m_imagesCount > m_surfaceCapabilities.maxImageCount)) {
			m_imagesCount = m_surfaceCapabilities.maxImageCount;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void SwapChain::SetImagesSize(const VkExtent2D _desiredSize) {
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

	//================================================================================================================================
	//================================================================================================================================
	void SwapChain::SetImagesUsage() {
		m_imageUsage = 0;
		if (m_surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) {
			m_imageUsage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void SwapChain::SetDesiredSurfaceFormat(VkSurfaceFormatKHR _desiredSurfaceFormat) {
		uint32_t formatsCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(m_device.vkPhysicalDevice, m_surface, &formatsCount, nullptr);
		m_supportedSurfaceFormats.resize(formatsCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(m_device.vkPhysicalDevice, m_surface, &formatsCount, m_supportedSurfaceFormats.data());

		if (m_supportedSurfaceFormats.size() == 1 && m_supportedSurfaceFormats[0].format == VK_FORMAT_UNDEFINED) { // no restriction
			m_surfaceFormat = _desiredSurfaceFormat;
			return;
		}

		for (int formatIndex = 0; formatIndex < m_supportedSurfaceFormats.size(); formatIndex++) {
			if (m_supportedSurfaceFormats[formatIndex].colorSpace == _desiredSurfaceFormat.colorSpace &&
				m_supportedSurfaceFormats[formatIndex].format == _desiredSurfaceFormat.format) {
				m_surfaceFormat = _desiredSurfaceFormat;
				return;
			}
		}

		for (int formatIndex = 0; formatIndex < m_supportedSurfaceFormats.size(); formatIndex++) {
			if (m_supportedSurfaceFormats[formatIndex].format == _desiredSurfaceFormat.format) {
				m_surfaceFormat = m_supportedSurfaceFormats[formatIndex];
				return;
			}
		}

		m_surfaceFormat = m_supportedSurfaceFormats[0];
	}

	//================================================================================================================================
	//================================================================================================================================
	void SwapChain::CreateSwapChain() {
		VkSwapchainKHR oldSwapchain = m_swapchain;

		VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
		swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchainCreateInfo.pNext = nullptr;
		swapchainCreateInfo.flags = 0;
		swapchainCreateInfo.surface = m_surface;
		swapchainCreateInfo.minImageCount = m_imagesCount;
		swapchainCreateInfo.imageFormat = m_surfaceFormat.format;
		swapchainCreateInfo.imageColorSpace = m_surfaceFormat.colorSpace;
		swapchainCreateInfo.imageExtent = m_size;
		swapchainCreateInfo.imageArrayLayers = 1;
		swapchainCreateInfo.imageUsage = m_imageUsage;
		swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapchainCreateInfo.queueFamilyIndexCount = 0;
		swapchainCreateInfo.pQueueFamilyIndices = nullptr;
		swapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapchainCreateInfo.presentMode = m_presentMode;
		swapchainCreateInfo.clipped = VK_TRUE;
		swapchainCreateInfo.oldSwapchain = oldSwapchain;

		vkCreateSwapchainKHR(m_device.vkDevice, &swapchainCreateInfo, nullptr, &m_swapchain);
		Debug::Get() << Debug::Severity::log << std::hex << "VkSwapchainKHR        " << m_swapchain << std::dec << Debug::Endl();

		if (oldSwapchain != VK_NULL_HANDLE) {
			vkDestroySwapchainKHR(m_device.vkDevice, oldSwapchain, nullptr);
		}
		
		uint32_t imagesCount;
		vkGetSwapchainImagesKHR(m_device.vkDevice, m_swapchain, &imagesCount, nullptr);
		m_images.resize(imagesCount);
		vkGetSwapchainImagesKHR(m_device.vkDevice, m_swapchain, &imagesCount, m_images.data());

		for (uint32_t imageIndex = 0; imageIndex < imagesCount; imageIndex++) {
			Debug::Get() << Debug::Severity::log << std::hex << "VkImage swapchain     " << m_images[imageIndex] << std::dec << Debug::Endl();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void SwapChain::CreateSemaphores() {

		m_imagesAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		m_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

		VkSemaphoreCreateInfo semaphoreCreateInfo;
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		semaphoreCreateInfo.pNext = nullptr;
		semaphoreCreateInfo.flags = 0;

		VkFenceCreateInfo fenceCreateInfo;
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.pNext = nullptr;
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (int semaphoreIndex = 0; semaphoreIndex < MAX_FRAMES_IN_FLIGHT; semaphoreIndex++) {
			vkCreateSemaphore(m_device.vkDevice, &semaphoreCreateInfo, nullptr, &m_imagesAvailableSemaphores[semaphoreIndex]);
			vkCreateSemaphore(m_device.vkDevice, &semaphoreCreateInfo, nullptr, &m_renderFinishedSemaphores[semaphoreIndex]);
			vkCreateFence(m_device.vkDevice, &fenceCreateInfo, nullptr, &m_inFlightFences[semaphoreIndex]);

			Debug::Get() << Debug::Severity::log << std::hex << "VkSemaphore           " << m_imagesAvailableSemaphores[semaphoreIndex] << std::dec << Debug::Endl();
			Debug::Get() << Debug::Severity::log << std::hex << "VkSemaphore           " << m_renderFinishedSemaphores[semaphoreIndex] << std::dec << Debug::Endl();
			Debug::Get() << Debug::Severity::log << std::hex << "VkFence               " << m_inFlightFences[semaphoreIndex] << std::dec << Debug::Endl();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void SwapChain::CreateImageViews() {
		m_imageViews.resize(m_images.size());
		for (int imageIndex = 0; imageIndex < m_imageViews.size(); imageIndex++) {
			m_imageViews[imageIndex] = new ImageView(m_device);
			m_imageViews[imageIndex]->Create(m_images[imageIndex], m_surfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D);
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void SwapChain::DestroyImageViews() {
		for (int imageIndex = 0; imageIndex < m_imageViews.size(); imageIndex++) {
			delete m_imageViews[imageIndex];
		}
		m_imageViews.clear();
	}
}