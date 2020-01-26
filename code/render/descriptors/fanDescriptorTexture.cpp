#include "render/descriptors/fanDescriptorTexture.hpp"
#include "render/core/fanDevice.hpp"
#include "render/core/fanTexture.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	DescriptorTextures::DescriptorTextures( Device& _device, const uint32_t _maxTextures, VkSampler _sampler ) :
		  m_device( _device )
		, m_sampler( _sampler )
		, m_maxTextures( _maxTextures )
	{
		m_views.reserve( m_maxTextures );

		m_descriptorType = ( m_sampler == VK_NULL_HANDLE ? VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE : VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER );

		// Descriptor pool
		{
			VkDescriptorPoolSize descriptorPoolSize = {};
			descriptorPoolSize.type = m_descriptorType;
			descriptorPoolSize.descriptorCount = 1;

			std::vector<VkDescriptorPoolSize> poolSizes = { descriptorPoolSize };
			VkDescriptorPoolCreateInfo descriptorPoolInfo = {};

			descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>( poolSizes.size() );
			descriptorPoolInfo.pPoolSizes = poolSizes.data();
			descriptorPoolInfo.maxSets = m_maxTextures;
			vkCreateDescriptorPool( m_device.vkDevice, &descriptorPoolInfo, nullptr, &m_descriptorPool );
		}

		// Descriptor set layout
		{
			VkDescriptorSetLayoutBinding setLayoutBinding {};
			setLayoutBinding.descriptorType = m_descriptorType;
			setLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
			setLayoutBinding.binding = 0;
			setLayoutBinding.descriptorCount = 1;

			std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings = { setLayoutBinding };

			VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
			descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			descriptorSetLayoutCreateInfo.pBindings = setLayoutBindings.data();
			descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>( setLayoutBindings.size() );

			vkCreateDescriptorSetLayout( m_device.vkDevice, &descriptorSetLayoutCreateInfo, nullptr, &m_descriptorSetLayout );

			// Descriptor set
			std::vector<VkDescriptorSetLayout>  layouts( m_maxTextures, m_descriptorSetLayout );

			VkDescriptorSetAllocateInfo descriptorSetAllocateInfo {};
			descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			descriptorSetAllocateInfo.descriptorPool = m_descriptorPool;
			descriptorSetAllocateInfo.pSetLayouts = layouts.data();
			descriptorSetAllocateInfo.descriptorSetCount = m_maxTextures;

			m_descriptorSets.resize( m_maxTextures );
			vkAllocateDescriptorSets( m_device.vkDevice, &descriptorSetAllocateInfo, m_descriptorSets.data() );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	DescriptorTextures::~DescriptorTextures()
	{
		if ( m_descriptorPool != VK_NULL_HANDLE )
		{
			vkDestroyDescriptorPool( m_device.vkDevice, m_descriptorPool, nullptr );
			m_descriptorPool = VK_NULL_HANDLE;
		}

		if ( m_descriptorSetLayout != VK_NULL_HANDLE )
		{
			vkDestroyDescriptorSetLayout( m_device.vkDevice, m_descriptorSetLayout, nullptr );
			m_descriptorSetLayout = VK_NULL_HANDLE;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	uint32_t DescriptorTextures::Append( VkImageView _imageView )
	{
		assert( m_views.size() <= m_maxTextures );
		m_views.push_back( _imageView );
		return static_cast< uint32_t> (m_views.size() ) - 1;
	}

	//================================================================================================================================
	//================================================================================================================================
	void DescriptorTextures::Set( const uint32_t _index, VkImageView _imageView )
	{
		assert( _index < m_maxTextures );

		m_views[_index] = _imageView ;
	}

	//================================================================================================================================
	//================================================================================================================================
	void	 DescriptorTextures::UpdateRange( const uint32_t _begin, const uint32_t _end )
	{
		assert( _begin <= _end );
		assert( _end < m_views.size() );

		std::vector < VkWriteDescriptorSet> writeDescriptorSets( _end - _begin + 1 );
		std::vector<VkDescriptorImageInfo> imageInfos( m_maxTextures );
		for ( uint32_t viewIndex = _begin; viewIndex <= _end; viewIndex++ )
		{
			imageInfos[viewIndex].sampler = m_sampler;
			imageInfos[viewIndex].imageView = m_views[viewIndex];
			imageInfos[viewIndex].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			writeDescriptorSets[viewIndex].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptorSets[viewIndex].dstSet = m_descriptorSets[viewIndex];
			writeDescriptorSets[viewIndex].descriptorType = m_descriptorType;
			writeDescriptorSets[viewIndex].dstBinding = 0;
			writeDescriptorSets[viewIndex].pImageInfo = &imageInfos[viewIndex];
			writeDescriptorSets[viewIndex].descriptorCount = 1;
		}
		vkUpdateDescriptorSets( m_device.vkDevice, static_cast<uint32_t>( writeDescriptorSets.size() ), writeDescriptorSets.data(), 0, nullptr );
	}
}