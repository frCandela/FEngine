#include "render/draw/fanDrawModels.hpp"

#include "core/fanDebug.hpp"
#include "render/fanVertex.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void DrawModels::Create( Device& _device, uint32_t _imagesCount )
	{
		const size_t initialDrawDataSize = 256;
		mDrawData.reserve( initialDrawDataSize );
		mUniforms.Create( _device.mDeviceProperties.limits.minUniformBufferOffsetAlignment );
		mUniforms.m_dynamicUniformsVert.Resize( initialDrawDataSize );
		mUniforms.m_dynamicUniformsMaterial.Resize( initialDrawDataSize );

		mDescriptorUniforms.AddUniformBinding( _device, _imagesCount, VK_SHADER_STAGE_VERTEX_BIT, sizeof( VertUniforms ) );
		mDescriptorUniforms.AddDynamicUniformBinding( _device, _imagesCount, VK_SHADER_STAGE_VERTEX_BIT, mUniforms.m_dynamicUniformsVert.Size(), mUniforms.m_dynamicUniformsVert.Alignment() );
		mDescriptorUniforms.AddUniformBinding( _device, _imagesCount, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof( FragUniforms ) );
		mDescriptorUniforms.AddDynamicUniformBinding( _device, _imagesCount, VK_SHADER_STAGE_FRAGMENT_BIT, mUniforms.m_dynamicUniformsMaterial.Size(), mUniforms.m_dynamicUniformsMaterial.Alignment() );
		mDescriptorUniforms.AddUniformBinding( _device, _imagesCount, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof( LightsUniforms ) );
		mDescriptorUniforms.Create( _device, _imagesCount );
	}

	//================================================================================================================================
	//================================================================================================================================
	void DrawModels::Destroy( Device& _device )
	{
		mPipeline.Destroy( _device );
		mDescriptorUniforms.Destroy( _device );
	}

	//================================================================================================================================
	//================================================================================================================================
	PipelineConfig DrawModels::GetPipelineConfig( DescriptorImages& _imagesDescriptor, DescriptorSampler& _samplerDescriptor ) const
	{
		PipelineConfig config( mVertexShader, mFragmentShader );

		config.bindingDescription = Vertex::GetBindingDescription();
		config.attributeDescriptions = Vertex::GetAttributeDescriptions();
		config.descriptorSetLayouts = {
			mDescriptorUniforms.mDescriptorSetLayout
			, _imagesDescriptor.mDescriptorSetLayout
			, _samplerDescriptor.mDescriptorSetLayout
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
			 _indexOffset * static_cast<uint32_t>( mUniforms.m_dynamicUniformsVert.Alignment() )
			,_indexOffset* static_cast<uint32_t>( mUniforms.m_dynamicUniformsMaterial.Alignment() )
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
		mDescriptorUniforms.SetData( _device, 0, _index, &mUniforms.m_vertUniforms, sizeof( VertUniforms ), 0 );
		mDescriptorUniforms.SetData( _device, 1, _index, &mUniforms.m_dynamicUniformsVert[0], mUniforms.m_dynamicUniformsVert.Alignment() * mUniforms.m_dynamicUniformsVert.Size(), 0 );
		mDescriptorUniforms.SetData( _device, 2, _index, &mUniforms.m_fragUniforms, sizeof( FragUniforms ), 0 );
		mDescriptorUniforms.SetData( _device, 3, _index, &mUniforms.m_dynamicUniformsMaterial[0], mUniforms.m_dynamicUniformsMaterial.Alignment() * mUniforms.m_dynamicUniformsMaterial.Size(), 0 );
		mDescriptorUniforms.SetData( _device, 4, _index, &mUniforms.m_lightUniforms, sizeof( LightsUniforms ), 0 );		
	}
}