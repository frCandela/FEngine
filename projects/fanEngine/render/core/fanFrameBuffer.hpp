#pragma once

#include "glfw/glfw3.h"
#include "render/core/fanSwapChain.hpp"

namespace fan
{
    struct Device;
    struct RenderPass;
    struct ImageView;

    //==================================================================================================================================================================================================
    // render target with attachments (color, depth, etc. )
    //==================================================================================================================================================================================================
    struct FrameBuffer
    {
        void CreateForSwapchain( Device& _device,
                                 const size_t _framesCount,
                                 const VkExtent2D _extent,
                                 RenderPass& _renderPass,
                                 ImageView* _swapchainImageViews );
        void Create( Device& _device,
                     const size_t _framesCount,
                     const VkExtent2D _extent,
                     RenderPass& _renderPass,
                     const VkImageView* _attachments,
                     uint32_t _attCount );
        void Destroy( Device& _device );

        VkFramebuffer mFrameBuffers[SwapChain::sMaxFramesInFlight] = { VK_NULL_HANDLE,
                                                                       VK_NULL_HANDLE,
                                                                       VK_NULL_HANDLE };
    };
}