#include "render/draw/fanDrawUI.hpp"

#include "core/fanDebug.hpp"
#include "render/fanVertex.hpp"

namespace fan
{
 	//================================================================================================================================
	//================================================================================================================================
	void DrawUI::Create( Device& _device, uint32_t _imagesCount )
	{
		mUniforms.Create( _device.mDeviceProperties.limits.minUniformBufferOffsetAlignment );
		mSampler.Create( _device, 0, 1, VK_FILTER_NEAREST );
		mDescriptorSampler.Create( _device, mSampler.mSampler );

		mDescriptorTransform.AddDynamicUniformBinding( _device, _imagesCount, VK_SHADER_STAGE_VERTEX_BIT, mUniforms.mDynamicUniforms.Size(), mUniforms.mDynamicUniforms.Alignment() );
		mDescriptorTransform.Create( _device, _imagesCount );
	}

	//================================================================================================================================
	//================================================================================================================================
	void DrawUI::Destroy( Device& _device )
	{
		mDescriptorTransform.Destroy( _device );
		mDescriptorSampler.Destroy( _device );
		mSampler.Destroy( _device );
	}

	//================================================================================================================================
	//================================================================================================================================
	void DrawUI::BindDescriptors( VkCommandBuffer _commandBuffer, const size_t _indexFrame, const uint32_t _indexOffset )
	{
		std::vector<VkDescriptorSet> descriptors = {
			mDescriptorTransform.mDescriptorSets[_indexFrame]
		};
		std::vector<uint32_t> dynamicOffsets = {
			 _indexOffset * static_cast<uint32_t>( mUniforms.mDynamicUniforms.Alignment() )
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
	PipelineConfig DrawUI::GetPipelineConfig( DescriptorImages& _descriptorImages ) const
	{
		PipelineConfig config( mVertexShader, mFragmentShader );
		config.bindingDescription = UIVertex::GetBindingDescription();
		config.attributeDescriptions = UIVertex::GetAttributeDescriptions();
		config.descriptorSetLayouts = {
			 mDescriptorTransform.mDescriptorSetLayout
			,_descriptorImages.mDescriptorSetLayout
			,mDescriptorSampler.mDescriptorSetLayout
		};

		return config;
	}

	//================================================================================================================================
	//================================================================================================================================
	void DrawUI::UpdateUniformBuffers( Device& _device, const size_t _index )
	{
		mDescriptorTransform.SetData( _device, 0, _index, &mUniforms.mDynamicUniforms[0], mUniforms.mDynamicUniforms.Alignment() * mUniforms.mDynamicUniforms.Size(), 0 );
	}

	//================================================================================================================================
	// UiUniforms
	//================================================================================================================================
	void UniformsUI::Create( const VkDeviceSize _minUniformBufferOffsetAlignment )
	{
		// Calculate required alignment based on minimum device offset alignment
		size_t minUboAlignment = (size_t)_minUniformBufferOffsetAlignment;
		size_t dynamicAlignmentVert = sizeof( DynamicUniformUIVert );
		if( minUboAlignment > 0 )
		{
			dynamicAlignmentVert = ( ( sizeof( DynamicUniformUIVert ) + minUboAlignment - 1 ) & ~( minUboAlignment - 1 ) );
		}
		mDynamicUniforms.SetAlignement( dynamicAlignmentVert );
		mDynamicUniforms.Resize( 256 );
	}
}