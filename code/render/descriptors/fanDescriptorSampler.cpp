#include "render/descriptors/fanDescriptorSampler.hpp"
#include "render/core/fanDevice.hpp"
#include "render/core/fanTexture.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	DescriptorSampler::DescriptorSampler( Device& _device, VkSampler _sampler ) :
		m_device( _device )
	{
		assert( _sampler != VK_NULL_HANDLE );

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
			vkCreateDescriptorPool( m_device.mDevice, &descriptorPoolInfo, nullptr, &m_descriptorPool );
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

			vkCreateDescriptorSetLayout( m_device.mDevice, &descriptorSetLayoutCreateInfo, nullptr, &m_descriptorSetLayout );

			// Descriptor set
			std::vector<VkDescriptorSetLayout>  layouts = { m_descriptorSetLayout };

			VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
			descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			descriptorSetAllocateInfo.descriptorPool = m_descriptorPool;
			descriptorSetAllocateInfo.pSetLayouts = layouts.data();
			descriptorSetAllocateInfo.descriptorSetCount = 1;

			vkAllocateDescriptorSets( m_device.mDevice, &descriptorSetAllocateInfo, &m_descriptorSet );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	DescriptorSampler::~DescriptorSampler()
	{
		if ( m_descriptorPool != VK_NULL_HANDLE )
		{
			vkDestroyDescriptorPool( m_device.mDevice, m_descriptorPool, nullptr );
			m_descriptorPool = VK_NULL_HANDLE;
		}

		if ( m_descriptorSetLayout != VK_NULL_HANDLE )
		{
			vkDestroyDescriptorSetLayout( m_device.mDevice, m_descriptorSetLayout, nullptr );
			m_descriptorSetLayout = VK_NULL_HANDLE;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void DescriptorSampler::Update()
	{

		// 		VkDescriptorImageInfo imageInfo;
		// 		imageInfo.sampler = _sampler;
		// 		imageInfo.imageView = VK_NULL_HANDLE;
		// 		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkWriteDescriptorSet writeDescriptorSet;
		writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.dstSet = m_descriptorSet;
		writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
		writeDescriptorSet.dstBinding = 0;
		writeDescriptorSet.pImageInfo = nullptr;
		writeDescriptorSet.descriptorCount = 1;

		vkUpdateDescriptorSets( m_device.mDevice, 1, &writeDescriptorSet, 0, nullptr );
	}
}