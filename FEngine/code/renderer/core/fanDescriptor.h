#pragma once

#include "core/memory/fanAlignedMemory.h"
#include "renderer/fanUniforms.h"

namespace fan {
	class Device;
	class Shader;
	class Buffer;

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
		VkDescriptorSetLayout GetLayout() { return m_descriptorSetLayout; }
		VkDescriptorSet		  GetSet() { return m_descriptorSet; }

// 		Buffer *	m_vertUniformBuffer;
// 		Buffer *	m_dynamicUniformBufferVert;

	private:
		Device& m_device;

		VkDescriptorSetLayout	m_descriptorSetLayout;
		VkDescriptorPool		m_descriptorPool;
		VkDescriptorSet			m_descriptorSet;

		Shader * m_fragmentShader = nullptr;
		Shader * m_vertexShader = nullptr;

		std::vector< Buffer * > m_uniformBuffers;
		std::vector< VkDescriptorSetLayoutBinding > m_layoutBindings;

		void AddBinding( VkShaderStageFlags _stage, VkDescriptorType _descriptorType, uint32_t _descriptorCount, VkDeviceSize _bufferSize, VkDeviceSize _alignment = 1 );

	};
}