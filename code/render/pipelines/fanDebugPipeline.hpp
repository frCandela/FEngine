#pragma once

#include "fanDisableWarnings.hpp"
WARNINGS_GLM_PUSH()
#include "glm/glm.hpp"
WARNINGS_POP()
#include "render/core/fanPipeline.hpp"
#include "render/core/descriptors/fanDescriptorUniforms.hpp"

namespace fan
{
	struct DebugUniforms;

	//================================================================================================================================
	// For debug lines
	//================================================================================================================================
	class DebugPipeline : public Pipeline
	{
	public:
		//================================================================
		//================================================================
		struct DebugUniforms
		{
			glm::mat4 model;
			glm::mat4 view;
			glm::mat4 proj;
			glm::vec4 color;
		} m_debugUniforms;

		DebugPipeline( Device& _device, const VkPrimitiveTopology _primitiveTopology, const bool _depthTestEnable );
		virtual ~DebugPipeline() override;

		void Bind( VkCommandBuffer _commandBuffer, const size_t _index ) override;
		void SetUniformsData( const size_t _index ) override;
		void CreateDescriptors( const uint32_t _numSwapchainImages );

	protected:
		void ConfigurePipeline() override;

	private:
		DescriptorUniforms			m_descriptor;
		VkPrimitiveTopology m_primitiveTopology;
		bool				m_depthTestEnable;
	};
}
