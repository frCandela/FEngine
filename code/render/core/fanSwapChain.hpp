#pragma once

#include "render/fanRenderPrecompiled.hpp"

namespace fan
{
	class Device;
	class Image;
	class ImageView;
	class FrameBuffer;

	//================================================================================================================================
	//================================================================================================================================
	class SwapChain
	{
	public:
		SwapChain( Device& _device, VkSurfaceKHR _surface, VkExtent2D _desiredSize );
		~SwapChain();

		void Create( VkSurfaceKHR _surface, VkExtent2D _desiredSize );
		void Resize( VkExtent2D _desiredSize );

		void		StartNextFrame() { m_currentFrame = ( m_currentFrame + 1 ) % MAX_FRAMES_IN_FLIGHT; }
		VkResult	AcquireNextImage();
		bool		PresentImage();

		uint32_t			GetCurrentFrame() const { return m_currentFrame; }
		uint32_t			GetSwapchainImagesCount() const { return m_imagesCount; }
		uint32_t			GetCurrentImageIndex() const { return m_currentImageIndex; }
		VkSemaphore* GetCurrentImageAvailableSemaphore() { return &m_imagesAvailableSemaphores[ m_currentFrame ]; }
		VkSemaphore* GetCurrentRenderFinishedSemaphore() { return &m_renderFinishedSemaphores[ m_currentFrame ]; }
		VkFence* GetCurrentInFlightFence() { return &m_inFlightFences[ m_currentFrame ]; }
		VkSwapchainKHR		GetVkSwapChain() { return m_swapchain; }
		VkSurfaceFormatKHR	GetSurfaceFormat() const { return m_surfaceFormat; }
		VkExtent2D			GetExtent() const { return m_size; }
		std::vector< ImageView* >	GetImageViews();

		const int MAX_FRAMES_IN_FLIGHT = 3;
	private:

		Device& m_device;

		VkSurfaceKHR	m_surface;
		std::vector<VkPresentModeKHR>	m_supportedPresentModes;
		std::vector<VkSurfaceFormatKHR> m_supportedSurfaceFormats;

		std::vector< VkImage >		m_images;
		std::vector< ImageView* >	m_imageViews;

		std::vector<VkSemaphore> m_imagesAvailableSemaphores;
		std::vector<VkSemaphore> m_renderFinishedSemaphores;
		std::vector<VkFence>	 m_inFlightFences;

		uint32_t m_currentImageIndex;
		uint32_t m_currentFrame = 0;

		VkSwapchainKHR				m_swapchain = VK_NULL_HANDLE;
		VkPresentModeKHR			m_presentMode;
		VkSurfaceCapabilitiesKHR	m_surfaceCapabilities;
		uint32_t					m_imagesCount;
		VkExtent2D					m_size;
		VkImageUsageFlags			m_imageUsage;
		VkSurfaceFormatKHR			m_surfaceFormat;

		void SetDesiredPresentMode( const VkPresentModeKHR _desiredPresentMode );
		void SetNumberOfImages();
		void SetImagesSize( const VkExtent2D _desiredSize );
		void SetImagesUsage();
		void SetDesiredSurfaceFormat( VkSurfaceFormatKHR _desiredSurfaceFormat );
		void CreateSwapChain();
		void CreateSemaphores();
		void CreateImageViews();
		void DestroyImageViews();
	};
}