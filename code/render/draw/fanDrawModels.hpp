#pragma once

#include "fanDisableWarnings.hpp"
WARNINGS_GLM_PUSH()
#include "glm/glm.hpp"
WARNINGS_POP()
#include "render/fanRenderGlobal.hpp"
#include "render/core/fanPipeline.hpp"
#include "render/core/fanShader.hpp"
#include "render/core/fanSampler.hpp"
#include "render/core/descriptors/fanDescriptorUniforms.hpp"
#include "render/core/descriptors/fanDescriptorImages.hpp"
#include "render/core/descriptors/fanDescriptorSampler.hpp"
#include "render/core/fanCommandBuffer.hpp"

namespace fan
{
	class Mesh;

	//================================================================
	//================================================================
	struct RenderDataModel
	{
		Mesh* mesh;
		glm::mat4 modelMatrix;
		glm::mat4 normalMatrix;
		glm::vec4 color;
		uint32_t shininess;
		uint32_t textureIndex;
	};

	//================================================================
	//================================================================
	struct RenderDataDirectionalLight
	{
		glm::vec4 direction;
		glm::vec4 ambiant;
		glm::vec4 diffuse;
		glm::vec4 specular;
	};

	//================================================================
	//================================================================
	struct RenderDataPointLight
	{
		glm::vec4	 position;
		glm::vec4	 diffuse;
		glm::vec4	 specular;
		glm::vec4	 ambiant;
		glm::float32 constant;
		glm::float32 linear;
		glm::float32 quadratic;
	};

	//================================================================
	//================================================================
	struct UniformDirectionalLight
	{
		alignas( 16 ) glm::vec4 direction;
		alignas( 16 ) glm::vec4 ambiant;
		alignas( 16 ) glm::vec4 diffuse;
		alignas( 16 ) glm::vec4 specular;
	};

	//================================================================
	//================================================================
	struct UniformPointLight
	{
		alignas( 16 ) glm::vec4		position;
		alignas( 16 ) glm::vec4		diffuse;
		alignas( 16 ) glm::vec4		specular;
		alignas( 16 ) glm::vec4		ambiant;
		alignas( 4 ) glm::float32	constant;
		alignas( 4 ) glm::float32	linear;
		alignas( 4 ) glm::float32	quadratic;
		alignas( 4 ) glm::float32	_0;
	};

	//================================================================
	//================================================================
	struct UniformLights
	{
		UniformDirectionalLight dirLights[RenderGlobal::sMaximumNumDirectionalLight];
		UniformPointLight		pointlights[RenderGlobal::sMaximumNumPointLights];
		uint32_t				dirLightsNum;
		uint32_t				pointLightNum;
	};

	//================================================================
	//================================================================
	struct DynamicUniformMatrices
	{
		glm::mat4 modelMat;
		glm::mat4 normalMat;
	};

	//================================================================
	//================================================================
	struct UniformCameraPosition
	{
		glm::vec3 cameraPosition = glm::vec3( 0, 0, 0 );
	};

	//================================================================
	//================================================================
	struct UniformViewProj
	{
		glm::mat4 view;
		glm::mat4 proj;
	};

	//================================================================
	// material data for drawing a mesh
	//================================================================
	struct DynamicUniformsMaterial
	{
		glm::vec4  color = glm::vec4( 1 );
		glm::int32 shininess;
	};

	//================================================================
	// All uniforms data for the models drawing
	//================================================================
	struct UniformsModelDraw
	{
		void Create( const VkDeviceSize _minUniformBufferOffsetAlignment );

		AlignedMemory<DynamicUniformsMaterial>	mDynamicUniformsMaterial;
		AlignedMemory<DynamicUniformMatrices>	mDynamicUniformsMatrices;
		UniformLights							mUniformsLights;
		UniformViewProj							mUniformsProjView;
		UniformCameraPosition					mUniformsCameraPosition;
	};

	//================================================================
	// references a mesh that must be drawn on the screen
	//================================================================
	struct DrawData
	{
		Mesh*		mesh;
		uint32_t	textureIndex;
	};

	struct RenderPass;
	struct FrameBuffer;
	struct DescriptorImages;

	//================================================================================================================================
	// Models drawing data for the rendering engine
	//================================================================================================================================
	struct DrawModels
	{
		Pipeline				mPipeline;
		Shader					mFragmentShader;
		Shader					mVertexShader;
		UniformsModelDraw		mUniforms;
		DescriptorUniforms		mDescriptorUniforms;
		DescriptorSampler		mDescriptorSampler;
		Sampler					mSamplerTextures;
		CommandBuffer			mCommandBuffers;
		std::vector< DrawData >	mDrawData;
		
		void Create( Device& _device, uint32_t _imagesCount );
		void Destroy( Device& _device );
		void BindDescriptors( VkCommandBuffer _commandBuffer, const size_t _indexFrame, const uint32_t _indexOffset );
		void UpdateUniformBuffers( Device& _device, const size_t _index );
		void RecordCommandBuffer( const size_t _index, RenderPass& _renderPass, FrameBuffer& _framebuffer, VkExtent2D _extent, DescriptorImages& _descriptorTextures );
		void BindTexture( VkCommandBuffer _commandBuffer, const uint32_t _textureIndex, DescriptorSampler& _descriptorSampler, DescriptorImages& _descriptorTextures, VkPipelineLayout _pipelineLayout );
		void SetDrawData( Device& _device, const uint32_t _imagesCount, const std::vector<RenderDataModel>& _drawData );
		void SetPointLights( const std::vector<RenderDataPointLight>& _lightData );
		void SetDirectionalLights( const std::vector<RenderDataDirectionalLight>& _lightData );
		PipelineConfig	GetPipelineConfig( DescriptorImages& _imagesDescriptor ) const;
	};
}