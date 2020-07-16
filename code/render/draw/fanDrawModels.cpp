#include "render/draw/fanDrawModels.hpp"

#include "core/fanDebug.hpp"
#include "render/fanVertex.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void DrawModels::Create( Device& _device, uint32_t _imagesCount )
	{
		mSamplerTextures.Create( _device, 0, 8, VK_FILTER_LINEAR );
		mDescriptorSampler.Create( _device, mSamplerTextures.mSampler );

		const size_t initialDrawDataSize = 256;
		mDrawData.reserve( initialDrawDataSize );
		mUniforms.Create( _device.mDeviceProperties.limits.minUniformBufferOffsetAlignment );
		mUniforms.mDynamicUniformsMatrices.Resize( initialDrawDataSize );
		mUniforms.mDynamicUniformsMaterial.Resize( initialDrawDataSize );

		mDescriptorUniforms.AddUniformBinding( _device, _imagesCount, VK_SHADER_STAGE_VERTEX_BIT, sizeof( UniformViewProj ) );
		mDescriptorUniforms.AddDynamicUniformBinding( _device, _imagesCount, VK_SHADER_STAGE_VERTEX_BIT, mUniforms.mDynamicUniformsMatrices.Size(), mUniforms.mDynamicUniformsMatrices.Alignment() );
		mDescriptorUniforms.AddUniformBinding( _device, _imagesCount, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof( UniformCameraPosition ) );
		mDescriptorUniforms.AddDynamicUniformBinding( _device, _imagesCount, VK_SHADER_STAGE_FRAGMENT_BIT, mUniforms.mDynamicUniformsMaterial.Size(), mUniforms.mDynamicUniformsMaterial.Alignment() );
		mDescriptorUniforms.AddUniformBinding( _device, _imagesCount, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof( UniformLights ) );
		mDescriptorUniforms.Create( _device, _imagesCount );
	}

	//================================================================================================================================
	//================================================================================================================================
	void DrawModels::Destroy( Device& _device )
	{
		mPipeline.Destroy( _device );
		mDescriptorUniforms.Destroy( _device );
		mDescriptorSampler.Destroy( _device );
		mSamplerTextures.Destroy( _device );
	}

	//================================================================================================================================
	//================================================================================================================================
	PipelineConfig DrawModels::GetPipelineConfig( DescriptorImages& _imagesDescriptor ) const
	{
		PipelineConfig config( mVertexShader, mFragmentShader );

		config.bindingDescription = Vertex::GetBindingDescription();
		config.attributeDescriptions = Vertex::GetAttributeDescriptions();
		config.descriptorSetLayouts = {
			mDescriptorUniforms.mDescriptorSetLayout
			, _imagesDescriptor.mDescriptorSetLayout
			, mDescriptorSampler.mDescriptorSetLayout
		};

		return config;
	}

	//================================================================================================================================
	//================================================================================================================================
	void DrawModels::BindDescriptors( VkCommandBuffer _commandBuffer, const size_t _indexFrame, const uint32_t _indexOffset )
	{
		std::vector<VkDescriptorSet> descriptors = {
			mDescriptorUniforms.mDescriptorSets[_indexFrame]
		};
		std::vector<uint32_t> dynamicOffsets = {
			 _indexOffset * static_cast<uint32_t>( mUniforms.mDynamicUniformsMatrices.Alignment() )
			,_indexOffset* static_cast<uint32_t>( mUniforms.mDynamicUniformsMaterial.Alignment() )
		};
		vkCmdBindDescriptorSets(
			_commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			mPipeline.mPipelineLayout,
			0,
			static_cast<uint32_t>( descriptors.size() ),
			descriptors.data(),
			static_cast<uint32_t>( dynamicOffsets.size() ),
			dynamicOffsets.data()
		);
	}

	//================================================================================================================================
	//================================================================================================================================
	void DrawModels::UpdateUniformBuffers( Device& _device, const size_t _index )
	{
		mDescriptorUniforms.SetData( _device, 0, _index, &mUniforms.mUniformsProjView, sizeof( UniformViewProj ), 0 );
		mDescriptorUniforms.SetData( _device, 1, _index, &mUniforms.mDynamicUniformsMatrices[0], mUniforms.mDynamicUniformsMatrices.Alignment() * mUniforms.mDynamicUniformsMatrices.Size(), 0 );
		mDescriptorUniforms.SetData( _device, 2, _index, &mUniforms.mUniformsCameraPosition, sizeof( UniformCameraPosition ), 0 );
		mDescriptorUniforms.SetData( _device, 3, _index, &mUniforms.mDynamicUniformsMaterial[0], mUniforms.mDynamicUniformsMaterial.Alignment() * mUniforms.mDynamicUniformsMaterial.Size(), 0 );
		mDescriptorUniforms.SetData( _device, 4, _index, &mUniforms.mUniformsLights, sizeof( UniformLights ), 0 );		
	}

	//================================================================================================================================
	//================================================================================================================================
	void UniformsModelDraw::Create( const VkDeviceSize _minUniformBufferOffsetAlignment )
	{
		// Calculate required alignment based on minimum device offset alignment
		size_t minUboAlignment = (size_t)_minUniformBufferOffsetAlignment;
		size_t dynamicAlignmentVert = sizeof( DynamicUniformMatrices );
		size_t dynamicAlignmentFrag = sizeof( DynamicUniformsMaterial );
		if( minUboAlignment > 0 )
		{
			dynamicAlignmentVert = ( ( sizeof( DynamicUniformMatrices ) + minUboAlignment - 1 ) & ~( minUboAlignment - 1 ) );
			dynamicAlignmentFrag = ( ( sizeof( DynamicUniformsMaterial ) + minUboAlignment - 1 ) & ~( minUboAlignment - 1 ) );
		}

		mDynamicUniformsMatrices.SetAlignement( dynamicAlignmentVert );
		mDynamicUniformsMaterial.SetAlignement( dynamicAlignmentFrag );

		mDynamicUniformsMatrices.Resize( 256 );
		mDynamicUniformsMaterial.Resize( 256 );

		for( int uniformIndex = 0; uniformIndex < mDynamicUniformsMaterial.Size(); uniformIndex++ )
		{
			mDynamicUniformsMaterial[uniformIndex].color = glm::vec4( 1 );
			mDynamicUniformsMaterial[uniformIndex].shininess = 1;
		}
	}
}