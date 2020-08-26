#pragma once

#include <vector>
#include "fanDisableWarnings.hpp"
WARNINGS_GLM_PUSH()
#include "glm/glm.hpp"
WARNINGS_POP()
#include "render/core/fanPipeline.hpp"
#include "render/core/fanShader.hpp"
#include "render/core/fanBuffer.hpp"
#include "render/core/descriptors/fanDescriptorUniforms.hpp"
#include "render/core/descriptors/fanDescriptorImages.hpp"
#include "render/core/descriptors/fanDescriptorSampler.hpp"
#include "render/core/fanCommandBuffer.hpp"
#include "render/fanVertex.hpp"

namespace fan
{
	//================================================================
	//================================================================
	struct UniformsMVPColor
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
		glm::vec4 color;
	};

    //================================================================
    //================================================================
    struct UniformsScreenSize
    {
        glm::vec2 mScreenSize;
    };

	struct FrameBuffer;
	struct RenderPass;

	//================================================================================================================================
	// NDT = no depth test
	//================================================================================================================================
	struct DrawDebug
	{
		Pipeline		    mPipelineLines;
		Pipeline		    mPipelineLinesNDT;
		Pipeline            mPipelineTriangles;
        Pipeline            mPipelineLines2D;
		Shader              mFragmentShaderLines;
		Shader              mVertexShaderLines;
		Shader              mFragmentShaderLinesNDT;
		Shader              mVertexShaderLinesNDT;
		Shader              mFragmentShaderTriangles;
		Shader              mVertexShaderTriangles;
        Shader              mFragmentShaderLines2D;
        Shader              mVertexShaderLines2D;
		UniformsMVPColor    mUniformsMVPColor;
        UniformsScreenSize  mUniformsScreenSize;
		DescriptorUniforms  mDescriptorMVPColor;
        DescriptorUniforms  mDescriptorScreenSize;

		CommandBuffer		mCommandBuffers;
        CommandBuffer		mCommandBuffers2D;

		std::vector<Buffer>	mVertexBuffersLines;
		std::vector<Buffer>	mVertexBuffersLinesNDT;
		std::vector<Buffer>	mVertexBuffersTriangles;
		std::vector<Buffer>	mVertexBuffersLines2D;
		int mNumLines = 0;
		int mNumLinesNDT = 0;
		int mNumTriangles = 0;
		int mNumLines2D = 0;

		void Create( Device& _device, uint32_t _imagesCount );
		void Destroy( Device& _device );
		void UpdateUniformBuffers( Device& _device, const size_t _index );
        bool HasNothingToDraw() const
        {
            return mNumLines == 0 &&
                   mNumLinesNDT == 0 &&
                   mNumTriangles == 0;
        }
		void RecordCommandBuffer( const size_t _index,
								  RenderPass& _renderPass,
								  FrameBuffer& _framebuffer,
								  VkExtent2D _extent );
        void RecordCommandBuffer2D( const size_t _index,
                                  RenderPass& _renderPass,
                                  FrameBuffer& _framebuffer,
                                  VkExtent2D _extent );
		void SetDebugDrawData( const uint32_t _index,
							   Device& _device,
							   const std::vector<DebugVertex>& _debugLines,
							   const std::vector<DebugVertex>& _debugLinesNoDepthTest,
							   const std::vector<DebugVertex>& _debugTriangles,
							   const std::vector<DebugVertex2D>& _debugLines2D  );

		PipelineConfig GetPipelineConfigLines() const;
		PipelineConfig GetPipelineConfigLinesNDT() const;
		PipelineConfig GetPipelineConfigTriangles() const;
        PipelineConfig GetPipelineConfigLines2D() const;
	};
}