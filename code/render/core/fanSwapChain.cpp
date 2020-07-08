#include "render/core/fanSwapChain.hpp"

#include "core/fanDebug.hpp"
#include "render/core/fanDevice.hpp"
#include "render/core/fanImage.hpp"
#include "render/core/fanFrameBuffer.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void SwapChain::Create( Device& _device, VkSurfaceKHR _surface, VkExtent2D _desiredSize )
	{
		assert( surface == VK_NULL_HANDLE );
		assert( swapchain == VK_NULL_HANDLE );

		surface = _surface;
		CreateSwapChain( _device, _desiredSize );
		CreateSemaphores( _device );
		CreateImageViews( _device );
	}

	//================================================================================================================================
	//================================================================================================================================
	void SwapChain::Resize( Device& _device, VkExtent2D _desiredSize )
	{
		currentFrame = 0;
		CreateSwapChain( _device, _desiredSize );
		for( uint32_t imageIndex = 0; imageIndex < imagesCount; imageIndex++ )
		{
			imageViews[imageIndex].Destroy( _device );
		}
		CreateImageViews( _device );
	}

	//================================================================================================================================
	//================================================================================================================================
	void SwapChain::Destroy( Device& _device )
	{
		for( uint32_t i = 0; i < imagesCount; i++ )
		{
			imageViews[i].Destroy( _device );
		}

		for( int i = 0; i < s_maxFramesInFlight; i++ )
		{
			vkDestroySemaphore( _device.vkDevice, imagesAvailableSemaphores[i], nullptr );
			vkDestroySemaphore( _device.vkDevice, renderFinishedSemaphores[i], nullptr );
			vkDestroyFence( _device.vkDevice, inFlightFences[i], nullptr );
			imagesAvailableSemaphores[i] = VK_NULL_HANDLE;
			renderFinishedSemaphores[i] = VK_NULL_HANDLE;
			inFlightFences[i]			= VK_NULL_HANDLE;
		}

		vkDestroySwapchainKHR( _device.vkDevice, swapchain, nullptr );
		swapchain = VK_NULL_HANDLE;
	}

	//================================================================================================================================
	//================================================================================================================================
	VkResult SwapChain::AcquireNextImage( Device& _device )
	{
		return vkAcquireNextImageKHR( _device.vkDevice, swapchain, std::numeric_limits<uint64_t>::max(), imagesAvailableSemaphores[ currentFrame ], VK_NULL_HANDLE, &currentImageIndex );
	}

	//================================================================================================================================
	//================================================================================================================================
	bool SwapChain::PresentImage( Device& _device )
	{
		VkPresentInfoKHR presentInfo;
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext = nullptr;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = GetCurrentRenderFinishedSemaphore();
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &swapchain;
		presentInfo.pImageIndices = &currentImageIndex;
		presentInfo.pResults = nullptr;

		if ( vkQueuePresentKHR( _device.GetGraphicsQueue(), &presentInfo ) != VK_SUCCESS )
		{
			Debug::Warning( "Could not present image to graphics queue" );
			return false;
		}
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	VkPresentModeKHR SwapChain::FindDesiredPresentMode( Device& _device, const VkPresentModeKHR _desiredPresentMode ) const
	{
		std::vector<VkPresentModeKHR>	supportedPresentModes;

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR( _device.vkPhysicalDevice, surface, &presentModeCount, nullptr );
		supportedPresentModes.clear();
		supportedPresentModes.resize( presentModeCount );
		vkGetPhysicalDeviceSurfacePresentModesKHR( _device.vkPhysicalDevice, surface, &presentModeCount, supportedPresentModes.data() );

		for ( int presentModeIndex = 0; presentModeIndex < supportedPresentModes.size(); presentModeIndex++ )
		{
			if ( supportedPresentModes[ presentModeIndex ] == _desiredPresentMode )
			{
				return _desiredPresentMode;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	//================================================================================================================================
	//================================================================================================================================
	uint32_t SwapChain::FindDesiredNumberOfImages( const VkSurfaceCapabilitiesKHR _surfaceCapabilities, const uint32_t _desiredCount ) const
	{
		uint32_t count = std::max( _desiredCount, _surfaceCapabilities.minImageCount );
		if( ( _surfaceCapabilities.maxImageCount > 0 ) && ( count > _surfaceCapabilities.maxImageCount ) )
		{
			count = _surfaceCapabilities.maxImageCount;
		}
		return count;
	}

	//================================================================================================================================
	//================================================================================================================================
	VkExtent2D SwapChain::FindDesiredImagesSize( const VkSurfaceCapabilitiesKHR _surfaceCapabilities, const VkExtent2D _desiredSize ) const
	{
		if ( _surfaceCapabilities.currentExtent.height == 0xFFFFFFFF )
		{//means the size of images determines the size of the window
			VkExtent2D extentCpy = _desiredSize;
			if ( extentCpy.width > _surfaceCapabilities.maxImageExtent.width )
			{
				extentCpy.width = _surfaceCapabilities.maxImageExtent.width;
			}
			if ( extentCpy.height > _surfaceCapabilities.maxImageExtent.height )
			{
				extentCpy.height = _surfaceCapabilities.maxImageExtent.height;
			}
			return extentCpy;
		}
		else
		{
			return _surfaceCapabilities.currentExtent;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	VkImageUsageFlags SwapChain::FindDesiredImageUsage( const VkSurfaceCapabilitiesKHR _surfaceCapabilities, VkImageUsageFlagBits _desiredBits ) const
	{
		VkImageUsageFlags imageUsage = 0;
		if ( _surfaceCapabilities.supportedUsageFlags & _desiredBits )
		{
			imageUsage |= _desiredBits;
		}
		return imageUsage;
	}

	//================================================================================================================================
	//================================================================================================================================
	VkSurfaceFormatKHR SwapChain::FindDesiredSurfaceFormat( Device& _device, VkSurfaceFormatKHR _desiredSurfaceFormat ) const
	{
		uint32_t formatsCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR( _device.vkPhysicalDevice, surface, &formatsCount, nullptr );
		std::vector<VkSurfaceFormatKHR> supportedSurfaceFormats;
		supportedSurfaceFormats.resize( formatsCount );
		vkGetPhysicalDeviceSurfaceFormatsKHR( _device.vkPhysicalDevice, surface, &formatsCount, supportedSurfaceFormats.data() );

		if ( supportedSurfaceFormats.size() == 1 && supportedSurfaceFormats[ 0 ].format == VK_FORMAT_UNDEFINED )
		{
			return _desiredSurfaceFormat;
		}

		for ( int formatIndex = 0; formatIndex < supportedSurfaceFormats.size(); formatIndex++ )
		{
			if ( supportedSurfaceFormats[ formatIndex ].colorSpace == _desiredSurfaceFormat.colorSpace &&
				 supportedSurfaceFormats[ formatIndex ].format == _desiredSurfaceFormat.format )
			{
				return _desiredSurfaceFormat;
			}
		}

		for ( int formatIndex = 0; formatIndex < supportedSurfaceFormats.size(); formatIndex++ )
		{
			if ( supportedSurfaceFormats[ formatIndex ].format == _desiredSurfaceFormat.format )
			{
				return supportedSurfaceFormats[ formatIndex ];
			}
		}

		return supportedSurfaceFormats[ 0 ];
	}

	//================================================================================================================================
	//================================================================================================================================
	void SwapChain::CreateSwapChain( Device& _device, VkExtent2D _desiredSize )
	{
		VkSurfaceCapabilitiesKHR surfaceCapabilities;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR( _device.vkPhysicalDevice, surface, &surfaceCapabilities );

		imagesCount						   = FindDesiredNumberOfImages( surfaceCapabilities, s_maxFramesInFlight );
		extent							   = FindDesiredImagesSize( surfaceCapabilities, _desiredSize );
		surfaceFormat					   = FindDesiredSurfaceFormat( _device, { VK_FORMAT_R8G8B8A8_UNORM , VK_COLOR_SPACE_SRGB_NONLINEAR_KHR } );
		const VkPresentModeKHR presentMode = FindDesiredPresentMode( _device, VK_PRESENT_MODE_MAILBOX_KHR );
		const VkImageUsageFlags imageUsage = FindDesiredImageUsage( surfaceCapabilities, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT );

		VkSwapchainKHR oldSwapchain = swapchain;

		VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
		swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchainCreateInfo.pNext = nullptr;
		swapchainCreateInfo.flags = 0;
		swapchainCreateInfo.surface = surface;
		swapchainCreateInfo.minImageCount = imagesCount;
		swapchainCreateInfo.imageFormat = surfaceFormat.format;
		swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
		swapchainCreateInfo.imageExtent = extent;
		swapchainCreateInfo.imageArrayLayers = 1;
		swapchainCreateInfo.imageUsage = imageUsage;
		swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapchainCreateInfo.queueFamilyIndexCount = 0;
		swapchainCreateInfo.pQueueFamilyIndices = nullptr;
		swapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapchainCreateInfo.presentMode = presentMode;
		swapchainCreateInfo.clipped = VK_TRUE;
		swapchainCreateInfo.oldSwapchain = oldSwapchain;

		vkCreateSwapchainKHR( _device.vkDevice, &swapchainCreateInfo, nullptr, &swapchain );
		Debug::Get() << Debug::Severity::log << std::hex << "VkSwapchainKHR        " << swapchain << std::dec << Debug::Endl();

		if ( oldSwapchain != VK_NULL_HANDLE )
		{
			vkDestroySwapchainKHR( _device.vkDevice, oldSwapchain, nullptr );
		}

		uint32_t count;
		vkGetSwapchainImagesKHR( _device.vkDevice, swapchain, &count, nullptr );
		assert( count == imagesCount );
		if( vkGetSwapchainImagesKHR( _device.vkDevice, swapchain, &count, images ) == VK_INCOMPLETE )
		{
			Debug::Error() << "vkGetSwapchainImagesKHR failed." << Debug::Endl();
		}

		for ( uint32_t imageIndex = 0; imageIndex < imagesCount; imageIndex++ )
		{
			Debug::Log() << std::hex << "VkImage swapchain     " << images[ imageIndex ] << std::dec << Debug::Endl();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void SwapChain::CreateSemaphores( Device& _device )
	{
		VkSemaphoreCreateInfo semaphoreCreateInfo;
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		semaphoreCreateInfo.pNext = nullptr;
		semaphoreCreateInfo.flags = 0;

		VkFenceCreateInfo fenceCreateInfo;
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.pNext = nullptr;
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for ( int semaphoreIndex = 0; semaphoreIndex < s_maxFramesInFlight; semaphoreIndex++ )
		{
			vkCreateSemaphore( _device.vkDevice, &semaphoreCreateInfo, nullptr, &imagesAvailableSemaphores[ semaphoreIndex ] );
			vkCreateSemaphore( _device.vkDevice, &semaphoreCreateInfo, nullptr, &renderFinishedSemaphores[ semaphoreIndex ] );
			vkCreateFence( _device.vkDevice, &fenceCreateInfo, nullptr, &inFlightFences[ semaphoreIndex ] );

			Debug::Get() << Debug::Severity::log << std::hex << "VkSemaphore           " << imagesAvailableSemaphores[ semaphoreIndex ] << std::dec << Debug::Endl();
			Debug::Get() << Debug::Severity::log << std::hex << "VkSemaphore           " << renderFinishedSemaphores[ semaphoreIndex ] << std::dec << Debug::Endl();
			Debug::Get() << Debug::Severity::log << std::hex << "VkFence               " << inFlightFences[ semaphoreIndex ] << std::dec << Debug::Endl();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void SwapChain::CreateImageViews( Device& _device )
	{
		for ( uint32_t imageIndex = 0; imageIndex < imagesCount; imageIndex++ )
		{
			assert( imageViews[imageIndex].imageView == VK_NULL_HANDLE );
			imageViews[ imageIndex ].Create( _device, images[ imageIndex ], surfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D );
		}
	}
}