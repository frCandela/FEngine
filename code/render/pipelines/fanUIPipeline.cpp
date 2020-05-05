#include "render/pipelines/fanUIPipeline.hpp"

#include "core/fanDebug.hpp"
#include "render/core/fanDevice.hpp"
#include "render/descriptors/fanDescriptor.hpp"
#include "render/descriptors/fanDescriptorTexture.hpp"
#include "render/descriptors/fanDescriptorSampler.hpp"

namespace fan
{
	//================================================================================================================
	//================================================================================================================================
	UIPipeline::UIPipeline( Device& _device, DescriptorTextures*& _textures, DescriptorSampler*& _sampler ) :
		Pipeline( _device )
		, m_textures( _textures )
		, m_sampler( _sampler )
	{

		// Calculate required alignment based on minimum device offset alignment
		size_t minUboAlignment = (size_t)m_device.GetDeviceProperties().limits.minUniformBufferOffsetAlignment;
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
	UIPipeline::~UIPipeline()
	{
		delete m_transformDescriptor;
	}

	//================================================================================================================================
	//================================================================================================================================
	void UIPipeline::BindDescriptors( VkCommandBuffer _commandBuffer, const size_t _indexFrame, const uint32_t _indexOffset )
	{
		std::vector<VkDescriptorSet> descriptors = {
			m_transformDescriptor->GetSet( _indexFrame )
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
	void UIPipeline::ConfigurePipeline()
	{
		m_bindingDescription = UIVertex::GetBindingDescription();
		m_attributeDescriptions = UIVertex::GetAttributeDescriptions();

		m_descriptorSetLayouts = {
			 m_transformDescriptor->GetLayout()
			,m_textures->GetLayout()
			, m_sampler->GetLayout()
		};
	}

	//================================================================================================================================
	//================================================================================================================================
	void UIPipeline::UpdateUniformBuffers( const size_t _index )
	{
		m_transformDescriptor->SetBinding( 0, _index, &m_dynamicUniformsVert[ 0 ], m_dynamicUniformsVert.Alignment() * m_dynamicUniformsVert.Size(), 0 );
	}

	//================================================================================================================================
	//================================================================================================================================
	void UIPipeline::CreateDescriptors( const size_t _numSwapchainImages )
	{
		Debug::Log() << "UI pipeline : create descriptors" << Debug::Endl();
		m_transformDescriptor = new Descriptor( m_device, _numSwapchainImages );
		m_transformDescriptor->SetDynamicUniformBinding( VK_SHADER_STAGE_VERTEX_BIT, m_dynamicUniformsVert.Size(), m_dynamicUniformsVert.Alignment() );
		m_transformDescriptor->Create();
	}

	//================================================================================================================================
	//================================================================================================================================
	void UIPipeline::ResizeDynamicDescriptors( const size_t _newSize )
	{
		m_dynamicUniformsVert.Resize( _newSize );
		m_transformDescriptor->SetDynamicUniformBinding( VK_SHADER_STAGE_VERTEX_BIT, m_dynamicUniformsVert.Size(), m_dynamicUniformsVert.Alignment(), 1 );
	}

}