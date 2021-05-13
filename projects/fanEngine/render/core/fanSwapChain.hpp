#pragma once

#include "glfw/glfw3.h"
#include <vector>
#include "render/core/fanImageView.hpp"

namespace fan
{
    struct Device;

    //==================================================================================================================================================================================================
    // Abstraction of the vulkan swapchain
    //==================================================================================================================================================================================================
    struct SwapChain
    {
        void Create( Device& _device, VkSurfaceKHR _surface, VkExtent2D _desiredSize );
        void Destroy( Device& _device );
        void Resize( Device& _device, VkSurfaceKHR _surface, VkExtent2D _desiredSize );

        void StartNextFrame() { mCurrentFrame = ( mCurrentFrame + 1 ) % sMaxFramesInFlight; }
        VkResult AcquireNextImage( Device& _device );
        bool PresentImage( Device& _device );
        VkSemaphore* GetCurrentImageAvailableSemaphore() { return &mImagesAvailableSemaphores[mCurrentFrame]; }
        VkSemaphore* GetCurrentRenderFinishedSemaphore() { return &mRenderFinishedSemaphores[mCurrentFrame]; }
        VkFence* GetCurrentInFlightFence() { return &mInFlightFences[mCurrentFrame]; }

        static const uint32_t sMaxFramesInFlight = 3;

        VkSwapchainKHR     mSwapchain    = VK_NULL_HANDLE;
        uint32_t           mCurrentImageIndex;
        uint32_t           mCurrentFrame = 0;
        uint32_t           mImagesCount;
        VkSurfaceFormatKHR mSurfaceFormat;
        VkExtent2D         mExtent;

        VkImage     mImages[sMaxFramesInFlight]                    = { VK_NULL_HANDLE,
                                                                       VK_NULL_HANDLE,
                                                                       VK_NULL_HANDLE };
        ImageView   mImageViews[sMaxFramesInFlight]                = { VK_NULL_HANDLE,
                                                                       VK_NULL_HANDLE,
                                                                       VK_NULL_HANDLE };
        VkSemaphore mImagesAvailableSemaphores[sMaxFramesInFlight] = { VK_NULL_HANDLE,
                                                                       VK_NULL_HANDLE,
                                                                       VK_NULL_HANDLE };
        VkSemaphore mRenderFinishedSemaphores[sMaxFramesInFlight]  = { VK_NULL_HANDLE,
                                                                       VK_NULL_HANDLE,
                                                                       VK_NULL_HANDLE };
        VkFence     mInFlightFences[sMaxFramesInFlight]            = { VK_NULL_HANDLE,
                                                                       VK_NULL_HANDLE,
                                                                       VK_NULL_HANDLE };

    private:
        VkPresentModeKHR FindDesiredPresentMode( Device& _device,
                                                 VkSurfaceKHR _surface,
                                                 const VkPresentModeKHR _desiredPresentMode ) const;
        VkSurfaceFormatKHR FindDesiredSurfaceFormat( Device& _device,
                                                     VkSurfaceKHR _surface,
                                                     VkSurfaceFormatKHR _desiredSurfaceFormat ) const;
        VkImageUsageFlags FindDesiredImageUsage( const VkSurfaceCapabilitiesKHR _surfaceCapabilities,
                                                 VkImageUsageFlagBits _desiredBits ) const;
        VkExtent2D FindDesiredImagesSize( const VkSurfaceCapabilitiesKHR _surfaceCapabilities,
                                          const VkExtent2D _desiredSize ) const;
        uint32_t FindDesiredNumberOfImages( const VkSurfaceCapabilitiesKHR _surfaceCapabilities,
                                            const uint32_t _desiredCount ) const;

        void CreateSwapChain( Device& _device, VkSurfaceKHR _surface, VkExtent2D _desiredSize );
        void CreateSemaphores( Device& _device );
        void CreateImageViews( Device& _device );
    };
}