#pragma once

#include "renderer/core/fanPipeline.h"

namespace fan
{
	class Sampler;
	class Image;
	class ImageView;
	class Buffer;
	class Descriptor;

	//================================================================================================================================
	//================================================================================================================================
	class PostprocessPipeline : public Pipeline {
	public:
		//================================================================
		//================================================================
		struct Uniforms {
			glm::vec4 color;
		} uniforms;

		PostprocessPipeline( Device& _device );
		~PostprocessPipeline() override;

		void SetImageAndView( ImageView * _imageView, Sampler * _sampler );
		void CreateDescriptors( const size_t _numSwapchainImages );
		void Resize( const VkExtent2D _extent ) override;
		void Bind( VkCommandBuffer _commandBuffer, const size_t _index ) override;
		void UpdateUniformBuffers( const size_t _index = 0 ) override;

	protected:
		void ConfigurePipeline() override;

	private:		

		Descriptor *	m_descriptorImageSampler = nullptr;
		Descriptor *	m_descriptorUniforms = nullptr;
		ImageView *		m_imageView;
		Sampler *		m_sampler;
	};
}