#pragma once

#include "Sampler.h"
#include "Buffer.h"
#include "Texture.h"

#include <stdlib.h>

namespace vk
{
#define OBJECT_INSTANCES 125

	struct UniformBufferObject
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};

	struct UboDataDynamic 
	{
		glm::mat4 *model = nullptr;
	};

	class Descriptors
	{
	public:
		Descriptors(Device& device);
		~Descriptors();

		Device& m_device;

		VkDescriptorPool descriptorPool;
		VkDescriptorSet descriptorSet;
		VkDescriptorSetLayout descriptorSetLayout;

		VkBuffer uniformBuffer;
		VkDeviceMemory uniformBufferMemory;

		VkBuffer uniformBufferDynamic;
		VkDeviceMemory uniformBufferMemoryDynamic;

		size_t dynamicAlignment;
		UboDataDynamic uboDataDynamic;

		void createDescriptorPool();
		void createDescriptorSet(Texture& textureImage, Sampler& textureSampler);
		void createDescriptorSetLayout();
		void updateUniformBuffer(UniformBufferObject bufferObject);

		void  UpdateModelBuffer(glm::mat4 mat);

		void createUniformBuffer();
	};
}