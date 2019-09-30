#include "fanGlobalIncludes.h"

#include "renderer/pipelines/fanPostprocessPipeline.h"
#include "renderer/core/fanDevice.h"
#include "renderer/core/fanShader.h"
#include "renderer/core/fanBuffer.h"
#include "renderer/core/fanDescriptor.h"
#include "renderer/core/fanImageView.h"
#include "renderer/core/fanSampler.h"
#include "renderer/fanRenderer.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	PostprocessPipeline::PostprocessPipeline( Device& _device ) :
		Pipeline(_device)		 
	{
		uniforms.color = glm::vec4(1, 1, 1, 1);
	}

	//================================================================================================================================
	//================================================================================================================================
	PostprocessPipeline::~PostprocessPipeline() {
		delete m_descriptor;
	}

	//================================================================================================================================
	//================================================================================================================================
	void PostprocessPipeline::Resize( const VkExtent2D _extent ) {
		Pipeline::Resize(_extent);
		std::vector<VkImageView> views = { m_imageView->GetImageView() };
		m_descriptor->SetImageSamplerBinding( VK_SHADER_STAGE_FRAGMENT_BIT, views, m_sampler->GetSampler(), 0 );
		m_descriptor->Update();
	}

	//================================================================================================================================
	//================================================================================================================================
	void PostprocessPipeline::Bind( VkCommandBuffer _commandBuffer ) {
		Pipeline::Bind(_commandBuffer);
		m_descriptor->Bind( _commandBuffer, m_pipelineLayout );
	}

	//================================================================================================================================
	//================================================================================================================================
	void PostprocessPipeline::ReloadShaders( ) {
		Pipeline::ReloadShaders( );
		std::vector<VkImageView> views = { m_imageView->GetImageView() };
		m_descriptor->SetImageSamplerBinding( VK_SHADER_STAGE_FRAGMENT_BIT, views, m_sampler->GetSampler(), 0 );
		m_descriptor->Update();
	}

	//================================================================================================================================
	//================================================================================================================================
	void PostprocessPipeline::UpdateUniformBuffers() {
		m_descriptor->SetBinding(1, &uniforms, sizeof( Uniforms ) );
	}

	//================================================================================================================================
	//================================================================================================================================
	void PostprocessPipeline::CreateDescriptors() {
		delete m_descriptor;
		m_descriptor = new Descriptor( m_device );
		std::vector<VkImageView> views = { m_imageView->GetImageView() };
		m_descriptor->SetImageSamplerBinding( VK_SHADER_STAGE_FRAGMENT_BIT , views, m_sampler->GetSampler() );
		m_descriptor->SetUniformBinding( VK_SHADER_STAGE_FRAGMENT_BIT, sizeof( Uniforms ) );
		m_descriptor->Create();		
		UpdateUniformBuffers();
	}

	//================================================================================================================================
	//================================================================================================================================
	void PostprocessPipeline::ConfigurePipeline() {

		m_bindingDescription.resize(1);
		m_bindingDescription[0].binding = 0;								
		m_bindingDescription[0].stride = sizeof(glm::vec3);					
		m_bindingDescription[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		m_attributeDescriptions.resize(1);
		m_attributeDescriptions[0].binding = 0;
		m_attributeDescriptions[0].location = 0;
		m_attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		m_attributeDescriptions[0].offset = 0;
		m_inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		m_rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
		m_attachmentBlendStates[0].srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		m_attachmentBlendStates[0].dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		m_descriptorSetLayouts = { m_descriptor->GetLayout()	};
	}

	//================================================================================================================================
	//================================================================================================================================
	void PostprocessPipeline::SetImageAndView( ImageView* _imageView, Sampler* _sampler ) { 
		m_sampler = _sampler;
		m_imageView = _imageView;
	}
}