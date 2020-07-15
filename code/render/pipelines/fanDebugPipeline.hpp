#pragma once

#include "fanDisableWarnings.hpp"
WARNINGS_GLM_PUSH()
#include "glm/glm.hpp"
WARNINGS_POP()
#include "render/pipelines/fanPipeline.hpp"
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
		void Plop( Device& _device );

		void Bind( VkCommandBuffer _commandBuffer, VkExtent2D _extent, const size_t _index ) override;
		void SetUniformsData( Device& _device, const size_t _index ) override;
		void CreateDescriptors( Device& _device, const uint32_t _numSwapchainImages );
		PipelineConfig GetConfig( Shader& _vert, Shader& _frag );

	private:
		DescriptorUniforms			m_descriptor;
		VkPrimitiveTopology m_primitiveTopology;
		bool				m_depthTestEnable;
	};
}
