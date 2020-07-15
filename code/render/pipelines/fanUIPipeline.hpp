#pragma once

#include "core/memory/fanAlignedMemory.hpp"
#include "render/pipelines/fanPipeline.hpp"
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
		void Plop( Device& _device );

		void SetUniformsData( Device& _device, const size_t _index = 0 ) override;
		void CreateDescriptors( Device& _device, const uint32_t _numSwapchainImages );
		void BindDescriptors( VkCommandBuffer _commandBuffer, const size_t _indexFrame, const uint32_t _indexOffset );
		void ResizeDynamicDescriptors( Device& _device, const uint32_t _count, const size_t _newSize );

		AlignedMemory<DynamicUniformUIVert>		m_dynamicUniformsVert;

		PipelineConfig GetConfig( Shader& _vert, Shader& _frag );

	private:
		DescriptorUniforms m_transformDescriptor;
		DescriptorImages* m_textures;
		DescriptorSampler* m_sampler;
	};
}