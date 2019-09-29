#include "fanGlobalIncludes.h"

#include "renderer/pipelines/fanPostprocessPipeline.h"
#include "renderer/core/fanDevice.h"
#include "renderer/core/fanShader.h"
#include "renderer/core/fanSampler.h"
#include "renderer/core/fanImage.h"
#include "renderer/core/fanImageView.h" 
#include "renderer/core/fanBuffer.h"
#include "renderer/core/fanDescriptor.h"
#include "renderer/fanRenderer.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	PostprocessPipeline::PostprocessPipeline(Device& _device, const VkFormat _format, const VkExtent2D _extent ) :
		Pipeline(_device)		 
	{
		uniforms.color = glm::vec4(1, 1, 1, 1);
		m_format = _format;
		//Sampler
		m_sampler = new Sampler(m_device);
		m_sampler->CreateSampler(0, 1.f);

		CreateImagesAndViews( _extent );
		CreateDescriptors();

		uniforms.color = glm::vec4( 1, 1, 1, 1 );
		UpdateUniformBuffers();
	}

	//================================================================================================================================
	//================================================================================================================================
	PostprocessPipeline::~PostprocessPipeline() {
		delete m_descriptor;
		delete m_imageView;
		delete m_image;
		delete m_sampler;
	}

	//================================================================================================================================
	//================================================================================================================================
	void PostprocessPipeline::Resize( const VkExtent2D _extent ) {
		Pipeline::Resize(_extent);
		delete m_imageView;
		delete m_image;
		CreateImagesAndViews(_extent);
		std::vector<VkImageView> views = { m_imageView->GetImageView() };
		m_descriptor->AddImageSamplerBinding( VK_SHADER_STAGE_FRAGMENT_BIT, views, m_sampler->GetSampler(), 0 );
		m_descriptor->Update();
		Debug::Get() << Debug::Severity::warning << m_image->GetSize().width << " " << m_image->GetSize().height << Debug::Endl();
	}

	//================================================================================================================================
	//================================================================================================================================
	void PostprocessPipeline::Bind( VkCommandBuffer _commandBuffer ) {
		Pipeline::Bind(_commandBuffer);
		m_descriptor->Bind( _commandBuffer, m_pipelineLayout );
	}

	//================================================================================================================================
	//================================================================================================================================
	void PostprocessPipeline::UpdateUniformBuffers() {
		m_descriptor->SetBinding(1, &uniforms, sizeof( Uniforms ) );
	}

	//================================================================================================================================
	//================================================================================================================================
	VkImageView PostprocessPipeline::GetImageView() { return m_imageView->GetImageView(); }

	//================================================================================================================================
	//================================================================================================================================
	bool PostprocessPipeline::CreateDescriptors() {
		delete m_descriptor;
		m_descriptor = new Descriptor( m_device );
		std::vector<VkImageView> views = { m_imageView->GetImageView() };
		m_descriptor->AddImageSamplerBinding( VK_SHADER_STAGE_FRAGMENT_BIT , views, m_sampler->GetSampler() );
		m_descriptor->AddUniformBinding( VK_SHADER_STAGE_FRAGMENT_BIT, sizeof( Uniforms ) );
		return m_descriptor->Create();		
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
	void PostprocessPipeline::CreateImagesAndViews(VkExtent2D _extent) {

		m_image = new Image(m_device);
		m_image->Create(
			m_format,
			_extent,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);
		m_imageView = new ImageView(m_device);
		m_imageView->Create(m_image->GetImage(), m_format, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D);
	}


}