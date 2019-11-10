#pragma once

#include "renderer/core/fanPipeline.h"
#include "core/memory/fanAlignedMemory.h"
#include "renderer/fanUIMesh.h"

namespace fan
{
	class DescriptorTextures;
	class Sampler;
	class Descriptor;

	//================================================================
	//================================================================
	struct DynamicUniformUIVert
	{
		glm::vec2 position;
		glm::vec2 scale;
		glm::vec4 color;
	};

	//================================================================================================================================
	//================================================================================================================================
	class UIPipeline : public Pipeline
	{
	public:
		UIPipeline( Device& _device, DescriptorTextures*& _textures );
		~UIPipeline() override;

		void UpdateUniformBuffers( const size_t _index = 0 ) override;		
		void CreateDescriptors( const size_t _numSwapchainImages );
		void BindDescriptors( VkCommandBuffer _commandBuffer, const size_t _indexFrame, const uint32_t _indexOffset );
		void ResizeDynamicDescriptors ( const size_t _newSize );

		AlignedMemory<DynamicUniformUIVert>		m_dynamicUniformsVert;

	protected:
		void ConfigurePipeline() override;

	private:
		Descriptor *			m_transformDescriptor = nullptr;
		DescriptorTextures*&    m_textures;
		Sampler *				m_sampler;
	};
}