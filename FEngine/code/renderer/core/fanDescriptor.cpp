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

		for (int bufferIndex = 0; bufferIndex < m_uniformBuffers.size() ; bufferIndex++) {
			delete m_uniformBuffers[bufferIndex];
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Descriptor::AddUniformBinding( VkShaderStageFlags  _stage, VkDeviceSize _bufferSize ) {
		AddBinding( _stage, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, _bufferSize );
	}	
	void Descriptor::AddDynamicUniformBinding( VkShaderStageFlags  _stage, VkDeviceSize _bufferSize, VkDeviceSize _alignment ) {
		AddBinding( _stage, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1, _bufferSize, _alignment );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Descriptor::AddBinding( VkShaderStageFlags _stage, VkDescriptorType _descriptorType, uint32_t _descriptorCount, VkDeviceSize _sizeBuffer, VkDeviceSize _alignment ) {
		VkDescriptorSetLayoutBinding layoutBinding;
		layoutBinding.binding = static_cast<uint32_t>( m_layoutBindings.size() );
		layoutBinding.descriptorType = _descriptorType;
		layoutBinding.descriptorCount = _descriptorCount;
		layoutBinding.stageFlags = _stage;
		layoutBinding.pImmutableSamplers = nullptr;
		m_layoutBindings.push_back( layoutBinding );

		// Create Buffer
		Buffer * uniformBuffer = new Buffer( m_device );
		uniformBuffer->Create(
			_sizeBuffer,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
			_alignment
		);
		m_uniformBuffers.push_back( uniformBuffer );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Descriptor::SetBinding( const int _index, const void * _data, VkDeviceSize _size, VkDeviceSize _offset ) {
		assert( _index  >= 0 && _index < m_uniformBuffers .size() );	
		m_uniformBuffers[_index]->SetData( _data, _size, _offset );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Descriptor::Bind( VkCommandBuffer _commandBuffer, VkPipelineLayout _pipelineLayout ) {
		vkCmdBindDescriptorSets( _commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineLayout, 0, 1, &m_descriptorSet, 0, nullptr );
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Descriptor::Create() {
		// Create DescriptorSetLayout
		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
		descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutCreateInfo.pNext = nullptr;
		descriptorSetLayoutCreateInfo.flags = 0;
		descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>( m_layoutBindings.size() );
		descriptorSetLayoutCreateInfo.pBindings = m_layoutBindings.data();

		if ( vkCreateDescriptorSetLayout( m_device.vkDevice, &descriptorSetLayoutCreateInfo, nullptr, &m_descriptorSetLayout ) != VK_SUCCESS ) {
			Debug::Error( "Could not allocate descriptor set layout." );
			return false;
		} Debug::Get() << Debug::Severity::log << std::hex << "VkDescriptorSetLayout " << m_descriptorSetLayout << std::dec << Debug::Endl();

		// Create DescriptorPool
		std::vector< VkDescriptorPoolSize > poolSizes( m_layoutBindings.size() );
		for ( int layoutIndex = 0; layoutIndex < m_layoutBindings.size(); layoutIndex++ ) {
			poolSizes[layoutIndex].type = m_layoutBindings[layoutIndex].descriptorType;
			poolSizes[layoutIndex].descriptorCount = m_layoutBindings[layoutIndex].descriptorCount;
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

		// Create descriptor set
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

		// Update DescriptorSets
		std::vector<VkWriteDescriptorSet>	writeDescriptors( m_layoutBindings.size() );
		std::vector<VkDescriptorBufferInfo> bufferInfos( m_layoutBindings.size() );
		for ( int descriptorIndex = 0; descriptorIndex < m_layoutBindings.size(); descriptorIndex++ ) {
			Buffer * buffer = m_uniformBuffers[descriptorIndex];

			bufferInfos[descriptorIndex].buffer = buffer->GetBuffer();
			bufferInfos[descriptorIndex].offset = 0;
			bufferInfos[descriptorIndex].range = buffer->GetAlignment() > 1 ? buffer->GetAlignment() : buffer->GetSize();

			writeDescriptors[descriptorIndex].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptors[descriptorIndex].pNext = nullptr;
			writeDescriptors[descriptorIndex].dstSet = m_descriptorSet;
			writeDescriptors[descriptorIndex].dstBinding = descriptorIndex;
			writeDescriptors[descriptorIndex].dstArrayElement = 0;
			writeDescriptors[descriptorIndex].descriptorCount = m_layoutBindings[descriptorIndex].descriptorCount;
			writeDescriptors[descriptorIndex].descriptorType = m_layoutBindings[descriptorIndex].descriptorType;
			writeDescriptors[descriptorIndex].pImageInfo = nullptr;
			writeDescriptors[descriptorIndex].pBufferInfo = &bufferInfos[descriptorIndex];
			//uboWriteDescriptorSet.pTexelBufferView = nullptr;
		}

		vkUpdateDescriptorSets(
			m_device.vkDevice,
			static_cast<uint32_t>( writeDescriptors.size() ),
			writeDescriptors.data(),
			0,
			nullptr
		);
		return true;
	}
}