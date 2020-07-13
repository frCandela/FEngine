#include "render/pipelines/fanDebugPipeline.hpp"

#include "core/fanDebug.hpp"
#include "render/core/fanDevice.hpp"
#include "render/core/fanShader.hpp"
#include "render/core/fanImage.hpp"
#include "render/core/fanImageView.hpp" 
#include "render/core/fanBuffer.hpp"
#include "render/fanVertex.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	DebugPipeline::DebugPipeline( Device& _device, const VkPrimitiveTopology _primitiveTopology, const bool _depthTestEnable ) :
		Pipeline( _device )
		, m_primitiveTopology( _primitiveTopology )
		, m_depthTestEnable( _depthTestEnable )
	{}

	//================================================================================================================================
	//================================================================================================================================
	DebugPipeline::~DebugPipeline()
	{
		m_descriptor.Destroy( m_device );
	}

	//================================================================================================================================
	//================================================================================================================================
	void DebugPipeline::CreateDescriptors( const uint32_t _numSwapchainImages )
	{
		Debug::Log( "Debug pipeline : create descriptors" );
		m_descriptor.AddUniformBinding( m_device, _numSwapchainImages, VK_SHADER_STAGE_VERTEX_BIT, sizeof( DebugUniforms ) );
		m_descriptor.Create( m_device, _numSwapchainImages );
	}

	//================================================================================================================================
	//================================================================================================================================
	void DebugPipeline::SetUniformsData( const size_t _index )
	{
		m_descriptor.SetData( m_device, 0, _index, &m_debugUniforms, sizeof( DebugUniforms ), 0 );
	}

	//================================================================================================================================
	//================================================================================================================================
	void DebugPipeline::Bind( VkCommandBuffer _commandBuffer, const size_t _index )
	{
		Pipeline::Bind( _commandBuffer, _index );
		m_descriptor.Bind( _commandBuffer, m_pipelineLayout, _index );
	}

	//================================================================================================================================
	//================================================================================================================================
	void DebugPipeline::ConfigurePipeline()
	{
		m_bindingDescription = DebugVertex::GetBindingDescription();
		m_attributeDescriptions = DebugVertex::GetAttributeDescriptions();
		m_inputAssemblyStateCreateInfo.topology = m_primitiveTopology;
		m_depthStencilStateCreateInfo.depthTestEnable = m_depthTestEnable ? VK_TRUE : VK_FALSE;
		m_descriptorSetLayouts = { m_descriptor.mDescriptorSetLayout };
		m_rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
	}
}