#pragma once

#include "core/memory/fanAlignedMemory.h"
#include "renderer/fanUniforms.h"

namespace fan {
	class Device;
	class Buffer;
	class Texture;
	class Sampler;
	 
	//================================================================================================================================
	//================================================================================================================================
	class Descriptor {
	public:
		Descriptor( Device& _device );
		~Descriptor();

		bool Create(  );
		void SetBinding( const int _index, const void * _data, VkDeviceSize _size, VkDeviceSize _offset = 0 );
		void Bind( VkCommandBuffer _commandBuffer, VkPipelineLayout _pipelineLayout );
		
		void AddUniformBinding( VkShaderStageFlags  _stage, VkDeviceSize _bufferSize );
		void AddDynamicUniformBinding( VkShaderStageFlags  _stage, VkDeviceSize _bufferSize, VkDeviceSize _alignment );		
		void AddImageSamplerBinding( VkShaderStageFlags  _stage, std::vector< Texture * > & _textures, Sampler * _sampler );

		VkDescriptorSetLayout GetLayout() { return m_descriptorSetLayout; }
		VkDescriptorSet		  GetSet() { return m_descriptorSet; }

	private:
		Device& m_device;

		VkDescriptorSetLayout	m_descriptorSetLayout;
		VkDescriptorPool		m_descriptorPool;
		VkDescriptorSet			m_descriptorSet;

		struct BindingData {
			VkDescriptorSetLayoutBinding layoutBinding;
			Buffer * buffer = nullptr;
			VkDescriptorBufferInfo descriptorBufferInfo;
			std::vector< VkDescriptorImageInfo >  descriptorsImageInfo;
			VkWriteDescriptorSet   writeDescriptorSet;
			
			void SetImagesSampler( std::vector< Texture * > & _textures, Sampler * _sampler );
			void SetBuffer( Device& _device, VkDeviceSize _sizeBuffer, VkDeviceSize _alignment = 1 ) ;
			void UpdateLayoutBinding( const size_t _index, const VkShaderStageFlags _stage, const VkDescriptorType _descriptorType, const size_t _descriptorCount );
			void UpdateWriteDescriptorSet( const size_t _dstBinding, VkDescriptorSet _descriptorSet );
		};
		std::vector< BindingData > m_bindingData;
		std::vector < VkDescriptorSetLayoutBinding > GetLayoutBindingsArray();

	};
}