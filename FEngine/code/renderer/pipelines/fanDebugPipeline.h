#pragma once

#include "renderer/core/fanPipeline.h"

namespace fan
{
	class Device;
	class Shader;
	class Image;
	class ImageView;
	class Buffer;
	class Descriptor;
	struct DebugUniforms;

	//================================================================================================================================
	//================================================================================================================================
	class DebugPipeline : public Pipeline {
	public:
		DebugPipeline(Device& _device, const VkPrimitiveTopology _primitiveTopology, const bool _depthTestEnable);
		virtual ~DebugPipeline();

		void Bind( VkCommandBuffer _commandBuffer ) override;
		void SetUniformPointers( DebugUniforms * _debugUniforms	);
		void UpdateUniformBuffers();

	protected:
		void ConfigurePipeline() override;

	private:
		DebugUniforms  *	m_debugUniforms;
		Descriptor *		m_descriptor;
		VkPrimitiveTopology m_primitiveTopology;
		bool				m_depthTestEnable;
	};
}
