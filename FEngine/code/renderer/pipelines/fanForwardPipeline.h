#pragma once

#include "renderer/fanUniforms.h"
#include "core/memory/fanAlignedMemory.h"

#include "renderer/core/fanPipeline.h"

namespace fan
{
	class Mesh;
	struct Vertex;
	class Device;
	class Shader;
	class Image;
	class ImageView;
	class Buffer;
	class Texture;
	class Sampler;
	class Descriptor;

	//================================================================================================================================
	//================================================================================================================================
	class ForwardPipeline : public Pipeline {
	public:
		ForwardPipeline( Device& _device, const VkExtent2D _extent );
		~ForwardPipeline() override;

		void Resize( const VkExtent2D _extent ) override;
		void Bind( VkCommandBuffer _commandBuffer ) override;
		void BindDescriptors( VkCommandBuffer _commandBuffer, const uint32_t _indexOffset );
		void UpdateUniformBuffers();

		VkImageView		GetDepthImageView();

		VertUniforms  vertUniforms;
		AlignedMemory<DynamicUniformsVert> dynamicUniformsVert;
		FragUniforms  fragUniforms;		
		AlignedMemory<DynamicUniformsMaterial> dynamicUniformsMaterial;
		LightsUniforms  lightUniforms;

	protected:
		void ConfigurePipeline() override;

	private:
		Descriptor * m_sceneDescriptor;
		Descriptor * m_texturesDescriptor;

		Sampler *	m_sampler;
		Image *		m_depthImage;
		ImageView *	m_depthImageView;
		
		bool CreateSceneDescriptor();
		bool CreateTextureDescriptor();

		bool CreateDepthRessources(VkExtent2D _extent);
		void DeleteDepthRessources();

	};
}