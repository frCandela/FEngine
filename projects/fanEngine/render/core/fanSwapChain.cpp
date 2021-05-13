#include "render/core/fanSwapChain.hpp"

#include "core/fanDebug.hpp"
#include "render/core/fanDevice.hpp"
#include "render/core/fanImage.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void SwapChain::Create( Device& _device, VkSurfaceKHR _surface, VkExtent2D _desiredSize )
    {
        fanAssert( mSwapchain == VK_NULL_HANDLE );

        CreateSwapChain( _device, _surface, _desiredSize );
        CreateSemaphores( _device );
        CreateImageViews( _device );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void SwapChain::Resize( Device& _device, VkSurfaceKHR _surface, VkExtent2D _desiredSize )
    {
        mCurrentFrame = 0;
        CreateSwapChain( _device, _surface, _desiredSize );
        for( uint32_t imageIndex = 0; imageIndex < mImagesCount; imageIndex++ )
        {
            mImageViews[imageIndex].Destroy( _device );
        }
        CreateImageViews( _device );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void SwapChain::Destroy( Device& _device )
    {
        for( uint32_t i = 0; i < mImagesCount; i++ )
        {
            mImageViews[i].Destroy( _device );
        }

        for( int i = 0; i < (int)sMaxFramesInFlight; i++ )
        {
            vkDestroySemaphore( _device.mDevice, mImagesAvailableSemaphores[i], nullptr );
            vkDestroySemaphore( _device.mDevice, mRenderFinishedSemaphores[i], nullptr );
            vkDestroyFence( _device.mDevice, mInFlightFences[i], nullptr );
            mImagesAvailableSemaphores[i] = VK_NULL_HANDLE;
            mRenderFinishedSemaphores[i]  = VK_NULL_HANDLE;
            mInFlightFences[i]            = VK_NULL_HANDLE;
        }

        vkDestroySwapchainKHR( _device.mDevice, mSwapchain, nullptr );
        mSwapchain = VK_NULL_HANDLE;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    VkResult SwapChain::AcquireNextImage( Device& _device )
    {
        return vkAcquireNextImageKHR( _device.mDevice,
                                      mSwapchain,
                                      std::numeric_limits<uint64_t>::max(),
                                      mImagesAvailableSemaphores[mCurrentFrame],
                                      VK_NULL_HANDLE, &mCurrentImageIndex );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool SwapChain::PresentImage( Device& _device )
    {
        VkPresentInfoKHR presentInfo;
        presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.pNext              = nullptr;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores    = GetCurrentRenderFinishedSemaphore();
        presentInfo.swapchainCount     = 1;
        presentInfo.pSwapchains        = &mSwapchain;
        presentInfo.pImageIndices      = &mCurrentImageIndex;
        presentInfo.pResults           = nullptr;

        if( vkQueuePresentKHR( _device.mGraphicsQueue, &presentInfo ) != VK_SUCCESS )
        {
            Debug::Warning( "Could not present image to graphics queue" );
            return false;
        }
        return true;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    VkPresentModeKHR SwapChain::FindDesiredPresentMode( Device& _device,
                                                        VkSurfaceKHR _surface,
                                                        const VkPresentModeKHR _desiredPresentMode ) const
    {
        std::vector<VkPresentModeKHR> supportedPresentModes;

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR( _device.mPhysicalDevice,
                                                   _surface,
                                                   &presentModeCount, nullptr );
        supportedPresentModes.clear();
        supportedPresentModes.resize( presentModeCount );
        vkGetPhysicalDeviceSurfacePresentModesKHR( _device.mPhysicalDevice,
                                                   _surface,
                                                   &presentModeCount,
                                                   supportedPresentModes.data() );

        for( int i = 0; i < (int)supportedPresentModes.size(); i++ )
        {
            if( supportedPresentModes[i] == _desiredPresentMode )
            {
                return _desiredPresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    uint32_t SwapChain::FindDesiredNumberOfImages( const VkSurfaceCapabilitiesKHR _surfaceCapabilities,
                                                   const uint32_t _desiredCount ) const
    {
        uint32_t count = std::max( _desiredCount, _surfaceCapabilities.minImageCount );
        if( ( _surfaceCapabilities.maxImageCount > 0 ) && ( count > _surfaceCapabilities.maxImageCount ) )
        {
            count = _surfaceCapabilities.maxImageCount;
        }
        return count;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    VkExtent2D SwapChain::FindDesiredImagesSize( const VkSurfaceCapabilitiesKHR _surfaceCapabilities,
                                                 const VkExtent2D _desiredSize ) const
    {
        if( _surfaceCapabilities.currentExtent.height == 0xFFFFFFFF )
        {//means the size of images determines the size of the window
            VkExtent2D extentCpy = _desiredSize;
            if( extentCpy.width > _surfaceCapabilities.maxImageExtent.width )
            {
                extentCpy.width = _surfaceCapabilities.maxImageExtent.width;
            }
            if( extentCpy.height > _surfaceCapabilities.maxImageExtent.height )
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

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    VkImageUsageFlags SwapChain::FindDesiredImageUsage( const VkSurfaceCapabilitiesKHR _surfaceCapabilities,
                                                        VkImageUsageFlagBits _desiredBits ) const
    {
        VkImageUsageFlags imageUsage = 0;
        if( _surfaceCapabilities.supportedUsageFlags & _desiredBits )
        {
            imageUsage |= _desiredBits;
        }
        return imageUsage;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    VkSurfaceFormatKHR SwapChain::FindDesiredSurfaceFormat( Device& _device,
                                                            VkSurfaceKHR _surface,
                                                            VkSurfaceFormatKHR _desiredSurfaceFormat ) const
    {
        uint32_t formatsCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR( _device.mPhysicalDevice, _surface, &formatsCount, nullptr );
        std::vector<VkSurfaceFormatKHR> supportedFormats;
        supportedFormats.resize( formatsCount );
        vkGetPhysicalDeviceSurfaceFormatsKHR( _device.mPhysicalDevice,
                                              _surface,
                                              &formatsCount, supportedFormats.data() );

        if( supportedFormats.size() == 1 && supportedFormats[0].format == VK_FORMAT_UNDEFINED )
        {
            return _desiredSurfaceFormat;
        }

        for( int formatIndex = 0; formatIndex < (int)supportedFormats.size(); formatIndex++ )
        {
            if( supportedFormats[formatIndex].colorSpace == _desiredSurfaceFormat.colorSpace &&
                supportedFormats[formatIndex].format == _desiredSurfaceFormat.format )
            {
                return _desiredSurfaceFormat;
            }
        }

        for( int formatIndex = 0; formatIndex < (int)supportedFormats.size(); formatIndex++ )
        {
            if( supportedFormats[formatIndex].format == _desiredSurfaceFormat.format )
            {
                return supportedFormats[formatIndex];
            }
        }

        return supportedFormats[0];
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void SwapChain::CreateSwapChain( Device& _device, VkSurfaceKHR _surface, VkExtent2D _desiredSize )
    {
        VkSurfaceCapabilitiesKHR surfaceCapabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR( _device.mPhysicalDevice, _surface, &surfaceCapabilities );

        mImagesCount   = FindDesiredNumberOfImages( surfaceCapabilities, sMaxFramesInFlight );
        mExtent        = FindDesiredImagesSize( surfaceCapabilities, _desiredSize );
        mSurfaceFormat = FindDesiredSurfaceFormat( _device,
                                                   _surface,
                                                   { VK_FORMAT_R8G8B8A8_UNORM,
                                                     VK_COLOR_SPACE_SRGB_NONLINEAR_KHR } );
        const VkPresentModeKHR  presentMode = FindDesiredPresentMode( _device,
                                                                      _surface,
                                                                      VK_PRESENT_MODE_MAILBOX_KHR );
        const VkImageUsageFlags imageUsage  = FindDesiredImageUsage( surfaceCapabilities,
                                                                     VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT );

        VkSwapchainKHR oldSwapchain = mSwapchain;

        VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
        swapchainCreateInfo.sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfo.pNext                 = nullptr;
        swapchainCreateInfo.flags                 = 0;
        swapchainCreateInfo.surface               = _surface;
        swapchainCreateInfo.minImageCount         = mImagesCount;
        swapchainCreateInfo.imageFormat           = mSurfaceFormat.format;
        swapchainCreateInfo.imageColorSpace       = mSurfaceFormat.colorSpace;
        swapchainCreateInfo.imageExtent           = mExtent;
        swapchainCreateInfo.imageArrayLayers      = 1;
        swapchainCreateInfo.imageUsage            = imageUsage;
        swapchainCreateInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCreateInfo.queueFamilyIndexCount = 0;
        swapchainCreateInfo.pQueueFamilyIndices   = nullptr;
        swapchainCreateInfo.preTransform          = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        swapchainCreateInfo.compositeAlpha        = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainCreateInfo.presentMode           = presentMode;
        swapchainCreateInfo.clipped               = VK_TRUE;
        swapchainCreateInfo.oldSwapchain          = oldSwapchain;

        vkCreateSwapchainKHR( _device.mDevice, &swapchainCreateInfo, nullptr, &mSwapchain );
        Debug::Log() << std::hex << "VkSwapchainKHR        " << mSwapchain << std::dec << Debug::Endl();

        if( oldSwapchain != VK_NULL_HANDLE )
        {
            vkDestroySwapchainKHR( _device.mDevice, oldSwapchain, nullptr );
        }

        uint32_t count;
        vkGetSwapchainImagesKHR( _device.mDevice, mSwapchain, &count, nullptr );
        fanAssert( count == mImagesCount );
        if( vkGetSwapchainImagesKHR( _device.mDevice, mSwapchain, &count, mImages ) == VK_INCOMPLETE )
        {
            Debug::Error() << "vkGetSwapchainImagesKHR failed." << Debug::Endl();
        }

        for( uint32_t imageIndex = 0; imageIndex < mImagesCount; imageIndex++ )
        {
            Debug::Log() << std::hex << "VkImage swapchain     " << mImages[imageIndex]
                    << std::dec << Debug::Endl();
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
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

        for( int semaphoreIndex = 0; semaphoreIndex < (int)sMaxFramesInFlight; semaphoreIndex++ )
        {
            vkCreateSemaphore( _device.mDevice, &semaphoreCreateInfo, nullptr, &mImagesAvailableSemaphores[semaphoreIndex] );
            vkCreateSemaphore( _device.mDevice, &semaphoreCreateInfo, nullptr, &mRenderFinishedSemaphores[semaphoreIndex] );
            vkCreateFence( _device.mDevice, &fenceCreateInfo, nullptr, &mInFlightFences[semaphoreIndex] );

            Debug::Get() << Debug::Severity::log << std::hex << "VkSemaphore           "
                    << mImagesAvailableSemaphores[semaphoreIndex] << std::dec << Debug::Endl();

            Debug::Get() << Debug::Severity::log << std::hex << "VkSemaphore           "
                    << mRenderFinishedSemaphores[semaphoreIndex] << std::dec << Debug::Endl();

            Debug::Get() << Debug::Severity::log << std::hex << "VkFence               "
                    << mInFlightFences[semaphoreIndex] << std::dec << Debug::Endl();
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void SwapChain::CreateImageViews( Device& _device )
    {
        for( uint32_t imageIndex = 0; imageIndex < mImagesCount; imageIndex++ )
        {
            fanAssert( mImageViews[imageIndex].mImageView == VK_NULL_HANDLE );
            mImageViews[imageIndex].Create( _device,
                                            mImages[imageIndex],
                                            mSurfaceFormat.format,
                                            VK_IMAGE_ASPECT_COLOR_BIT,
                                            VK_IMAGE_VIEW_TYPE_2D );
        }
    }
}