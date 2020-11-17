#include "render/core/descriptors/fanDescriptorImages.hpp"

#include "core/fanDebug.hpp"
#include "render/core/fanDevice.hpp"

namespace fan
{
	//========================================================================================================
    //========================================================================================================
    void DescriptorImages::Create( Device& _device,
                                   VkImageView* _pViews,
                                   const uint32_t _count,
                                   VkSampler* _pSamplers )
	{
	    if(  _count == 0) { return; }

        fanAssert( mDescriptorPool == VK_NULL_HANDLE );
        fanAssert( mDescriptorSetLayout == VK_NULL_HANDLE );

        VkDescriptorType descriptorType = ( _pSamplers == nullptr ?
                VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE :
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER );

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

            vkCreateDescriptorSetLayout( _device.mDevice,
                                         &descriptorSetLayoutCreateInfo,
                                         nullptr,
                                         &mDescriptorSetLayout );

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
		}

		{
			std::vector<VkDescriptorImageInfo> imageInfo( _count );
			std::vector<VkWriteDescriptorSet> writeDescriptorSets( _count );
			for( uint32_t i = 0; i < _count; i++ )
			{
				imageInfo[i].sampler = _pSamplers == nullptr ? VK_NULL_HANDLE : _pSamplers[i];
				imageInfo[i].imageView = _pViews[i];
				imageInfo[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

				writeDescriptorSets[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				writeDescriptorSets[i].dstSet = mDescriptorSets[i];
				writeDescriptorSets[i].descriptorType = descriptorType;
				writeDescriptorSets[i].dstBinding = 0;
				writeDescriptorSets[i].pImageInfo = &imageInfo[i];
				writeDescriptorSets[i].descriptorCount = 1;
			}
            vkUpdateDescriptorSets( _device.mDevice,
                                    static_cast<uint32_t>( writeDescriptorSets.size() ),
                                    writeDescriptorSets.data(),
                                    0,
                                    nullptr );
		}
	}

	//========================================================================================================
	//========================================================================================================
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

	//========================================================================================================
	//========================================================================================================
    void DescriptorImages::UpdateDescriptorSet( Device& _device,
                                                const uint32_t _index,
                                                VkImageView _imageView,
                                                VkSampler _sampler )
    {
		VkDescriptorImageInfo iconsDescriptorImageInfo{};
		iconsDescriptorImageInfo.sampler = _sampler;
		iconsDescriptorImageInfo.imageView = _imageView;
		iconsDescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkWriteDescriptorSet writeDescriptorSetIcons{};
		writeDescriptorSetIcons.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSetIcons.dstSet = mDescriptorSets[_index];
        writeDescriptorSetIcons.descriptorType = _sampler == VK_NULL_HANDLE ?
                VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE :
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeDescriptorSetIcons.dstBinding = 0;
		writeDescriptorSetIcons.pImageInfo = &iconsDescriptorImageInfo;
		writeDescriptorSetIcons.descriptorCount = 1;

		vkUpdateDescriptorSets( _device.mDevice, 1, &writeDescriptorSetIcons, 0, nullptr );
	}
}