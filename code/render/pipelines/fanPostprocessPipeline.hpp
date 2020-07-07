#pragma once

#include "render/core/fanPipeline.hpp"
#include "fanDisableWarnings.hpp"
WARNINGS_GLM_PUSH()
#include "glm/glm.hpp"
WARNINGS_POP()
#include "render/core/fanSampler.hpp"

namespace fan
{
	struct ImageView;
	class Buffer;
	class DescriptorTextures;
	class Descriptor;

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

		void SetGameImageView( ImageView* _imageView ) { m_imageView = _imageView; }
		void CreateDescriptors( const size_t _numSwapchainImages );
		void Resize( const VkExtent2D _extent ) override;
		void Bind( VkCommandBuffer _commandBuffer, const size_t _index ) override;
		void UpdateUniformBuffers( const size_t _index = 0 ) override;

	protected:
		void ConfigurePipeline() override;

	private:

		DescriptorTextures* m_descriptorImageSampler = nullptr;
		Descriptor* m_descriptorUniforms = nullptr;
		ImageView* m_imageView;
		Sampler m_sampler;
	};
}