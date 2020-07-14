#include "render/pipelines/fanUIPipeline.hpp"

#include "core/fanDebug.hpp"
#include "render/core/fanDevice.hpp"

namespace fan
{
	//================================================================================================================
	//================================================================================================================================
	UIPipeline::UIPipeline( Device& _device, DescriptorImages* _textures, DescriptorSampler* _sampler ) 
		: m_textures( _textures )
		, m_sampler( _sampler )
	{

		// Calculate required alignment based on minimum device offset alignment
		size_t minUboAlignment = (size_t)_device.mDeviceProperties.limits.minUniformBufferOffsetAlignment;
		size_t dynamicAlignmentVert = sizeof( DynamicUniformUIVert );
		if ( minUboAlignment > 0 )
		{
			dynamicAlignmentVert = ( ( sizeof( DynamicUniformUIVert ) + minUboAlignment - 1 ) & ~( minUboAlignment - 1 ) );
		}
		m_dynamicUniformsVert.SetAlignement( dynamicAlignmentVert );
		m_dynamicUniformsVert.Resize( 256 );
	}

	//================================================================================================================================
	//================================================================================================================================
	void UIPipeline::Destroy( Device& _device )
	{
		Pipeline::Destroy( _device );
		m_transformDescriptor.Destroy( _device );
	}

	//================================================================================================================================
	//================================================================================================================================
	void UIPipeline::BindDescriptors( VkCommandBuffer _commandBuffer, const size_t _indexFrame, const uint32_t _indexOffset )
	{
		std::vector<VkDescriptorSet> descriptors = {
			m_transformDescriptor.mDescriptorSets[_indexFrame]
		};
		std::vector<uint32_t> dynamicOffsets = {
			 _indexOffset * static_cast< uint32_t >( m_dynamicUniformsVert.Alignment() )
		};

		vkCmdBindDescriptorSets(
			_commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_pipelineLayout,
			0,
			static_cast< uint32_t >( descriptors.size() ),
			descriptors.data(),
			static_cast< uint32_t >( dynamicOffsets.size() ),
			dynamicOffsets.data()
		);

	}

	//================================================================================================================================
	//================================================================================================================================
	PipelineConfig UIPipeline::GetConfig()
	{
		PipelineConfig config( m_vertexShader, m_fragmentShader );

		config.bindingDescription = UIVertex::GetBindingDescription();
		config.attributeDescriptions = UIVertex::GetAttributeDescriptions();
		config.descriptorSetLayouts = {
			 m_transformDescriptor.mDescriptorSetLayout
			,m_textures->mDescriptorSetLayout
			, m_sampler->mDescriptorSetLayout
		};

		return config;
	}

	//================================================================================================================================
	//================================================================================================================================
	void UIPipeline::SetUniformsData( Device& _device, const size_t _index )
	{
		m_transformDescriptor.SetData( _device, 0, _index, &m_dynamicUniformsVert[ 0 ], m_dynamicUniformsVert.Alignment() * m_dynamicUniformsVert.Size(), 0 );
	}

	//================================================================================================================================
	//================================================================================================================================
	void UIPipeline::CreateDescriptors( Device& _device, const uint32_t _numSwapchainImages )
	{
		Debug::Log() << "UI pipeline : create descriptors" << Debug::Endl();
		m_transformDescriptor.AddDynamicUniformBinding( _device, _numSwapchainImages, VK_SHADER_STAGE_VERTEX_BIT, m_dynamicUniformsVert.Size(), m_dynamicUniformsVert.Alignment() );
		m_transformDescriptor.Create( _device, _numSwapchainImages );
	}

	//================================================================================================================================
	//================================================================================================================================
	void UIPipeline::ResizeDynamicDescriptors( Device& _device, const uint32_t _count, const size_t _newSize )
	{
		m_dynamicUniformsVert.Resize( _newSize );
		m_transformDescriptor.ResizeDynamicUniformBinding( _device, _count, m_dynamicUniformsVert.Size(), m_dynamicUniformsVert.Alignment(), 1 );
	}
}