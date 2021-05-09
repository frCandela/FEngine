#pragma once

#include "glfw/glfw3.h"
#include <vector>

namespace fan
{
    struct Device;

    //==================================================================================================================================================================================================
    // Creates an image descriptor or a combined image sampler
    //==================================================================================================================================================================================================
    struct DescriptorImages
    {
        void Create( Device& _device,
                     VkImageView* _pViews,
                     const uint32_t _count,
                     VkSampler* _pSamplers = nullptr );
        void Destroy( Device& _device );
        void UpdateDescriptorSet( Device& _device,
                                  const uint32_t _index,
                                  VkImageView _imageView,
                                  VkSampler _sampler = VK_NULL_HANDLE );

        VkDescriptorSetLayout        mDescriptorSetLayout = VK_NULL_HANDLE;
        VkDescriptorPool             mDescriptorPool      = VK_NULL_HANDLE;
        std::vector<VkDescriptorSet> mDescriptorSets;
    };
}