#include "vulkan/Descriptors.h"

namespace vk
{

	Descriptors::Descriptors(Device& device) :
		m_device(device)
	{
		createDescriptorSetLayout();
		createUniformBuffer();
		createDescriptorPool();
	}

	Descriptors::~Descriptors()
	{
		vkDestroyDescriptorPool(m_device.device, descriptorPool, nullptr);

		// Destroy uniform buffer
		vkDestroyDescriptorSetLayout(m_device.device, descriptorSetLayout, nullptr);
		vkDestroyBuffer(m_device.device, uniformBuffer, nullptr);
		vkFreeMemory(m_device.device, uniformBufferMemory, nullptr);
	}

	// Create descriptor pool (for uniform buffers)
	void Descriptors::createDescriptorPool()
	{
		//Create descriptor pool for vertex and texture uniforms
		std::array<VkDescriptorPoolSize, 3> poolSizes = {};
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount = 1;
		poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[1].descriptorCount = 1;
		poolSizes[2].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[2].descriptorCount = 1;

		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = 1;

		if (vkCreateDescriptorPool(m_device.device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
			throw std::runtime_error("failed to create descriptor pool!");
	}
	// Create a desctiptor set from a descriptor pool
	void Descriptors::createDescriptorSet(Texture& textureImage, Sampler& textureSampler)
	{
		// Allocate the descriptor set
		VkDescriptorSetLayout layouts[] = { descriptorSetLayout };
		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = layouts;

		if (vkAllocateDescriptorSets(m_device.device, &allocInfo, &descriptorSet) != VK_SUCCESS)
			throw std::runtime_error("failed to allocate descriptor set!");

		// Create uniforms descriptor
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = uniformBuffer;
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		// Create image descriptor
		VkDescriptorImageInfo imageInfo = {};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = textureImage.imageView;
		imageInfo.sampler = textureSampler.sampler;

		// Create uniforms descriptor
		VkDescriptorBufferInfo bufferInfo2 = {};
		bufferInfo2.buffer = uniformBufferDynamic;
		bufferInfo2.offset = 0;
		bufferInfo2.range = sizeof(UniformBufferObject);

		// Regroup descriptors
		std::array<VkWriteDescriptorSet, 3> descriptorWrites = {};
		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = descriptorSet;
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = descriptorSet;
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pImageInfo = &imageInfo;

		descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[2].dstSet = descriptorSet;
		descriptorWrites[2].dstBinding = 2;
		descriptorWrites[2].dstArrayElement = 0;
		descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[2].descriptorCount = 1;
		descriptorWrites[2].pBufferInfo = &bufferInfo2;

		vkUpdateDescriptorSets(m_device.device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

	}

	// Creates descriptor set layouts (like the uniform buffers)
	void Descriptors::createDescriptorSetLayout()
	{
		// Create a descriptor binding for vertex uniform entries in vertex shaders
		VkDescriptorSetLayoutBinding uboLayoutBinding = {};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.pImmutableSamplers = nullptr;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		// Create a descriptor binding for texture uniform entries in fragment shaders
		VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
		samplerLayoutBinding.binding = 1;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.pImmutableSamplers = nullptr;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		// Create a descriptor binding for vertex uniform entries in vertex shaders
		VkDescriptorSetLayoutBinding uboLayoutBindingDynamic = {};
		uboLayoutBindingDynamic.binding = 2;
		uboLayoutBindingDynamic.descriptorCount = 1;
		uboLayoutBindingDynamic.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBindingDynamic.pImmutableSamplers = nullptr;
		uboLayoutBindingDynamic.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		// Regroup all bindings
		std::array<VkDescriptorSetLayoutBinding, 3> bindings = { uboLayoutBinding, samplerLayoutBinding, uboLayoutBindingDynamic };
		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		if (vkCreateDescriptorSetLayout(m_device.device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
			throw std::runtime_error("failed to create descriptor set layout!");

	}
	// Create the uniforms buffer
	void Descriptors::createUniformBuffer()
	{
		VkDeviceSize bufferSize1 = sizeof(UniformBufferObject);
		Buffer::createBuffer(m_device, bufferSize1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffer, uniformBufferMemory);
	
		VkDeviceSize bufferSize2 = sizeof(UniformBufferObject);
		Buffer::createBuffer(m_device, bufferSize2, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBufferDynamic, uniformBufferMemoryDynamic);


		// Calculate required alignment based on minimum device offset alignment
		/*size_t minUboAlignment = m_device.properties.limits.minUniformBufferOffsetAlignment;
		dynamicAlignment = sizeof(glm::mat4);
		if (minUboAlignment > 0) 
			dynamicAlignment = (dynamicAlignment + minUboAlignment - 1) & ~(minUboAlignment - 1);
		size_t bufferSize = OBJECT_INSTANCES * dynamicAlignment;

		uboDataDynamic.model = (glm::mat4*)_aligned_malloc(bufferSize, dynamicAlignment);
		Buffer::createBuffer(m_device, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT , uniformBufferDynamic, uniformBufferMemoryDynamic);*/

	}
	
	// Do stuff
	void  Descriptors::updateUniformBuffer(UniformBufferObject bufferObject)
	{
		//copy the data in the uniform buffer object to the uniform buffer
		void* data;
		vkMapMemory(m_device.device, uniformBufferMemory, 0, sizeof(bufferObject), 0, &data);
		memcpy(data, &bufferObject, sizeof(bufferObject));
		vkUnmapMemory(m_device.device, uniformBufferMemory);
	}
}