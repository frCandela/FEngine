#pragma once

#include "fanDisableWarnings.hpp"
WARNINGS_GLM_PUSH()
#include "glm/glm.hpp"
WARNINGS_POP()
#include "render/core/fanPipeline.hpp"
#include "render/core/fanSampler.hpp"
#include "render/descriptors/fanDescriptorUniforms.hpp"

namespace fan
{
	struct ImageView;
	class DescriptorTextures;

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
		~PostprocessPipeline() override;

		void SetGameImageView( ImageView& _imageView ) { m_imageView = &_imageView; }
		void CreateDescriptors( const uint32_t _numSwapchainImages );
		void Resize( const VkExtent2D _extent ) override;
		void Bind( VkCommandBuffer _commandBuffer, const size_t _index ) override;
		void SetUniformsData( const size_t _index = 0 ) override;

	protected:
		void ConfigurePipeline() override;

	private:

		DescriptorTextures* m_descriptorImageSampler = nullptr;
		DescriptorUniforms m_descriptorUniforms;
		ImageView* m_imageView;
		Sampler m_sampler;
	};
}