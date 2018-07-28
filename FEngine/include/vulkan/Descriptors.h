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
		VkDescriptorSet descriptorSet;
		VkDescriptorSetLayout descriptorSetLayout;
		static const int OBJECT_INSTANCES = 2;

		/// Creates descriptor set layouts
		void CreateDescriptorSetLayout();

		/// Create the uniforms bufferS
		void CreateUniformBuffer();



		/// Creates the descriptor set
		void CreateDescriptorSet(Texture& textureImage, Sampler& textureSampler, VkDescriptorPool descriptorPool);

		/// Update the view and projection matrices uniform
		void UpdateUniformBuffers( Camera& camera );

		/// Update the dynamic model matrices uniform
		void UpdateDynamicUniformBuffer( std::vector<glm::mat4> matrices );


	private:
		Device & m_device;
		vk::Buffer view;
		vk::Buffer dynamic;


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