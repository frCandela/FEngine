#pragma once

#include "fanDisableWarnings.hpp"
WARNINGS_GLM_PUSH()
#include "glm/glm.hpp"
WARNINGS_POP()
#include "render/pipelines/fanPipeline.hpp"
#include "render/core/fanSampler.hpp"
#include "render/core/descriptors/fanDescriptorUniforms.hpp"
#include "render/core/descriptors/fanDescriptorImages.hpp"

namespace fan
{
	struct ImageView;

	//================================================================================================================================
	// postprocess pipeline
	//================================================================================================================================
	class PostprocessPipeline : public Pipeline
	{
	public:
		//================================================================
		//================================================================
		struct Uniforms
		{
			glm::vec4 color;
		} uniforms;

		PostprocessPipeline( Device& _device );
		void Plop( Device& _device );

		void SetGameImageView( ImageView& _imageView ) { m_imageView = &_imageView; }
		void CreateDescriptors( Device& _device, const uint32_t _numSwapchainImages );
		void Bind( VkCommandBuffer _commandBuffer, VkExtent2D _extent, const size_t _index ) override;
		void SetUniformsData( Device& _device, const size_t _index = 0 ) override;

		PipelineConfig GetConfig( Shader& _vert, Shader& _frag );



		DescriptorImages m_descriptorImageSampler;
		DescriptorUniforms m_descriptorUniforms;
		ImageView* m_imageView;
		Sampler m_sampler;
	};
}