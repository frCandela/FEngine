#include "fanGlobalIncludes.h"
#include "renderer/pipelines/fanUIPipeline.h"

#include "renderer/core/fanDevice.h"
#include "renderer/core/fanDescriptorTexture.h"
#include "renderer/core/fanSampler.h"
#include "renderer/core/fanDescriptor.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	UIPipeline::UIPipeline( Device& _device ) :
		Pipeline( _device )
	{
		m_sampler = new Sampler( m_device );
		m_sampler->CreateSampler( 0, 8 );

		// Calculate required alignment based on minimum device offset alignment
		size_t minUboAlignment = m_device.GetDeviceProperties().limits.minUniformBufferOffsetAlignment;
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
		delete m_sampler;
		delete m_transformDescriptor;
		delete m_descriptorImageSampler;
	}

	//================================================================================================================================
	//================================================================================================================================
	void UIPipeline::BindDescriptors( VkCommandBuffer _commandBuffer, const size_t _indexFrame, const uint32_t _indexOffset )
	{
		std::vector<VkDescriptorSet> descriptors = {
			m_transformDescriptor->GetSet( _indexFrame )
		};
		std::vector<uint32_t> dynamicOffsets = {
			 _indexOffset  * static_cast<uint32_t>( m_dynamicUniformsVert.Alignment() )
		};

		vkCmdBindDescriptorSets(
			_commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_pipelineLayout,
			0,
			static_cast<uint32_t>( descriptors.size() ),
			descriptors.data(),
			static_cast<uint32_t>( dynamicOffsets.size() ),
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
		};
	}

	//================================================================================================================================
	//================================================================================================================================
	void UIPipeline::UpdateUniformBuffers( const size_t _index )
	{
		m_transformDescriptor->SetBinding( 0, _index, &m_dynamicUniformsVert[0], m_dynamicUniformsVert.Alignment() * m_dynamicUniformsVert.Size(),		0 );
	}

	//================================================================================================================================
	//================================================================================================================================
	void UIPipeline::CreateDescriptors( const size_t _numSwapchainImages )
	{
		m_descriptorImageSampler = new DescriptorTextures( m_device, m_sampler->GetSampler(), 1 );

		m_transformDescriptor = new Descriptor( m_device, _numSwapchainImages );
		m_transformDescriptor->SetDynamicUniformBinding ( VK_SHADER_STAGE_VERTEX_BIT,	m_dynamicUniformsVert.Size(), m_dynamicUniformsVert.Alignment() );
		m_transformDescriptor->Create();
	}

	//================================================================================================================================
	//================================================================================================================================
	void UIPipeline::ResizeDynamicDescriptors( const size_t _newSize )
	{
		m_dynamicUniformsVert.Resize( _newSize );
		m_transformDescriptor->SetDynamicUniformBinding ( VK_SHADER_STAGE_VERTEX_BIT, m_dynamicUniformsVert.Size(), m_dynamicUniformsVert.Alignment(), 1 );
	}

}