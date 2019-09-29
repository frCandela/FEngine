#include "fanGlobalIncludes.h"

#include "renderer/pipelines/fanDebugPipeline.h"
#include "renderer/core/fanDevice.h"
#include "renderer/core/fanShader.h"
#include "renderer/core/fanImage.h"
#include "renderer/core/fanImageView.h" 
#include "renderer/core/fanBuffer.h"
#include "renderer/core/fanDescriptor.h"
#include "renderer/fanRenderer.h"
#include "renderer/util/fanVertex.h"
#include "renderer/fanUniforms.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	DebugPipeline::DebugPipeline(Device& _device,const VkPrimitiveTopology _primitiveTopology, const bool _depthTestEnable) :
		Pipeline( _device )
		, m_primitiveTopology(_primitiveTopology)
		, m_depthTestEnable(_depthTestEnable)
	{
		m_descriptor = new Descriptor( m_device );
		m_descriptor->SetUniformBinding( VK_SHADER_STAGE_VERTEX_BIT, sizeof( DebugUniforms ) );
		m_descriptor->Create();
	}

	//================================================================================================================================
	//================================================================================================================================
	DebugPipeline::~DebugPipeline() {
		delete m_descriptor;
	}

	//================================================================================================================================
	//================================================================================================================================
	void DebugPipeline::UpdateUniformBuffers() {
		m_descriptor->SetBinding( 0, &debugUniforms, sizeof( DebugUniforms ) );
	}

	//================================================================================================================================
	//================================================================================================================================
	void DebugPipeline::Bind( VkCommandBuffer _commandBuffer ) {
		Pipeline::Bind(		_commandBuffer );
		m_descriptor->Bind( _commandBuffer, m_pipelineLayout );
	}

	//================================================================================================================================
	//================================================================================================================================
	void DebugPipeline::ConfigurePipeline() {
		m_bindingDescription =  DebugVertex::GetBindingDescription();
		m_attributeDescriptions = DebugVertex::GetAttributeDescriptions();
		m_inputAssemblyStateCreateInfo.topology = m_primitiveTopology;
		m_depthStencilStateCreateInfo.depthTestEnable = m_depthTestEnable ? VK_TRUE : VK_FALSE;
		m_descriptorSetLayouts= { m_descriptor->GetLayout() };		
	}
}