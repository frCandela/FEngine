#pragma once

#include "render/core/fanPipeline.hpp"
#include "render/core/fanShader.hpp"
#include "render/fanUniforms.hpp"
#include "render/core/descriptors/fanDescriptorUniforms.hpp"
#include "render/core/descriptors/fanDescriptorImages.hpp"
#include "render/core/descriptors/fanDescriptorSampler.hpp"
#include "render/core/fanCommandBuffer.hpp"

namespace fan
{
	class Mesh;

	//================================================================================================================================
	//================================================================================================================================
	struct DrawData
	{
		Mesh* mesh;
		uint32_t textureIndex;
	};

	//================================================================================================================================
	//================================================================================================================================
	struct DrawModels
	{
		Pipeline				mPipeline;
		Shader					mFragmentShader;
		Shader					mVertexShader;
		ForwardUniforms			mUniforms;
		DescriptorUniforms		mDescriptorUniforms;
		CommandBuffer			mCommandBuffers;
		std::vector< DrawData >	mDrawData;
		
		void			Create( Device& _device, uint32_t _imagesCount );
		void			Destroy( Device& _device );
		void			BindDescriptors( VkCommandBuffer _commandBuffer, const size_t _indexFrame, const uint32_t _indexOffset );
		void			UpdateUniformBuffers( Device& _device, const size_t _index );
		PipelineConfig	GetPipelineConfig( DescriptorImages& _imagesDescriptor, DescriptorSampler& _samplerDescriptor ) const;
	};
}