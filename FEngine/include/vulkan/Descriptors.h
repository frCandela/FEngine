#pragma once

#include "Sampler.h"
#include "Texture.h"

#include "Buffer.hpp"
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


		vks::Buffer view;
		vks::Buffer dynamic;

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


	};
}