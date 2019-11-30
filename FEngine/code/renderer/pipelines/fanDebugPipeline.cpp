#include "fanGlobalIncludes.h"

#include "renderer/pipelines/fanDebugPipeline.h"
#include "renderer/core/fanDevice.h"
#include "renderer/core/fanShader.h"
#include "renderer/core/fanImage.h"
#include "renderer/core/fanImageView.h" 
#include "renderer/core/fanBuffer.h"
#include "renderer/descriptors/fanDescriptor.h"
#include "renderer/util/fanVertex.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	DebugPipeline::DebugPipeline(Device& _device,const VkPrimitiveTopology _primitiveTopology, const bool _depthTestEnable) :
		Pipeline( _device )
		, m_primitiveTopology(_primitiveTopology)
		, m_depthTestEnable(_depthTestEnable)
	{}

	//================================================================================================================================
	//================================================================================================================================
	DebugPipeline::~DebugPipeline() {
		delete m_descriptor;
	}

	//================================================================================================================================
	//================================================================================================================================
	void DebugPipeline::CreateDescriptors( const size_t _numSwapchainImages ) {
		m_descriptor = new Descriptor( m_device, _numSwapchainImages );
		m_descriptor->SetUniformBinding( VK_SHADER_STAGE_VERTEX_BIT, sizeof( DebugUniforms ) );
		m_descriptor->Create();
	}

	//================================================================================================================================
	//================================================================================================================================
	void DebugPipeline::UpdateUniformBuffers( const size_t _index ) {
		m_descriptor->SetBinding( 0, _index, &m_debugUniforms, sizeof( DebugUniforms ), 0 );
	}

	//================================================================================================================================
	//================================================================================================================================
	void DebugPipeline::Bind( VkCommandBuffer _commandBuffer, const size_t _index ) {
		Pipeline::Bind(	_commandBuffer, _index );
		m_descriptor->Bind( _commandBuffer, m_pipelineLayout, _index );
	}

	//================================================================================================================================
	//================================================================================================================================
	void DebugPipeline::ConfigurePipeline() {
		m_bindingDescription =  DebugVertex::GetBindingDescription();
		m_attributeDescriptions = DebugVertex::GetAttributeDescriptions();
		m_inputAssemblyStateCreateInfo.topology = m_primitiveTopology;
		m_depthStencilStateCreateInfo.depthTestEnable = m_depthTestEnable ? VK_TRUE : VK_FALSE;
		m_descriptorSetLayouts= { m_descriptor->GetLayout() };		
		m_rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
	}
}