#pragma once

#include "Sampler.h"
#include "Buffer.h"
#include "Texture.h"

namespace vk
{
	class Descriptors
	{
	public:
		Descriptors(Device& device);
		~Descriptors();

		struct UniformBufferObject
		{
			glm::mat4 model;
			glm::mat4 view;
			glm::mat4 proj;
		};

		Device& m_device;

		VkDescriptorPool descriptorPool;
		VkDescriptorSet descriptorSet;
		VkDescriptorSetLayout descriptorSetLayout;

		VkBuffer uniformBuffer;
		VkDeviceMemory uniformBufferMemory;


		void createDescriptorPool();
		void createDescriptorSet(Texture& textureImage, Sampler& textureSampler);
		void createDescriptorSetLayout();
		void updateUniformBuffer(float aspectRatio);

		void createUniformBuffer();
	};
}