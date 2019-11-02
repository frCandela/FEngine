#pragma once

#include "core/memory/fanAlignedMemory.h"

namespace fan {
	class Device;
	class Buffer;
	class Sampler;
	 
	//================================================================================================================================
	//================================================================================================================================
	class Descriptor {
	private:
		//================================================================
		// Contain the information for a descriptor binding
		//================================================================
		struct BindingData {			
			VkDescriptorSetLayoutBinding		 layoutBinding;
			std::vector < VkWriteDescriptorSet>	 writeDescriptorSets;
			std::vector<VkDescriptorBufferInfo>  descriptorBufferInfos;
			std::vector<Buffer * >				 buffers;

			void SetBuffers( Device& _device, const size_t _count, VkDeviceSize _sizeBuffer, VkDeviceSize _alignment = 1 );
			void UpdateLayoutBinding( const size_t _index, const VkShaderStageFlags _stage, const VkDescriptorType _descriptorType, const size_t _descriptorCount );
			void UpdateWriteDescriptorSet( const size_t _dstBinding, const size_t _setIndex, VkDescriptorSet _descriptorSet );
		};

	public:
		Descriptor( Device& _device, const size_t _count );
		~Descriptor();

		bool Create(  );
		void SetBinding( const size_t _indexBinding, const size_t _indexBuffer, const void * _data, VkDeviceSize _size, VkDeviceSize _offset );
		void Update();
		void Bind( VkCommandBuffer _commandBuffer, VkPipelineLayout _pipelineLayout, const size_t _index = 0 );
		
		void SetUniformBinding( const VkShaderStageFlags  _stage, const VkDeviceSize _bufferSize);
		void SetDynamicUniformBinding( VkShaderStageFlags  _stage, VkDeviceSize _bufferSize, VkDeviceSize _alignment, const int _index = -1 );

		VkDescriptorSetLayout GetLayout() { return m_descriptorSetLayout; }
		VkDescriptorSet		  GetSet( const size_t _index = 0 ) { return m_descriptorSets[ _index ]; }

	private:
		Device& m_device;

		VkDescriptorSetLayout			m_descriptorSetLayout;
		VkDescriptorPool				m_descriptorPool;
		std::vector< VkDescriptorSet >	m_descriptorSets;

		std::vector< BindingData > m_bindingData;
		std::vector < VkDescriptorSetLayoutBinding > GetLayoutBindingsArray();

		size_t m_numDescriptors = 1;
	};
}