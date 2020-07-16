#pragma once

#include "render/core/fanPipeline.hpp"
#include "render/core/fanShader.hpp"
#include "render/fanUniforms.hpp"
#include "render/core/descriptors/fanDescriptorUniforms.hpp"
#include "render/core/descriptors/fanDescriptorImages.hpp"
#include "render/core/descriptors/fanDescriptorSampler.hpp"
#include "render/core/fanCommandBuffer.hpp"
#include "render/core/fanSampler.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	struct DrawUI
	{
		Pipeline			mPipeline;
		Shader				mFragmentShader;
		Shader				mVertexShader;
		DescriptorUniforms	mDescriptorTransform;
		UiUniforms			mUniforms;
		DescriptorSampler	mDescriptorSampler;
		Sampler				mSampler;
		CommandBuffer		mCommandBuffers;
		
		void			Create( Device& _device, uint32_t _imagesCount );
		void			Destroy( Device& _device );
		void			BindDescriptors( VkCommandBuffer _commandBuffer, const size_t _indexFrame, const uint32_t _indexOffset );
		void			UpdateUniformBuffers( Device& _device, const size_t _index );
		PipelineConfig	GetPipelineConfig( DescriptorImages& _descriptorImages ) const;
	};
}