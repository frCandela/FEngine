#pragma once

#include "renderer/core/fanPipeline.h"

namespace fan
{
	class Image;
	class ImageView;
	class Buffer;
	class Descriptor;
	struct DebugUniforms;

	//================================================================================================================================
	//================================================================================================================================
	class DebugPipeline : public Pipeline {
	public:
		//================================================================
		//================================================================
		struct DebugUniforms {
			glm::mat4 model;
			glm::mat4 view;
			glm::mat4 proj;
			glm::vec4 color;
		} debugUniforms;

		DebugPipeline(Device& _device, const VkPrimitiveTopology _primitiveTopology, const bool _depthTestEnable);
		virtual ~DebugPipeline() override;

		void Bind( VkCommandBuffer _commandBuffer ) override;
		void UpdateUniformBuffers();

	protected:
		void ConfigurePipeline() override;

	private:
		Descriptor *		m_descriptor;
		VkPrimitiveTopology m_primitiveTopology;
		bool				m_depthTestEnable;
	};
}
