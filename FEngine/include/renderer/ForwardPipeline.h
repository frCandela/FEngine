#pragma once

#include "vulkan/Sampler.h"
#include "vulkan/Texture.h"
#include "vulkan/Buffer.hpp"
#include "vulkan/Shader.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"
#include "glm/glm.hpp"

#include <array>

typedef int * render_id;

class ForwardPipeline
{
public:
	ForwardPipeline(vk::Device& device, vk::Texture& texture, vk::Sampler& sampler);
	~ForwardPipeline();

	// Binds the pipeline
	void BindPipeline(VkCommandBuffer commandBuffer);

	// Binds the pipeline descriptors
	void BindDescriptors(VkCommandBuffer commandBuffer, int offsetIndex);

	// Update the view and projection matrices uniform
	void UpdateUniforms(glm::mat4 projectionMat, glm::mat4 viewMat);

	// Update the dynamic model matrices uniform
	void UpdateDynamicUniformBuffer(std::vector<glm::mat4> matrices);

	// Creates the graphics pipeline
	void CreateGraphicsPipeline(VkRenderPass renderPass, VkExtent2D extent2D);

private:
	// Creates descriptor set layouts
	void CreateDescriptorSetLayout();

	// Create the uniforms buffers
	void CreateUniformBuffer();

	// Create the descriptor pool
	void CreateDescriptorPool();

	// Creates the descriptor set
	void CreateDescriptorSet(vk::Texture& textureImage, vk::Sampler& textureSampler, VkDescriptorPool descriptorPool);

	// References
	vk::Device & m_device;

	vk::Shader* vertShader;
	vk::Shader* fragShader;
	vk::Buffer view;	// Proj view
	vk::Buffer dynamic;	// Model

	VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
	VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
	VkPipeline graphicsPipeline1 = VK_NULL_HANDLE;
	VkPipelineLayout pipelineLayout1 = VK_NULL_HANDLE;
	VkDescriptorSet descriptorSet = VK_NULL_HANDLE;

	size_t dynamicAlignment;
	static const int OBJECT_INSTANCES = 512;

	struct
	{
		glm::mat4 projection;
		glm::mat4 view;
	} uboVS;

	// One big uniform buffer that contains all matrices(we need to manually allocate the data to cope for GPU-specific uniform buffer offset alignments)
	struct UboDataDynamic
	{
		glm::mat4 *model = nullptr;
	} uboDataDynamic;

public:
	struct Vertex
	{
		glm::vec3 pos;
		glm::vec3 color;
		glm::vec2 texCoord;

		bool operator==(const Vertex& other) const
		{
			return pos == other.pos && color == other.color && texCoord == other.texCoord;
		}

		// Tells Vulkan how to pass this data format to the vertex shader
		static VkVertexInputBindingDescription getBindingDescription()
		{
			VkVertexInputBindingDescription bindingDescription = {};

			bindingDescription.binding = 0;								// Index of the binding in the array of bindings
			bindingDescription.stride = sizeof(Vertex);					// Number of bytes from one entry to the next
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		// Describes how to handle vertex input
		static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions()
		{
			std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = {};

			// Position
			attributeDescriptions[0].binding = 0;							// Tells Vulkan from which binding the per-vertex data comes
			attributeDescriptions[0].location = 0;							// References the location directive of the input in the vertex shader
			attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;	// Describes the type of data for the attribute
			attributeDescriptions[0].offset = offsetof(Vertex, pos);

			// Color
			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, color);

			// Texture coordinates
			attributeDescriptions[2].binding = 0;
			attributeDescriptions[2].location = 2;
			attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

			return attributeDescriptions;
		}
	};
};

namespace std
{
	template<> struct hash<ForwardPipeline::Vertex> {
		size_t operator()(ForwardPipeline::Vertex const& vertex) const {
			return ((hash<glm::vec3>()(vertex.pos) ^
				(hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
				(hash<glm::vec2>()(vertex.texCoord) << 1);
		}
	};
}
