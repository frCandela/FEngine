#pragma once

#include "core/memory/fanAlignedMemory.hpp"
#include "render/core/fanPipeline.hpp"
#include "render/fanUIMesh.hpp"
#include "render/descriptors/fanDescriptorUniforms.hpp"

namespace fan
{
	class DescriptorTextures;
	class DescriptorSampler;

	//================================================================
	//================================================================
	struct DynamicUniformUIVert
	{
		glm::vec2 position;
		glm::vec2 scale;
		glm::vec4 color;
	};

	//================================================================================================================================
	// 2D ui
	//================================================================================================================================
	class UIPipeline : public Pipeline
	{
	public:
		UIPipeline( Device& _device, DescriptorTextures*& _textures, DescriptorSampler*& _sampler );
		~UIPipeline() override;

		void SetUniformsData( const size_t _index = 0 ) override;
		void CreateDescriptors( const uint32_t _numSwapchainImages );
		void BindDescriptors( VkCommandBuffer _commandBuffer, const size_t _indexFrame, const uint32_t _indexOffset );
		void ResizeDynamicDescriptors( const uint32_t _count, const size_t _newSize );

		AlignedMemory<DynamicUniformUIVert>		m_dynamicUniformsVert;

	protected:
		void ConfigurePipeline() override;

	private:
		DescriptorUniforms m_transformDescriptor;
		DescriptorTextures*& m_textures;
		DescriptorSampler*& m_sampler;
	};
}