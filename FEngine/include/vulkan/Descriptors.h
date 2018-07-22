#pragma once

#include "Sampler.h"
#include "Buffer.h"
#include "Texture.h"

#include "VulkanBuffer.hpp"
#include "Camera.h"

#include <stdlib.h>

#define VERTEX_BUFFER_BIND_ID 0
#define ENABLE_VALIDATION false
#define OBJECT_INSTANCES 2

// Vertex layout for this example
struct Vertex {
	float pos[3];
	float color[3];
};

namespace vk
{


	class Descriptors
	{
	public:

		struct {
			VkPipelineVertexInputStateCreateInfo inputState;
			std::vector<VkVertexInputBindingDescription> bindingDescriptions;
			std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
		} vertices;

		vks::Buffer vertexBuffer;
		vks::Buffer indexBuffer;
		uint32_t indexCount;

		struct {
			vks::Buffer view;
			vks::Buffer dynamic;
		} uniformBuffers;

		struct {
			glm::mat4 projection;
			glm::mat4 view;
		} uboVS;

		// Store random per-object rotations
		glm::vec3 rotations[OBJECT_INSTANCES];
		glm::vec3 rotationSpeeds[OBJECT_INSTANCES];

		// One big uniform buffer that contains all matrices
		// Note that we need to manually allocate the data to cope for GPU-specific uniform buffer offset alignments
		struct UboDataDynamic {
			glm::mat4 *model = nullptr;
		} uboDataDynamic;

		VkPipelineLayout pipelineLayout;
		VkDescriptorSet descriptorSet;
		VkDescriptorSetLayout descriptorSetLayout;

		float animationTimer = 0.0f;

		size_t dynamicAlignment;


		Descriptors(Device& device);
		~Descriptors();

		Device& m_device;

		VkDescriptorPool descriptorPool;
		//VkDescriptorSet descriptorSet;
		//VkDescriptorSetLayout descriptorSetLayout;

		/*VkBuffer uniformBuffer;
		VkDeviceMemory uniformBufferMemory;

		VkBuffer uniformBufferDynamic;
		VkDeviceMemory uniformBufferMemoryDynamic;*/

		//size_t dynamicAlignment;
		//UboDataDynamic uboDataDynamic;

		void createDescriptorPool();
		void createDescriptorSet(Texture& textureImage, Sampler& textureSampler);
		void createDescriptorSetLayout();
		//void updateUniformBuffer(UniformBufferObject bufferObject);

		void  UpdateModelBuffer(glm::mat4 mat1, glm::mat4 mat2);

		void createUniformBuffer();

		void updateUniformBuffers( Camera& camera );
		void updateDynamicUniformBuffer(bool force = false);

		VkResult createBuffer(VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, vks::Buffer *buffer, VkDeviceSize size, void *data = nullptr)
		{
			buffer->device = m_device.device;

			// Create the buffer handle
			VkBufferCreateInfo bufferCreateInfo = vks::initializers::bufferCreateInfo(usageFlags, size);
			VK_CHECK_RESULT(vkCreateBuffer(m_device.device, &bufferCreateInfo, nullptr, &buffer->buffer));

			// Create the memory backing up the buffer handle
			VkMemoryRequirements memReqs;
			VkMemoryAllocateInfo memAlloc = vks::initializers::memoryAllocateInfo();
			vkGetBufferMemoryRequirements(m_device.device, buffer->buffer, &memReqs);
			memAlloc.allocationSize = memReqs.size;
			// Find a memory type index that fits the properties of the buffer
			memAlloc.memoryTypeIndex = m_device.findMemoryType(memReqs.memoryTypeBits, memoryPropertyFlags);

			VK_CHECK_RESULT(vkAllocateMemory(m_device.device, &memAlloc, nullptr, &buffer->memory));

			buffer->alignment = memReqs.alignment;
			buffer->size = memAlloc.allocationSize;
			buffer->usageFlags = usageFlags;
			buffer->memoryPropertyFlags = memoryPropertyFlags;

			// If a pointer to the buffer data has been passed, map the buffer and copy over the data
			if (data != nullptr)
			{
				VK_CHECK_RESULT(buffer->map());
				memcpy(buffer->mapped, data, size);
				if ((memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0)
					buffer->flush();

				buffer->unmap();
			}

			// Initialize a default descriptor that covers the whole buffer size
			buffer->setupDescriptor();

			// Attach the memory to the buffer object
			return buffer->bind();
		}
	};
}

#define VK_CHECK_RESULT(f)																				\
{																										\
	VkResult res = (f);																					\
	if (res != VK_SUCCESS)																				\
	{																									\
		std::cout << "Fatal : VkResult is \"" << vks::tools::errorString(res) << "\" in " << __FILE__ << " at line " << __LINE__ << std::endl; \
		assert(res == VK_SUCCESS);																		\
	}																									\
}