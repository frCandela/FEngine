#pragma once

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
		void AddBinding( VkShaderStageFlags  _stage, VkDeviceSize _sizeBuffer );
		void SetBinding( const int _index, const void * _data, VkDeviceSize _size, VkDeviceSize _offset = 0 );
		void Bind( const int _index, VkCommandBuffer _commandBuffer, VkPipelineLayout _pipelineLayout );
		
		VkDescriptorSetLayout GetLayout() { return m_descriptorSetLayout; }

	private:
		Device& m_device;

		VkDescriptorSetLayout	m_descriptorSetLayout;
		VkDescriptorPool		m_descriptorPool;
		std::vector<VkDescriptorSet> m_descriptorSets;

		Shader * m_fragmentShader = nullptr;
		Shader * m_vertexShader = nullptr;

		std::vector< Buffer * > m_uniformBuffers;
		std::vector< VkDescriptorSetLayoutBinding > m_layoutBindings;
	};
}