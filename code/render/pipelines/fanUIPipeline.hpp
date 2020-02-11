#pragma once

#include "render/fanRenderPrecompiled.hpp"
#include "render/core/fanPipeline.hpp"
#include "core/memory/fanAlignedMemory.hpp"
#include "render/fanUIMesh.hpp"

namespace fan
{
	class DescriptorTextures;
	class DescriptorSampler;
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
		UIPipeline( Device& _device, DescriptorTextures*& _textures, DescriptorSampler*& _sampler );
		~UIPipeline() override;

		void UpdateUniformBuffers( const size_t _index = 0 ) override;
		void CreateDescriptors( const size_t _numSwapchainImages );
		void BindDescriptors( VkCommandBuffer _commandBuffer, const size_t _indexFrame, const uint32_t _indexOffset );
		void ResizeDynamicDescriptors( const size_t _newSize );

		AlignedMemory<DynamicUniformUIVert>		m_dynamicUniformsVert;

	protected:
		void ConfigurePipeline() override;

	private:
		Descriptor* m_transformDescriptor = nullptr;
		DescriptorTextures*& m_textures;
		DescriptorSampler*& m_sampler;
	};
}