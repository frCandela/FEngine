#include "render/descriptors/fanDescriptor.hpp"
#include "render/core/fanDevice.hpp"
#include "render/core/fanBuffer.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Descriptor::Descriptor( Device& _device, const size_t _count ) :
		m_device( _device )
		, m_numDescriptors( _count )
	{

	}

	//================================================================================================================================
	//================================================================================================================================
	Descriptor::~Descriptor()
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

		for ( int bindingIndex = 0; bindingIndex < m_bindingData.size(); bindingIndex++ )
		{
			for ( int bufferIndex = 0; bufferIndex < m_bindingData[ bindingIndex ].buffers.size(); bufferIndex++ )
			{
				delete m_bindingData[ bindingIndex ].buffers[ bufferIndex ];
			}
		}
	}

	//================================================================================================================================
	// Adds a dynamic uniform buffer binding
	//================================================================================================================================
	void Descriptor::SetUniformBinding( const VkShaderStageFlags  _stage, const VkDeviceSize _bufferSize )
	{
		BindingData bindingData;
		bindingData.SetBuffers( m_device, m_numDescriptors, _bufferSize );
		bindingData.UpdateLayoutBinding( m_bindingData.size(), _stage, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1 );
		m_bindingData.push_back( bindingData );
	}

	//================================================================================================================================
	// Adds a uniform buffer binding
	//================================================================================================================================
	void Descriptor::SetDynamicUniformBinding( VkShaderStageFlags  _stage, VkDeviceSize _bufferSize, VkDeviceSize _alignment, const int _index )
	{
		if ( _index < 0 )
		{
			BindingData bindingData;
			bindingData.SetBuffers( m_device, m_numDescriptors, _bufferSize * _alignment, _alignment );
			bindingData.UpdateLayoutBinding( m_bindingData.size(), _stage, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1 );
			m_bindingData.push_back( bindingData );
		}
		else
		{
			assert( _index < m_bindingData.size() );
			for ( int bufferIndex = 0; bufferIndex < m_bindingData[ _index ].buffers.size(); bufferIndex++ )
			{
				delete m_bindingData[ _index ].buffers[ bufferIndex ];
			} m_bindingData[ _index ].buffers.clear();
			m_bindingData[ _index ].SetBuffers( m_device, m_numDescriptors, _bufferSize * _alignment, _alignment );
		}
	}

	//================================================================================================================================
	// For uniform buffers only, update buffer data of the binding at _index
	//================================================================================================================================
	void Descriptor::SetBinding( const size_t _indexBinding, const size_t _indexBuffer, const void* _data, VkDeviceSize _size, VkDeviceSize _offset )
	{
		assert( _indexBinding >= 0 && _indexBinding < m_bindingData.size() );
		assert( _indexBuffer < m_bindingData[ _indexBinding ].buffers.size() );
		m_bindingData[ _indexBinding ].buffers[ _indexBuffer ]->SetData( _data, _size, _offset );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Descriptor::Bind( VkCommandBuffer _commandBuffer, VkPipelineLayout _pipelineLayout, const size_t _index )
	{
		assert( _index < m_numDescriptors );
		vkCmdBindDescriptorSets( _commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineLayout, 0, 1, &m_descriptorSets[ _index ], 0, nullptr );
	}

	//================================================================================================================================
	//================================================================================================================================
	std::vector < VkDescriptorSetLayoutBinding > Descriptor::GetLayoutBindingsArray()
	{
		std::vector < VkDescriptorSetLayoutBinding > layoutBindings;
		layoutBindings.reserve( m_bindingData.size() );
		for ( int bindingIndex = 0; bindingIndex < m_bindingData.size(); bindingIndex++ )
		{
			layoutBindings.push_back( m_bindingData[ bindingIndex ].layoutBinding );
		}
		return layoutBindings;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Descriptor::Create()
	{
		// Create DescriptorSetLayout
		{
			std::vector < VkDescriptorSetLayoutBinding > bindingsArray = GetLayoutBindingsArray();
			VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
			descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			descriptorSetLayoutCreateInfo.pNext = nullptr;
			descriptorSetLayoutCreateInfo.flags = 0;
			descriptorSetLayoutCreateInfo.bindingCount = static_cast< uint32_t >( m_bindingData.size() );
			descriptorSetLayoutCreateInfo.pBindings = bindingsArray.data();

			if ( vkCreateDescriptorSetLayout( m_device.vkDevice, &descriptorSetLayoutCreateInfo, nullptr, &m_descriptorSetLayout ) != VK_SUCCESS )
			{
				Debug::Error( "Could not allocate descriptor set layout." );
				return false;
			} Debug::Get() << Debug::Severity::log << std::hex << "VkDescriptorSetLayout " << m_descriptorSetLayout << std::dec << Debug::Endl();
		}

		// Create DescriptorPool
		{
			std::vector< VkDescriptorPoolSize > poolSizes( m_bindingData.size() );
			for ( int layoutIndex = 0; layoutIndex < m_bindingData.size(); layoutIndex++ )
			{
				poolSizes[ layoutIndex ].type = m_bindingData[ layoutIndex ].layoutBinding.descriptorType;
				poolSizes[ layoutIndex ].descriptorCount = m_bindingData[ layoutIndex ].layoutBinding.descriptorCount;
			}
			VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
			descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			descriptorPoolCreateInfo.pNext = nullptr;
			descriptorPoolCreateInfo.flags = 0;
			descriptorPoolCreateInfo.maxSets = static_cast< uint32_t >( m_numDescriptors );
			descriptorPoolCreateInfo.poolSizeCount = static_cast< uint32_t >( poolSizes.size() );
			descriptorPoolCreateInfo.pPoolSizes = poolSizes.data();
			if ( vkCreateDescriptorPool( m_device.vkDevice, &descriptorPoolCreateInfo, nullptr, &m_descriptorPool ) != VK_SUCCESS )
			{
				Debug::Error( "Could not allocate descriptor pool." );
				return false;
			} Debug::Get() << Debug::Severity::log << std::hex << "VkDescriptorPool      " << m_descriptorPool << std::dec << Debug::Endl();
		}

		// Create descriptor set
		{
			std::vector< VkDescriptorSetLayout > descriptorSetLayouts;
			for ( int layoutIndex = 0; layoutIndex < m_numDescriptors; layoutIndex++ )
			{
				descriptorSetLayouts.push_back( m_descriptorSetLayout );
			}
			VkDescriptorSetAllocateInfo descriptorSetAllocateInfo;
			descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			descriptorSetAllocateInfo.pNext = nullptr;
			descriptorSetAllocateInfo.descriptorPool = m_descriptorPool;
			descriptorSetAllocateInfo.descriptorSetCount = static_cast< uint32_t >( m_numDescriptors );
			descriptorSetAllocateInfo.pSetLayouts = descriptorSetLayouts.data();

			m_descriptorSets.resize( m_numDescriptors );
			if ( vkAllocateDescriptorSets( m_device.vkDevice, &descriptorSetAllocateInfo, m_descriptorSets.data() ) != VK_SUCCESS )
			{
				Debug::Error( "Could not allocate descriptor set." );
				return false;
			}
			for ( int setIndex = 0; setIndex < m_numDescriptors; setIndex++ )
			{
				Debug::Get() << Debug::Severity::log << std::hex << "VkDescriptorSet       " << m_descriptorSets[ setIndex ] << std::dec << Debug::Endl();
			}
		}

		Update();

		return true;
	}

	//================================================================================================================================
	// Update the descriptors sets using the old WriteDescriptor sets
	//================================================================================================================================
	void Descriptor::Update()
	{
		std::vector<VkWriteDescriptorSet>	writeDescriptors;

		for ( int bindingIndex = 0; bindingIndex < m_bindingData.size(); bindingIndex++ )
		{
			BindingData& bindingData = m_bindingData[ bindingIndex ];
			for ( int descriptorIndex = 0; descriptorIndex < m_numDescriptors; descriptorIndex++ )
			{
				bindingData.UpdateWriteDescriptorSet( bindingIndex, descriptorIndex, m_descriptorSets[ descriptorIndex ] );
				writeDescriptors.push_back( bindingData.writeDescriptorSets[ descriptorIndex ] );
			}
		}

		vkUpdateDescriptorSets(
			m_device.vkDevice,
			static_cast< uint32_t >( writeDescriptors.size() ),
			writeDescriptors.data(),
			0,
			nullptr
		);
	}

	//================================================================================================================================
	// Descriptor::BindingData
	// Adds a single uniform buffer
	// If alignement != 1 the buffer is considered dynamic and it's offset can be set at binding time
	//================================================================================================================================
	void Descriptor::BindingData::SetBuffers( Device& _device, const size_t _count, VkDeviceSize _sizeBuffer, VkDeviceSize _alignment )
	{
		buffers.resize( _count );
		descriptorBufferInfos.resize( _count );
		writeDescriptorSets.resize( _count );
		for ( int bufferIndex = 0; bufferIndex < _count; bufferIndex++ )
		{
			buffers[ bufferIndex ] = new Buffer( _device );
			buffers[ bufferIndex ]->Create(
				_sizeBuffer,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
				_alignment
			);
			descriptorBufferInfos[ bufferIndex ].buffer = buffers[ bufferIndex ]->GetBuffer();
			descriptorBufferInfos[ bufferIndex ].offset = 0;
			descriptorBufferInfos[ bufferIndex ].range = buffers[ bufferIndex ]->GetAlignment() > 1 ? buffers[ bufferIndex ]->GetAlignment() : buffers[ bufferIndex ]->GetSize();
		}
	}

	//================================================================================================================================
	// Descriptor::BindingData
	//================================================================================================================================
	void Descriptor::BindingData::UpdateLayoutBinding( const size_t _index, const VkShaderStageFlags _stage, const VkDescriptorType _descriptorType, const size_t _descriptorCount )
	{
		layoutBinding.binding = static_cast< uint32_t >( _index );
		layoutBinding.descriptorType = _descriptorType;
		layoutBinding.descriptorCount = static_cast< uint32_t >( _descriptorCount );
		layoutBinding.stageFlags = _stage;
		layoutBinding.pImmutableSamplers = nullptr;
	}

	//================================================================================================================================
	// Descriptor::BindingData
	//================================================================================================================================
	void  Descriptor::BindingData::UpdateWriteDescriptorSet( const size_t _dstBinding, const size_t _setIndex, VkDescriptorSet _descriptorSet )
	{
		writeDescriptorSets.resize( buffers.empty() ? 1 : buffers.size() );

		writeDescriptorSets[ _setIndex ].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[ _setIndex ].pNext = nullptr;
		writeDescriptorSets[ _setIndex ].dstSet = _descriptorSet;
		writeDescriptorSets[ _setIndex ].dstBinding = static_cast< uint32_t >( _dstBinding );
		writeDescriptorSets[ _setIndex ].dstArrayElement = 0;
		writeDescriptorSets[ _setIndex ].descriptorCount = layoutBinding.descriptorCount;
		writeDescriptorSets[ _setIndex ].descriptorType = layoutBinding.descriptorType;
		writeDescriptorSets[ _setIndex ].pImageInfo = nullptr;
		writeDescriptorSets[ _setIndex ].pBufferInfo = buffers.empty() ? nullptr : &descriptorBufferInfos[ _setIndex ];
		//uboWriteDescriptorSet.pTexelBufferView = nullptr;		
	}
}