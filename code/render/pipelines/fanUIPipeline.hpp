#pragma once

#include "core/memory/fanAlignedMemory.hpp"
#include "render/core/fanPipeline.hpp"
#include "render/fanUIMesh.hpp"
#include "render/core/descriptors/fanDescriptorUniforms.hpp"
#include "render/core/descriptors/fanDescriptorImages.hpp"
#include "render/core/descriptors/fanDescriptorSampler.hpp"

namespace fan
{
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
		UIPipeline( Device& _device, DescriptorImages* _textures, DescriptorSampler* _sampler );
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
		DescriptorImages* m_textures;
		DescriptorSampler* m_sampler;
	};
}