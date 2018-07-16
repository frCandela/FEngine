#pragma once

#include "Sampler.h"
#include "Buffer.h"
#include "Texture.h"

namespace vk
{
	struct UniformBufferObject
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
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


		void createDescriptorPool();
		void createDescriptorSet(Texture& textureImage, Sampler& textureSampler);
		void createDescriptorSetLayout();
		void updateUniformBuffer(UniformBufferObject bufferObject);

		void createUniformBuffer();
	};
}