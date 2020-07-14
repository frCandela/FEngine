#include "render/pipelines/fanPostprocessPipeline.hpp"

#include "core/fanDebug.hpp"
#include "render/core/fanDevice.hpp"
#include "render/core/fanShader.hpp"
#include "render/core/fanBuffer.hpp"
#include "render/core/fanImageView.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	PostprocessPipeline::PostprocessPipeline( Device& _device )
	{
		uniforms.color = glm::vec4( 1, 1, 1, 1 );

		m_sampler.Create( _device, 0, 0.f, VK_FILTER_NEAREST );
	}

	//================================================================================================================================
	//================================================================================================================================
	void PostprocessPipeline::Destroy( Device& _device )
	{
		Pipeline::Destroy( _device );
		m_descriptorImageSampler.Destroy( _device );
		m_descriptorUniforms.Destroy( _device );
		m_sampler.Destroy( _device );
	}

	//================================================================================================================================
	//================================================================================================================================
	void PostprocessPipeline::Bind( VkCommandBuffer _commandBuffer, VkExtent2D _extent, const size_t _index )
	{
		Pipeline::Bind( _commandBuffer, _extent, _index );
		std::vector<VkDescriptorSet> descriptors = {
			m_descriptorImageSampler.mDescriptorSets[ 0 ]
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
	void PostprocessPipeline::SetUniformsData( Device& _device, const size_t _index )
	{
		m_descriptorUniforms.SetData( _device, 0, _index, &uniforms, sizeof( Uniforms ), 0 );
	}

	//================================================================================================================================
	//================================================================================================================================
	void PostprocessPipeline::CreateDescriptors( Device& _device, const uint32_t _numSwapchainImages )
	{
		Debug::Log() << "Postprocess pipeline : create descriptors" << Debug::Endl();
		m_descriptorUniforms.Destroy( _device );
		m_descriptorUniforms.AddUniformBinding( _device, _numSwapchainImages, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof( Uniforms ) );
		m_descriptorUniforms.Create( _device, _numSwapchainImages );
		SetUniformsData( _device );

		m_descriptorImageSampler.Destroy( _device );
		m_descriptorImageSampler.Create( _device, &m_imageView->mImageView, 1, m_sampler.mSampler );
	}

	//================================================================================================================================
	//================================================================================================================================
	PipelineConfig PostprocessPipeline::GetConfig()
	{
		PipelineConfig config( m_vertexShader, m_fragmentShader );

		config.bindingDescription.resize( 1 );
		config.bindingDescription[ 0 ].binding = 0;
		config.bindingDescription[ 0 ].stride = sizeof( glm::vec3 );
		config.bindingDescription[ 0 ].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		config.attributeDescriptions.resize( 1 );
		config.attributeDescriptions[0].binding = 0;
		config.attributeDescriptions[0].location = 0;
		config.attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		config.attributeDescriptions[0].offset = 0;
		config.inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		config.rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
		config.attachmentBlendStates[0].srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		config.attachmentBlendStates[0].dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		config.descriptorSetLayouts = { m_descriptorImageSampler.mDescriptorSetLayout, m_descriptorUniforms.mDescriptorSetLayout };
	
		return config;
	}
}