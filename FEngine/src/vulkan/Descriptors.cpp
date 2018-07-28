#include "vulkan/Descriptors.h"
#include "vulkan/Initializers.h"

namespace vk
{

	Descriptors::Descriptors(Device& device) :
		m_device(device)
		, view(device)
		, dynamic(device)		
	{
		CreateDescriptorSetLayout();
		CreateUniformBuffer();
	}

	Descriptors::~Descriptors()
	{
		vkDestroyDescriptorSetLayout(m_device.device, descriptorSetLayout, nullptr);
	}

	void Descriptors::CreateDescriptorSetLayout()
	{
		std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings =
		{
			vk::init::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 0),
			vk::init::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_SHADER_STAGE_VERTEX_BIT, 1),
			vk::init::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 2)
		};

		VkDescriptorSetLayoutCreateInfo descriptorLayout =
			vk::init::descriptorSetLayoutCreateInfo(
				setLayoutBindings.data(),
				static_cast<uint32_t>(setLayoutBindings.size()));

		VK_CHECK_RESULT(vkCreateDescriptorSetLayout(m_device.device, &descriptorLayout, nullptr, &descriptorSetLayout));
	}

	void Descriptors::CreateUniformBuffer()
	{
		// Calculate required alignment based on minimum device offset alignment
		size_t minUboAlignment = m_device.properties.limits.minUniformBufferOffsetAlignment;
		dynamicAlignment = sizeof(glm::mat4);
		if (minUboAlignment > 0) {
			dynamicAlignment = (dynamicAlignment + minUboAlignment - 1) & ~(minUboAlignment - 1);
		}

		size_t bufferSize = OBJECT_INSTANCES * dynamicAlignment;

		uboDataDynamic.model = (glm::mat4*)_aligned_malloc(bufferSize, dynamicAlignment);
		assert(uboDataDynamic.model);

		//std::cout << "minUniformBufferOffsetAlignment = " << minUboAlignment << std::endl;
		//std::cout << "dynamicAlignment = " << dynamicAlignment << std::endl;

		// Static shared uniform buffer object with projection and view matrix
		view.CreateBuffer(
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			sizeof(uboVS)
		);

		// Uniform buffer object with per-object matrices
		dynamic.CreateBuffer(
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
			bufferSize
		);

		// Map persistent
		VK_CHECK_RESULT(view.Map());
		VK_CHECK_RESULT(dynamic.Map());
	}


	
	void Descriptors::CreateDescriptorSet(Texture& textureImage, Sampler& textureSampler, VkDescriptorPool descriptorPool)
	{
		VkDescriptorSetAllocateInfo allocInfo =	vk::init::descriptorSetAllocateInfo(descriptorPool,	&descriptorSetLayout, 1);

		if (vkAllocateDescriptorSets(m_device.device, &allocInfo, &descriptorSet) != VK_SUCCESS)
			throw std::runtime_error("failed to allocate descriptor set!");
		
		// Create image descriptor
		VkDescriptorImageInfo imageInfo = {};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = textureImage.imageView;
		imageInfo.sampler = textureSampler.sampler;

		VkWriteDescriptorSet descriptorWrite = {};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = descriptorSet;
		descriptorWrite.dstBinding = 2;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pImageInfo = &imageInfo;

		std::vector<VkWriteDescriptorSet> writeDescriptorSets = 
		{
			// Binding 0 : Projection/View matrix uniform buffer			
			vk::init::writeDescriptorSet(descriptorSet, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, &view.descriptor)

			// Binding 1 : Instance matrix as dynamic uniform buffer
			,vk::init::writeDescriptorSet(descriptorSet, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1, &dynamic.descriptor)

			// Binding 1 : Image sampler and texture
			,descriptorWrite

		};

		vkUpdateDescriptorSets(m_device.device, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, NULL);
	}

	void Descriptors::UpdateUniformBuffers(Camera& camera)
	{
		// Fixed ubo with projection and view matrices
		uboVS.projection = camera.GetProj();
		uboVS.view = camera.GetView();

		memcpy(view.mappedData, &uboVS, sizeof(uboVS));
	}

	void Descriptors::UpdateDynamicUniformBuffer(std::vector<glm::mat4> matrices)
	{
		for (int i = 0; i < OBJECT_INSTANCES; ++i)
		{
			glm::mat4* modelMat = (glm::mat4*)(((uint64_t)uboDataDynamic.model + (i * dynamicAlignment)));
			*modelMat = matrices[i];
		}

		memcpy(dynamic.mappedData, uboDataDynamic.model, dynamic.m_size);

		// Flush to make changes visible to the host 
		VkMappedMemoryRange memoryRange = vk::init::mappedMemoryRange();
		memoryRange.memory = dynamic.memory;
		memoryRange.size = dynamic.m_size;
		vkFlushMappedMemoryRanges(m_device.device, 1, &memoryRange);
	}
}