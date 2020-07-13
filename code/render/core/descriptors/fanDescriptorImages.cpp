#include "render/core/descriptors/fanDescriptorImages.hpp"

#include "core/fanDebug.hpp"
#include "render/core/fanDevice.hpp"
#include "render/core/fanTexture.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void DescriptorImages::Create( Device& _device, VkImageView* _pViews, const uint32_t _count, VkSampler _sampler  )
	{
		assert( mDescriptorPool == VK_NULL_HANDLE );
		assert( mDescriptorSetLayout == VK_NULL_HANDLE );

		Debug::Log() << "reserve: " << _count << Debug::Endl();

		VkDescriptorType descriptorType = ( _sampler == VK_NULL_HANDLE ? VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE : VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER );

		// Descriptor pool
		{
			VkDescriptorPoolSize descriptorPoolSize = {};
			descriptorPoolSize.type = descriptorType;
			descriptorPoolSize.descriptorCount = _count;

			std::vector<VkDescriptorPoolSize> poolSizes = { descriptorPoolSize };
			VkDescriptorPoolCreateInfo descriptorPoolInfo = {};

			descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			descriptorPoolInfo.poolSizeCount = static_cast< uint32_t >( poolSizes.size() );
			descriptorPoolInfo.pPoolSizes = poolSizes.data();
			descriptorPoolInfo.maxSets = _count;
			vkCreateDescriptorPool( _device.mDevice, &descriptorPoolInfo, nullptr, &mDescriptorPool );
		}

		// Descriptor set layout
		{
			VkDescriptorSetLayoutBinding setLayoutBinding{};
			setLayoutBinding.descriptorType = descriptorType;
			setLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
			setLayoutBinding.binding = 0;
			setLayoutBinding.descriptorCount = 1;

			std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings = { setLayoutBinding };

			VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
			descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			descriptorSetLayoutCreateInfo.pBindings = setLayoutBindings.data();
			descriptorSetLayoutCreateInfo.bindingCount = static_cast< uint32_t >( setLayoutBindings.size() );

			vkCreateDescriptorSetLayout( _device.mDevice, &descriptorSetLayoutCreateInfo, nullptr, &mDescriptorSetLayout );

			Debug::Log() << "VkDescriptorSetAllocateInfo: " << _count << Debug::Endl();
			// Descriptor set
			std::vector<VkDescriptorSetLayout>  layouts( _count, mDescriptorSetLayout );

			VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
			descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			descriptorSetAllocateInfo.descriptorPool = mDescriptorPool;
			descriptorSetAllocateInfo.pSetLayouts = layouts.data();
			descriptorSetAllocateInfo.descriptorSetCount = _count;

			mDescriptorSets.resize( _count );
			vkAllocateDescriptorSets( _device.mDevice, &descriptorSetAllocateInfo, mDescriptorSets.data() );

			Debug::Log() << " allocated " << _count << Debug::Endl();
		}

		{
			std::vector < VkWriteDescriptorSet> writeDescriptorSets( _count );
			std::vector<VkDescriptorImageInfo> imageInfos( _count );
			for( uint32_t viewIndex = 0; viewIndex < _count; viewIndex++ )
			{
				imageInfos[viewIndex].sampler = _sampler;
				imageInfos[viewIndex].imageView = _pViews[viewIndex];
				imageInfos[viewIndex].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

				writeDescriptorSets[viewIndex].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				writeDescriptorSets[viewIndex].dstSet = mDescriptorSets[viewIndex];
				writeDescriptorSets[viewIndex].descriptorType = descriptorType;
				writeDescriptorSets[viewIndex].dstBinding = 0;
				writeDescriptorSets[viewIndex].pImageInfo = &imageInfos[viewIndex];
				writeDescriptorSets[viewIndex].descriptorCount = 1;
			}
			vkUpdateDescriptorSets( _device.mDevice, static_cast<uint32_t>( writeDescriptorSets.size() ), writeDescriptorSets.data(), 0, nullptr );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void DescriptorImages::Destroy( Device& _device )
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

		mDescriptorSets.clear();
	}
}