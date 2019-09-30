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
		void CreateDescriptors();
		void Resize( const VkExtent2D _extent ) override;
		void Bind( VkCommandBuffer _commandBuffer ) override;
		void ReloadShaders() override;
		void UpdateUniformBuffers() override;

	protected:
		void ConfigurePipeline() override;

	private:		

		Descriptor *	m_descriptor = nullptr;
		ImageView *		m_imageView;
		Sampler *		m_sampler;
	};
}