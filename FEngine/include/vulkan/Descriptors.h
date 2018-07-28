#pragma once

#include "Sampler.h"
#include "Texture.h"

#include "Buffer.hpp"
#include "Camera.h"

namespace vk
{
	class Descriptors
	{
	public:
		Descriptors(Device& device);
		~Descriptors();

		size_t dynamicAlignment;
		static const int OBJECT_INSTANCES = 2;

		VkDescriptorSet descriptorSet;
		VkDescriptorSetLayout descriptorSetLayout;

		// Creates the descriptor set
		void CreateDescriptorSet(Texture& textureImage, Sampler& textureSampler, VkDescriptorPool descriptorPool);

		// Creates descriptor set layouts
		void CreateDescriptorSetLayout();

		// Create the uniforms buffers
		void CreateUniformBuffer();

		// Update the view and projection matrices uniform
		void UpdateUniformBuffers( Camera& camera );

		// Update the dynamic model matrices uniform
		void UpdateDynamicUniformBuffer( std::vector<glm::mat4> matrices );

	private:
		Device & m_device;

		vk::Buffer view;	// Proj view
		vk::Buffer dynamic;	// Model

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

	};
}