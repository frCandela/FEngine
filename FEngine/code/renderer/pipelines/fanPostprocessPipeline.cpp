#include "fanGlobalIncludes.h"

#include "renderer/pipelines/fanPostprocessPipeline.h"
#include "renderer/core/fanDevice.h"
#include "renderer/core/fanShader.h"
#include "renderer/core/fanBuffer.h"
#include "renderer/descriptors/fanDescriptorTexture.h"
#include "renderer/descriptors/fanDescriptor.h"
#include "renderer/core/fanImageView.h"
#include "renderer/core/fanSampler.h"

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
		delete m_descriptorImageSampler;
		delete m_descriptorUniforms;
	}

	//================================================================================================================================
	//================================================================================================================================
	void PostprocessPipeline::Resize( const VkExtent2D _extent ) {
		Pipeline::Resize(_extent);
		m_descriptorImageSampler->Set(0, m_imageView->GetImageView() );
		m_descriptorImageSampler->UpdateRange(0,0);

// 		std::vector<VkImageView> views = { m_imageView->GetImageView() };
// 		m_descriptorImageSampler->
// 		m_descriptorImageSampler->SetImageSamplerBinding( VK_SHADER_STAGE_FRAGMENT_BIT, views, m_sampler->GetSampler(), 0 );
// 		m_descriptorImageSampler->Update();
	}

	//================================================================================================================================
	//================================================================================================================================
	void PostprocessPipeline::Bind( VkCommandBuffer _commandBuffer, const size_t _index ) {
		Pipeline::Bind(_commandBuffer, _index );
		std::vector<VkDescriptorSet> descriptors = {
			m_descriptorImageSampler->GetSet(0)
			, m_descriptorUniforms->GetSet( _index )
		};
		vkCmdBindDescriptorSets(
			_commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_pipelineLayout,
			0,
			static_cast<uint32_t>( descriptors.size() ),
			descriptors.data(),
			0,
			nullptr
		);
	}

	//================================================================================================================================
	//================================================================================================================================
	void PostprocessPipeline::UpdateUniformBuffers( const size_t _index ) {
		m_descriptorUniforms->SetBinding( 0, _index, &uniforms, sizeof( Uniforms ), 0);
	}

	//================================================================================================================================
	//================================================================================================================================
	void PostprocessPipeline::CreateDescriptors( const size_t _numSwapchainImages ) {
		delete m_descriptorUniforms;
		m_descriptorUniforms = new Descriptor( m_device, _numSwapchainImages );
		m_descriptorUniforms->SetUniformBinding( VK_SHADER_STAGE_FRAGMENT_BIT, sizeof( Uniforms ) );		
		m_descriptorUniforms->Create();
		UpdateUniformBuffers();

		delete m_descriptorImageSampler;
		m_descriptorImageSampler = new DescriptorTextures( m_device, 1, m_sampler->GetSampler() );
		m_descriptorImageSampler->Append( m_imageView->GetImageView() );
		m_descriptorImageSampler->UpdateRange(0,0);
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
		m_descriptorSetLayouts = { m_descriptorImageSampler->GetLayout(), m_descriptorUniforms->GetLayout()	};
	}

	//================================================================================================================================
	//================================================================================================================================
	void PostprocessPipeline::SetImageAndView( ImageView* _imageView, Sampler* _sampler ) { 
		m_sampler = _sampler;
		m_imageView = _imageView;
	}
}