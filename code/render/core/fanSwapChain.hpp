#pragma once

#include "glfw/glfw3.h"
#include <vector>
#include "render/core/fanImageView.hpp"

namespace fan
{
	struct Device;
	class FrameBuffer;

	//================================================================================================================================
	// Abstraction of the vulkan swapchain
	//================================================================================================================================
	struct SwapChain
	{
		void Create( Device& _device, VkSurfaceKHR _surface, VkExtent2D _desiredSize );
		void Destroy( Device& _device );
		void Resize( Device& _device, VkExtent2D _desiredSize );

		void			StartNextFrame() { currentFrame = ( currentFrame + 1 ) % s_maxFramesInFlight; }
		VkResult		AcquireNextImage( Device& _device );
		bool			PresentImage( Device& _device );
		VkSemaphore*	GetCurrentImageAvailableSemaphore() { return &imagesAvailableSemaphores[ currentFrame ]; }
		VkSemaphore*	GetCurrentRenderFinishedSemaphore() { return &renderFinishedSemaphores[ currentFrame ]; }
		VkFence*		GetCurrentInFlightFence()			{ return &inFlightFences[ currentFrame ]; }

		static const uint32_t s_maxFramesInFlight = 3;

		VkSurfaceKHR		surface = VK_NULL_HANDLE;
		VkSwapchainKHR		swapchain = VK_NULL_HANDLE;
		uint32_t			currentImageIndex;
		uint32_t			currentFrame = 0;
		uint32_t			imagesCount;
		VkSurfaceFormatKHR	surfaceFormat;
		VkExtent2D			extent;		

		VkImage 	 images						[ s_maxFramesInFlight ] = { VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE };
		ImageView	 imageViews					[ s_maxFramesInFlight ] = { VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE };
		VkSemaphore  imagesAvailableSemaphores	[ s_maxFramesInFlight ] = { VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE };
		VkSemaphore  renderFinishedSemaphores	[ s_maxFramesInFlight ] = { VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE };
		VkFence		 inFlightFences				[ s_maxFramesInFlight ] = { VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE };

	private:
		VkPresentModeKHR	FindDesiredPresentMode( Device& _device, const VkPresentModeKHR _desiredPresentMode ) const;
		VkSurfaceFormatKHR	FindDesiredSurfaceFormat( Device& _device, VkSurfaceFormatKHR _desiredSurfaceFormat ) const;
		VkImageUsageFlags	FindDesiredImageUsage( const VkSurfaceCapabilitiesKHR _surfaceCapabilities, VkImageUsageFlagBits _desiredBits ) const;
		VkExtent2D			FindDesiredImagesSize( const VkSurfaceCapabilitiesKHR _surfaceCapabilities, const VkExtent2D _desiredSize ) const;
		uint32_t			FindDesiredNumberOfImages( const VkSurfaceCapabilitiesKHR _surfaceCapabilities, const uint32_t _desiredCount ) const;

		void				CreateSwapChain( Device& _device, VkExtent2D _desiredSize );
		void				CreateSemaphores( Device& _device );
		void				CreateImageViews( Device& _device );
	};
}