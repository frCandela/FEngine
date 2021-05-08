#include "render/core/descriptors/fanDescriptorSampler.hpp"
#include "render/core/fanDevice.hpp"
#include "render/resources/fanTexture.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void DescriptorSampler::Create( Device& _device, VkSampler _sampler ) 
	{
        fanAssert( _sampler != VK_NULL_HANDLE );

		// Descriptor pool
		{
			VkDescriptorPoolSize descriptorPoolSize = {};
			descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_SAMPLER;
			descriptorPoolSize.descriptorCount = 1;

			std::vector<VkDescriptorPoolSize> poolSizes = { descriptorPoolSize };
			VkDescriptorPoolCreateInfo descriptorPoolInfo = {};

			descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			descriptorPoolInfo.poolSizeCount = static_cast< uint32_t >( poolSizes.size() );
			descriptorPoolInfo.pPoolSizes = poolSizes.data();
			descriptorPoolInfo.maxSets = 1;
			vkCreateDescriptorPool( _device.mDevice, &descriptorPoolInfo, nullptr, &mDescriptorPool );
		}

		// Descriptor set layout
		{
			VkDescriptorSetLayoutBinding setLayoutBinding{};
			setLayoutBinding.binding = 0;
			setLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
			setLayoutBinding.descriptorCount = 1;
			setLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
			setLayoutBinding.pImmutableSamplers = &_sampler;

			std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings = { setLayoutBinding };

			VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
			descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			descriptorSetLayoutCreateInfo.pBindings = setLayoutBindings.data();
			descriptorSetLayoutCreateInfo.bindingCount = static_cast< uint32_t >( setLayoutBindings.size() );

            vkCreateDescriptorSetLayout( _device.mDevice,
                                         &descriptorSetLayoutCreateInfo,
                                         nullptr,
                                         &mDescriptorSetLayout );

			// Descriptor set
			std::vector<VkDescriptorSetLayout>  layouts = { mDescriptorSetLayout };

			VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
			descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			descriptorSetAllocateInfo.descriptorPool = mDescriptorPool;
			descriptorSetAllocateInfo.pSetLayouts = layouts.data();
			descriptorSetAllocateInfo.descriptorSetCount = 1;

			vkAllocateDescriptorSets( _device.mDevice, &descriptorSetAllocateInfo, &mDescriptorSet );
		}
	}

	//========================================================================================================
	//========================================================================================================
	void DescriptorSampler::Destroy( Device& _device )
	{
		if ( mDescriptorPool != VK_NULL_HANDLE )
		{
			vkDestroyDescriptorPool( _device.mDevice, mDescriptorPool, nullptr );
			mDescriptorPool = VK_NULL_HANDLE;
		}

		if ( mDescriptorSetLayout != VK_NULL_HANDLE )
		{
			vkDestroyDescriptorSetLayout( _device.mDevice, mDescriptorSetLayout, nullptr );
			mDescriptorSetLayout = VK_NULL_HANDLE;
		}

		mDescriptorSet = VK_NULL_HANDLE;
	}
}