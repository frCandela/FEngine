#include "render/pipelines/fanPostprocessPipeline.hpp"

#include "core/fanDebug.hpp"
#include "render/core/fanDevice.hpp"
#include "render/core/fanShader.hpp"
#include "render/core/fanBuffer.hpp"
#include "render/descriptors/fanDescriptorTexture.hpp"
#include "render/descriptors/fanDescriptorUniforms.hpp"
#include "render/core/fanImageView.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	PostprocessPipeline::PostprocessPipeline( Device& _device ) :
		Pipeline( _device )
	{
		uniforms.color = glm::vec4( 1, 1, 1, 1 );

		m_sampler.Create( _device, 0, 0.f, VK_FILTER_NEAREST );
	}

	//================================================================================================================================
	//================================================================================================================================
	PostprocessPipeline::~PostprocessPipeline()
	{
		delete m_descriptorImageSampler;
		m_descriptorUniforms.Destroy( m_device );
		m_sampler.Destroy( m_device );
	}

	//================================================================================================================================
	//================================================================================================================================
	void PostprocessPipeline::Resize( const VkExtent2D _extent )
	{
		Pipeline::Resize( _extent );
		m_descriptorImageSampler->Set( 0, m_imageView->mImageView );
		m_descriptorImageSampler->UpdateRange( 0, 0 );
	}

	//================================================================================================================================
	//================================================================================================================================
	void PostprocessPipeline::Bind( VkCommandBuffer _commandBuffer, const size_t _index )
	{
		Pipeline::Bind( _commandBuffer, _index );
		std::vector<VkDescriptorSet> descriptors = {
			m_descriptorImageSampler->GetSet( 0 )
			, m_descriptorUniforms.mDescriptorSets[ _index ]
		};
		vkCmdBindDescriptorSets(
			_commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_pipelineLayout,
			0,
			static_cast< uint32_t >( descriptors.size() ),
			descriptors.data(),
			0,
			nullptr
		);
	}

	//================================================================================================================================
	//================================================================================================================================
	void PostprocessPipeline::SetUniformsData( const size_t _index )
	{
		m_descriptorUniforms.SetData( m_device, 0, _index, &uniforms, sizeof( Uniforms ), 0 );
	}

	//================================================================================================================================
	//================================================================================================================================
	void PostprocessPipeline::CreateDescriptors( const uint32_t _numSwapchainImages )
	{
		Debug::Log() << "Postprocess pipeline : create descriptors" << Debug::Endl();
		m_descriptorUniforms.Destroy( m_device );
		m_descriptorUniforms.AddUniformBinding( m_device, _numSwapchainImages, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof( Uniforms ) );
		m_descriptorUniforms.Create( m_device, _numSwapchainImages );
		SetUniformsData();

		delete m_descriptorImageSampler;
		m_descriptorImageSampler = new DescriptorTextures( m_device, 1, m_sampler.mSampler );
		m_descriptorImageSampler->Append( m_imageView->mImageView );
		m_descriptorImageSampler->UpdateRange( 0, 0 );
	}

	//================================================================================================================================
	//================================================================================================================================
	void PostprocessPipeline::ConfigurePipeline()
	{
		m_bindingDescription.resize( 1 );
		m_bindingDescription[ 0 ].binding = 0;
		m_bindingDescription[ 0 ].stride = sizeof( glm::vec3 );
		m_bindingDescription[ 0 ].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		m_attributeDescriptions.resize( 1 );
		m_attributeDescriptions[ 0 ].binding = 0;
		m_attributeDescriptions[ 0 ].location = 0;
		m_attributeDescriptions[ 0 ].format = VK_FORMAT_R32G32B32_SFLOAT;
		m_attributeDescriptions[ 0 ].offset = 0;
		m_inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		m_rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
		m_attachmentBlendStates[ 0 ].srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		m_attachmentBlendStates[ 0 ].dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		m_descriptorSetLayouts = { m_descriptorImageSampler->GetLayout(), m_descriptorUniforms.mDescriptorSetLayout };
	}
}