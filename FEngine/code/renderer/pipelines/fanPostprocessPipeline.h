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
		PostprocessPipeline( Device& _device, const VkFormat _format, const VkExtent2D _extent );
		~PostprocessPipeline() override;

		struct Uniforms {
			glm::vec4 color;
		};

		void Resize( const VkExtent2D _extent ) override;
		void Bind( VkCommandBuffer _commandBuffer ) override;

		Uniforms GetUniforms() const { return m_uniforms; }
		void SetUniforms(const Uniforms _uniforms);
		VkImageView		GetImageView();

	protected:
		void ConfigurePipeline() override;

	private:
		bool CreateDescriptors();
		void CreateImagesAndViews(VkExtent2D _extent);
		void CreateVertexBuffer();

		VkFormat m_format;

		Descriptor *	m_descriptor = nullptr;
		Sampler *		m_sampler;
		Image *			m_image;
		ImageView *		m_imageView;

		Buffer * m_vertexBuffer;
		Uniforms m_uniforms;
	};
}