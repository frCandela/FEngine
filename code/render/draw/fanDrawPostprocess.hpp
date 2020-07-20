#pragma once

#include "fanDisableWarnings.hpp"
WARNINGS_GLM_PUSH()
#include "glm/glm.hpp"
WARNINGS_POP()
#include "render/core/fanPipeline.hpp"
#include "render/core/fanShader.hpp"
#include "render/core/descriptors/fanDescriptorUniforms.hpp"
#include "render/core/descriptors/fanDescriptorImages.hpp"
#include "render/core/descriptors/fanDescriptorSampler.hpp"
#include "render/core/fanCommandBuffer.hpp"
#include "render/core/fanSampler.hpp"

namespace fan
{
	//================================================================
	//================================================================
	struct UniformsPostprocess
	{
		glm::vec4 color = glm::vec4( 1, 1, 1, 1 );
	};

	struct RenderPass;
	struct FrameBuffer;

	//================================================================================================================================
	//================================================================================================================================
	struct DrawPostprocess
	{
		Pipeline			mPipeline;
		Shader				mFragmentShader;
		Shader				mVertexShader;
		DescriptorImages	mDescriptorImage;
		DescriptorUniforms	mDescriptorUniform;
		Sampler				mSampler;
		UniformsPostprocess	mUniforms;
		CommandBuffer		mCommandBuffers;
		Buffer				mVertexBufferQuad;
 		
		void Create( Device& _device, uint32_t _imagesCount, ImageView& _inputImageView );
		void Destroy( Device& _device );
		void BindDescriptors( VkCommandBuffer _commandBuffer, const size_t _index );
		void UpdateUniformBuffers( Device& _device, const size_t _index );
		void CreateQuadVertexBuffer( Device& _device );
		void RecordCommandBuffer( const size_t _index, RenderPass& _renderPass, FrameBuffer& _framebuffer, VkExtent2D _extent );
		PipelineConfig	GetPipelineConfig() const;
	};
}	