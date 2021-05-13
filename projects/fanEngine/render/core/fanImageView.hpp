#pragma once

#include "glfw/glfw3.h"

namespace fan
{
    struct Device;

    //==================================================================================================================================================================================================
    // gpu image accessor
    //==================================================================================================================================================================================================
    struct ImageView
    {
        bool Create( Device& _device,
                     VkImage _image,
                     const VkFormat _format,
                     const VkImageAspectFlags _aspectFlags,
                     const VkImageViewType _viewType );
        void Destroy( Device& _device );

        VkImageView mImageView = VK_NULL_HANDLE;
    };
}