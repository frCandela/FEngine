#pragma once

#include "glfw/glfw3.h"
#include <vector>
#include "core/memory/fanAlignedMemory.hpp"
#include "render/core/fanBuffer.hpp"
#include "render/core/fanSwapChain.hpp"

namespace fan
{
	struct Device;

	//================================================================================================================================
	// Create a uniform buffer descriptor or a dynamic  uniform buffer descriptor
	// @todo the descriptor class shouldn't create buffers for the user, they should be passed as arguments
	//================================================================================================================================
	struct DescriptorUniforms
	{
		//================================================================
		// Contain the information for a descriptor binding
		// used when creating & updating the descriptor sets
		//================================================================
		struct BindingData
		{
			VkDescriptorSetLayoutBinding	layoutBinding;						
			Buffer							buffers[ SwapChain::s_maxFramesInFlight ];

			void CreateBuffers( Device& _device, const size_t _count, VkDeviceSize _sizeBuffer, VkDeviceSize _alignment = 1 );
		};

		void AddUniformBinding( Device& _device, const size_t _count, const VkShaderStageFlags  _stage, const VkDeviceSize _bufferSize );
		void AddDynamicUniformBinding( Device& _device, const size_t _count, VkShaderStageFlags  _stage, VkDeviceSize _bufferSize, VkDeviceSize _alignment );
		bool Create( Device& _device, const uint32_t _count = 1 );
		void Destroy( Device& _device );

		void Bind( VkCommandBuffer _commandBuffer, VkPipelineLayout _pipelineLayout, const size_t _index );
		void SetData( Device& _device, const size_t _indexBinding, const size_t _indexBuffer, const void* _data, VkDeviceSize _size, VkDeviceSize _offset );
		void ResizeDynamicUniformBinding( Device& _device, const size_t _count, VkDeviceSize _bufferSize, VkDeviceSize _alignment, const int _index );
		void UpdateDescriptorSets( Device& _device, const uint32_t _count );		

		VkDescriptorSetLayout		mDescriptorSetLayout = VK_NULL_HANDLE;
		VkDescriptorPool			mDescriptorPool	  = VK_NULL_HANDLE;
		VkDescriptorSet 			mDescriptorSets[SwapChain::s_maxFramesInFlight] = { VK_NULL_HANDLE ,VK_NULL_HANDLE ,VK_NULL_HANDLE };
		std::vector< BindingData >  mBindingData;
	};
}