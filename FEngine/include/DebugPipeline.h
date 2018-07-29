#pragma once

#include "vulkan/Device.h"
#include "vulkan/Buffer.hpp"
#include "vulkan/Shader.h"
#include "vulkan/Vertex.h"
#include "vulkan/Mesh.h"

#include "imgui/imgui.h"

// Render pipeline and depencencies for drawing 3D debug information (lines, points, sphere, etc.)
class DebugPipeline
{
public:
	// Constructor : Initialize depencencies
	DebugPipeline(vk::Device& rDevice);
	~DebugPipeline();

	// Creates the graphics pileline and layout
	void CreateGraphicsPipeline(VkRenderPass renderPass, VkExtent2D extent2D);

	// Binds the pipeline
	void Bind(VkCommandBuffer commandBuffer);

	// Updates the projection/view matrices uniform
	void UpdateUniforms(glm::mat4 projection, glm::mat4 view);

	// Render Imgui collapsable header for the DebugPipeline parameters
	void RenderGui();

private:

	// Creates the descriptor pool
	void CreateDescriptorPool();

	// Creates the descriptors
	void CreateDescriptors();

	// Reference to the device
	vk::Device& m_rDevice;

	// Dependencies
	VkPipeline m_graphicsPipeline = VK_NULL_HANDLE;
	VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
	VkDescriptorSet m_descriptorSet = VK_NULL_HANDLE;
	VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;
	VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;

	vk::Buffer* m_projViewBuffer;
	vk::Shader* m_vertShaderDebug;
	vk::Shader* m_fragShaderDebug;

	struct { glm::mat4 projection; glm::mat4 view; } projView;

	// Dynamic states
	float m_lineWidth = 1.f;


public:
	struct Vertex
	{
		glm::vec3 pos;
		glm::vec3 color;

		// Tells Vulkan how to pass this data format to the vertex shader
		static VkVertexInputBindingDescription GetBindingDescription()
		{
			VkVertexInputBindingDescription bindingDescription = {};
			bindingDescription.binding = 0;								// Index of the binding in the array of bindings
			bindingDescription.stride = sizeof(DebugPipeline::Vertex);	// Number of bytes from one entry to the next
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			return bindingDescription;
		}

		// Describes how to handle vertex input
		static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions()
		{
			std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};

			// Position
			attributeDescriptions[0].binding = 0;							// Tells Vulkan from which binding the per-vertex data comes
			attributeDescriptions[0].location = 0;							// References the location directive of the input in the vertex shader
			attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;	// Describes the type of data for the attribute
			attributeDescriptions[0].offset = offsetof(DebugPipeline::Vertex, pos);

			// Color
			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(DebugPipeline::Vertex, color);

			return attributeDescriptions;
		}
	};

};

