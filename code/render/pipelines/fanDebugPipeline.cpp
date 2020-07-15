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
	DebugPipeline::DebugPipeline( Device& /*_device*/, const VkPrimitiveTopology _primitiveTopology, const bool _depthTestEnable ) 
		: m_primitiveTopology( _primitiveTopology )
		, m_depthTestEnable( _depthTestEnable )
	{}

	//================================================================================================================================
	//================================================================================================================================
	void DebugPipeline::Plop( Device& _device )
	{
		m_descriptor.Destroy( _device );
	}

	//================================================================================================================================
	//================================================================================================================================
	void DebugPipeline::CreateDescriptors( Device& _device, const uint32_t _numSwapchainImages )
	{
		Debug::Log( "Debug pipeline : create descriptors" );
		m_descriptor.AddUniformBinding( _device, _numSwapchainImages, VK_SHADER_STAGE_VERTEX_BIT, sizeof( DebugUniforms ) );
		m_descriptor.Create( _device, _numSwapchainImages );
	}

	//================================================================================================================================
	//================================================================================================================================
	void DebugPipeline::SetUniformsData( Device& _device, const size_t _index )
	{
		m_descriptor.SetData( _device, 0, _index, &m_debugUniforms, sizeof( DebugUniforms ), 0 );
	}

	//================================================================================================================================
	//================================================================================================================================
	void DebugPipeline::Bind( VkCommandBuffer _commandBuffer, VkExtent2D _extent, const size_t _index )
	{
		Pipeline::Bind( _commandBuffer, _extent,  _index );
		m_descriptor.Bind( _commandBuffer, m_pipelineLayout, _index );
	}

	//================================================================================================================================
	//================================================================================================================================
	PipelineConfig DebugPipeline::GetConfig( Shader& _vert, Shader& _frag )
	{
		PipelineConfig config( _vert, _frag );

		config.bindingDescription = DebugVertex::GetBindingDescription();
		config.attributeDescriptions = DebugVertex::GetAttributeDescriptions();
		config.inputAssemblyStateCreateInfo.topology = m_primitiveTopology;
		config.depthStencilStateCreateInfo.depthTestEnable = m_depthTestEnable ? VK_TRUE : VK_FALSE;
		config.descriptorSetLayouts = { m_descriptor.mDescriptorSetLayout };
		config.rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;

		return config;
	}
}