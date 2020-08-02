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
	struct Mesh2D;

	//================================================================
	//================================================================
	struct RenderDataUIMesh
	{
		Mesh2D* mesh;
		glm::vec2 position;
		glm::vec2 scale;
		glm::vec4 color;
		uint32_t textureIndex;
	};

	//================================================================
	//================================================================
	struct DynamicUniformUIVert
	{
		glm::vec2 position;
		glm::vec2 scale;
		glm::vec4 color;
	};

	//================================================================
	//================================================================
	struct UniformsUI
	{
		void Create( const VkDeviceSize _minUniformBufferOffsetAlignment );

		AlignedMemory<DynamicUniformUIVert>	mDynamicUniforms;
	};

	//================================================================
	//================================================================
	struct UIDrawData
	{
		Mesh2D* mesh;
		uint32_t textureIndex;
	};

	struct FrameBuffer;
	struct RenderPass;

	//================================================================================================================================
	//================================================================================================================================
	struct DrawUI
	{
		Pipeline					mPipeline;
		Shader						mFragmentShader;
		Shader						mVertexShader;
		DescriptorUniforms			mDescriptorTransform;
		UniformsUI					mUniforms;
		DescriptorSampler			mDescriptorSampler;
		Sampler						mSampler;
		CommandBuffer				mCommandBuffers;
		std::vector< UIDrawData >	mDrawData;

		void Create( Device& _device, uint32_t _imagesCount );
		void Destroy( Device& _device );
		void BindDescriptors( VkCommandBuffer _commandBuffer, const size_t _indexFrame, const uint32_t _indexOffset );
		void UpdateUniformBuffers( Device& _device, const size_t _index );
		void RecordCommandBuffer( const size_t _index, RenderPass& _renderPass, FrameBuffer& _framebuffer, VkExtent2D _extent, DescriptorImages& _descriptorTextures );
		void BindTexture( VkCommandBuffer _commandBuffer, const uint32_t _textureIndex, DescriptorSampler& _descriptorSampler, DescriptorImages& _descriptorTextures, VkPipelineLayout _pipelineLayout );
		void SetUIDrawData( const std::vector<RenderDataUIMesh>& _drawData );
		PipelineConfig	GetPipelineConfig( DescriptorImages& _descriptorImages ) const;
	};
}