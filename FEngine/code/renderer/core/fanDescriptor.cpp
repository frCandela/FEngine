#include "fanGlobalIncludes.h"
#include "renderer/core/fanDescriptor.h"

#include "renderer/core/fanDevice.h"
#include "renderer/core/fanBuffer.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	Descriptor::Descriptor( Device& _device ) :
		m_device( _device ) {

	}

	//================================================================================================================================
	//================================================================================================================================
	Descriptor::~Descriptor() {
		if ( m_descriptorPool != VK_NULL_HANDLE ) {
			vkDestroyDescriptorPool( m_device.vkDevice, m_descriptorPool, nullptr );
			m_descriptorPool = VK_NULL_HANDLE;
		}

		if ( m_descriptorSetLayout != VK_NULL_HANDLE ) {
			vkDestroyDescriptorSetLayout( m_device.vkDevice, m_descriptorSetLayout, nullptr );
			m_descriptorSetLayout = VK_NULL_HANDLE;
		}

		for (int bindingIndex = 0; bindingIndex < m_bindingData.size() ; bindingIndex++) {
			delete m_bindingData[bindingIndex].buffer;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Descriptor::SetUniformBinding( VkShaderStageFlags  _stage, VkDeviceSize _bufferSize, const int /*_index*/ ) {
		BindingData bindingData;
		bindingData.SetBuffer( m_device, _bufferSize );
		bindingData.UpdateLayoutBinding( m_bindingData.size(), _stage, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1 );
		m_bindingData.push_back(bindingData);
	}	

	//================================================================================================================================
	//================================================================================================================================
	void Descriptor::SetDynamicUniformBinding( VkShaderStageFlags  _stage, VkDeviceSize _bufferSize, VkDeviceSize _alignment, const int /*_index*/ ) {
		BindingData bindingData;
		bindingData.SetBuffer( m_device, _bufferSize, _alignment );
		bindingData.UpdateLayoutBinding(  m_bindingData.size(), _stage, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1 );
		m_bindingData.push_back( bindingData );
	}

	//================================================================================================================================
	// Adds a COMBINED_IMAGE_SAMPLER binding 
	// If '_index' is -1 appends it to the bindings list
	// If '_index' > 0 updates the corresponding binding
	//================================================================================================================================
	void Descriptor::SetImageSamplerBinding( VkShaderStageFlags  _stage, std::vector< VkImageView > & _imageViews, VkSampler _sampler, const int _index ) {
		if ( _index < 0 ) {
			BindingData bindingData;
			bindingData.SetImagesSampler( _imageViews, _sampler );
			bindingData.UpdateLayoutBinding( m_bindingData.size(), _stage, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, _imageViews.size() );
			m_bindingData.push_back( bindingData );
		} else {
			assert( _index < m_bindingData.size() );
			m_bindingData[_index].SetImagesSampler( _imageViews, _sampler );
			m_bindingData[_index].UpdateLayoutBinding( m_bindingData.size(), _stage, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, _imageViews.size() );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Descriptor::SetBinding( const int _index, const void * _data, VkDeviceSize _size, VkDeviceSize _offset ) {
		assert( _index  >= 0 && _index < m_bindingData .size() );	
		assert( m_bindingData[_index].buffer != nullptr );
		m_bindingData[_index].buffer->SetData( _data, _size, _offset );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Descriptor::Bind( VkCommandBuffer _commandBuffer, VkPipelineLayout _pipelineLayout ) {
		vkCmdBindDescriptorSets( _commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineLayout, 0, 1, &m_descriptorSet, 0, nullptr );
	}

	//================================================================================================================================
	//================================================================================================================================
	std::vector < VkDescriptorSetLayoutBinding > Descriptor::GetLayoutBindingsArray() {
		std::vector < VkDescriptorSetLayoutBinding > layoutBindings;
		layoutBindings.reserve( m_bindingData.size());
		for (int bindingIndex = 0; bindingIndex < m_bindingData.size(); bindingIndex++)	{
			layoutBindings.push_back( m_bindingData[bindingIndex].layoutBinding );
		}
		return layoutBindings;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Descriptor::Create() {
		// Create DescriptorSetLayout
		{
			std::vector < VkDescriptorSetLayoutBinding > bindingsArray = GetLayoutBindingsArray();
			VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
			descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			descriptorSetLayoutCreateInfo.pNext = nullptr;
			descriptorSetLayoutCreateInfo.flags = 0;
			descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>( m_bindingData.size() );
			descriptorSetLayoutCreateInfo.pBindings = bindingsArray.data();

			if ( vkCreateDescriptorSetLayout( m_device.vkDevice, &descriptorSetLayoutCreateInfo, nullptr, &m_descriptorSetLayout ) != VK_SUCCESS ) {
				Debug::Error( "Could not allocate descriptor set layout." );
				return false;
			} Debug::Get() << Debug::Severity::log << std::hex << "VkDescriptorSetLayout " << m_descriptorSetLayout << std::dec << Debug::Endl();
		}

		// Create DescriptorPool
		{
			std::vector< VkDescriptorPoolSize > poolSizes( m_bindingData.size() );
			for ( int layoutIndex = 0; layoutIndex < m_bindingData.size(); layoutIndex++ ) {
				poolSizes[layoutIndex].type = m_bindingData[layoutIndex].layoutBinding.descriptorType;
				poolSizes[layoutIndex].descriptorCount = m_bindingData[layoutIndex].layoutBinding.descriptorCount;
			}
			VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
			descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			descriptorPoolCreateInfo.pNext = nullptr;
			descriptorPoolCreateInfo.flags = 0;
			descriptorPoolCreateInfo.maxSets = 1;
			descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>( poolSizes.size() );
			descriptorPoolCreateInfo.pPoolSizes = poolSizes.data();
			if ( vkCreateDescriptorPool( m_device.vkDevice, &descriptorPoolCreateInfo, nullptr, &m_descriptorPool ) != VK_SUCCESS ) {
				Debug::Error( "Could not allocate descriptor pool." );
				return false;
			} Debug::Get() << Debug::Severity::log << std::hex << "VkDescriptorPool      " << m_descriptorPool << std::dec << Debug::Endl();
		}
		
		// Create descriptor set
		{
			std::vector< VkDescriptorSetLayout > descriptorSetLayouts = { m_descriptorSetLayout };
			VkDescriptorSetAllocateInfo descriptorSetAllocateInfo;
			descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			descriptorSetAllocateInfo.pNext = nullptr;
			descriptorSetAllocateInfo.descriptorPool = m_descriptorPool;
			descriptorSetAllocateInfo.descriptorSetCount = static_cast<uint32_t>( descriptorSetLayouts.size() );
			descriptorSetAllocateInfo.pSetLayouts = descriptorSetLayouts.data();

			if ( vkAllocateDescriptorSets( m_device.vkDevice, &descriptorSetAllocateInfo, &m_descriptorSet ) != VK_SUCCESS ) {
				Debug::Error( "Could not allocate descriptor set." );
				return false;
			}
			Debug::Get() << Debug::Severity::log << std::hex << "VkDescriptorSet       " << m_descriptorSet << std::dec << Debug::Endl();
		}

		Update();

		return true;
	}

	void Descriptor::Update() {
		std::vector<VkWriteDescriptorSet>	writeDescriptors;
		for ( int descriptorIndex = 0; descriptorIndex < m_bindingData.size(); descriptorIndex++ ) {
			m_bindingData[descriptorIndex].UpdateWriteDescriptorSet( descriptorIndex, m_descriptorSet );
			writeDescriptors.push_back( m_bindingData[descriptorIndex].writeDescriptorSet );
		}

		vkUpdateDescriptorSets(
			m_device.vkDevice,
			static_cast<uint32_t>( writeDescriptors.size() ),
			writeDescriptors.data(),
			0,
			nullptr
		);
	}

	//================================================================================================================================
	// Descriptor::BindingData
	//================================================================================================================================
	void Descriptor::BindingData::SetBuffer( Device& _device, VkDeviceSize _sizeBuffer, VkDeviceSize _alignment ) {
		buffer = new Buffer( _device );
		buffer->Create(   
			_sizeBuffer,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
			_alignment
		);
		descriptorBufferInfo.buffer = buffer->GetBuffer();
		descriptorBufferInfo.offset = 0;
		descriptorBufferInfo.range = buffer->GetAlignment() > 1 ? buffer->GetAlignment() : buffer->GetSize();
	}

	//================================================================================================================================
	// Descriptor::BindingData
	//================================================================================================================================
	void Descriptor::BindingData::SetImagesSampler( std::vector< VkImageView > & _imageViews, VkSampler _sampler ) {
		descriptorsImageInfo.clear();
		descriptorsImageInfo.reserve( _imageViews.size() );
		for ( int viewIndex = 0; viewIndex < _imageViews.size(); viewIndex++ ) {
			VkDescriptorImageInfo imageInfo;
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = _imageViews[viewIndex];
			imageInfo.sampler = _sampler;
			descriptorsImageInfo.push_back( imageInfo );
		}
	}
	//================================================================================================================================
	// Descriptor::BindingData
	//================================================================================================================================
	void Descriptor::BindingData::UpdateLayoutBinding( const size_t _index, const VkShaderStageFlags _stage, const VkDescriptorType _descriptorType, const size_t _descriptorCount ) {
		layoutBinding.binding = static_cast<uint32_t>(_index);
		layoutBinding.descriptorType = _descriptorType;
		layoutBinding.descriptorCount = static_cast<uint32_t>(_descriptorCount);
		layoutBinding.stageFlags = _stage;
		layoutBinding.pImmutableSamplers = nullptr;
	}

	//================================================================================================================================
	// Descriptor::BindingData
	//================================================================================================================================
	void  Descriptor::BindingData::UpdateWriteDescriptorSet( const size_t _dstBinding, VkDescriptorSet _descriptorSet ) {
		writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.pNext = nullptr;
		writeDescriptorSet.dstSet = _descriptorSet;
		writeDescriptorSet.dstBinding = static_cast<uint32_t>(_dstBinding );
		writeDescriptorSet.dstArrayElement = 0;
		writeDescriptorSet.descriptorCount = layoutBinding.descriptorCount;
		writeDescriptorSet.descriptorType = layoutBinding.descriptorType;
		writeDescriptorSet.pImageInfo = descriptorsImageInfo.empty() ? nullptr : descriptorsImageInfo.data();
		writeDescriptorSet.pBufferInfo = buffer != nullptr ? &descriptorBufferInfo : nullptr ;
		//uboWriteDescriptorSet.pTexelBufferView = nullptr;
	}
}