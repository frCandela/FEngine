#include "vulkan/Descriptors.h"
#include "vulkan/Initializers.h"

namespace vk
{

	Descriptors::Descriptors(Device& device) :
		m_device(device)
		, view(device)
		, dynamic(device)		
	{
		createDescriptorSetLayout();
		createUniformBuffer();
		createDescriptorPool();
	}

	Descriptors::~Descriptors()
	{
		vkDestroyDescriptorPool(m_device.device, descriptorPool, nullptr);
		vkDestroyDescriptorSetLayout(m_device.device, descriptorSetLayout, nullptr);
		dynamic.destroy();
		view.destroy();
	}

	// Create descriptor pool (for uniform buffers)
	void Descriptors::createDescriptorPool()
	{
		// Example uses one ubo and one image sampler
		std::vector<VkDescriptorPoolSize> poolSizes =
		{
			vk::init::descriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1),
			vk::init::descriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1),
			vk::init::descriptorPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1)
		};

		VkDescriptorPoolCreateInfo descriptorPoolInfo =
			vk::init::descriptorPoolCreateInfo(
				static_cast<uint32_t>(poolSizes.size()),
				poolSizes.data(),
				2);

		if (vkCreateDescriptorPool(m_device.device, &descriptorPoolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
			throw std::runtime_error("failed to create descriptor pool!");

		//Create descriptor pool for vertex and texture uniforms
		/*std::array<VkDescriptorPoolSize, 3> poolSizes = {};
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount = 1;
		poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[1].descriptorCount = 1;
		poolSizes[2].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		poolSizes[2].descriptorCount = 1;

		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = 1;

		if (vkCreateDescriptorPool(m_device.device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
			throw std::runtime_error("failed to create descriptor pool!");*/
	}
	// Create a desctiptor set from a descriptor pool
	void Descriptors::createDescriptorSet(Texture& textureImage, Sampler& textureSampler)
	{
		VkDescriptorSetAllocateInfo allocInfo =
			vk::init::descriptorSetAllocateInfo(
				descriptorPool,
				&descriptorSetLayout,
				1);

		if (vkAllocateDescriptorSets(m_device.device, &allocInfo, &descriptorSet) != VK_SUCCESS)
			throw std::runtime_error("failed to allocate descriptor set!");

		std::vector<VkWriteDescriptorSet> writeDescriptorSets = {
			// Binding 0 : Projection/View matrix uniform buffer			
			vk::init::writeDescriptorSet(descriptorSet, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, &view.descriptor),
			// Binding 1 : Instance matrix as dynamic uniform buffer
			vk::init::writeDescriptorSet(descriptorSet, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1, &dynamic.descriptor),
		};

		vkUpdateDescriptorSets(m_device.device, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, NULL);

		// Allocate the descriptor set
		/*VkDescriptorSetLayout layouts[] = { descriptorSetLayout };
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
		bufferInfo2.range = sizeof(glm::mat4);

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
		descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		descriptorWrites[2].descriptorCount = 1;
		descriptorWrites[2].pBufferInfo = &bufferInfo2;

		vkUpdateDescriptorSets(m_device.device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		*/
	}

	// Creates descriptor set layouts (like the uniform buffers)
	void Descriptors::createDescriptorSetLayout()
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

		if(vkCreateDescriptorSetLayout(m_device.device, &descriptorLayout, nullptr, &descriptorSetLayout) != VK_SUCCESS)
			throw std::runtime_error("failed to create descriptor set layout!");

		// Create a descriptor binding for vertex uniform entries in vertex shaders
		/*VkDescriptorSetLayoutBinding uboLayoutBinding = {};
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
		uboLayoutBindingDynamic.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		uboLayoutBindingDynamic.pImmutableSamplers = nullptr;
		uboLayoutBindingDynamic.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		// Regroup all bindings
		std::array<VkDescriptorSetLayoutBinding, 3> bindings = { uboLayoutBinding, samplerLayoutBinding, uboLayoutBindingDynamic };
		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		if (vkCreateDescriptorSetLayout(m_device.device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
			throw std::runtime_error("failed to create descriptor set layout!");*/

	}
	
	// Create the uniforms buffer
	void Descriptors::createUniformBuffer()
	{
		/*VkDeviceSize bufferSize1 = sizeof(UniformBufferObject);
		Buffer::createBuffer(m_device, bufferSize1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffer, uniformBufferMemory);
	
		//VkDeviceSize bufferSize2 = sizeof(UniformBufferObject);
		//Buffer::createBuffer(m_device, bufferSize2, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBufferDynamic, uniformBufferMemoryDynamic);
		

		// Calculate required alignment based on minimum device offset alignment
		size_t minUboAlignment = m_device.properties.limits.minUniformBufferOffsetAlignment;
		dynamicAlignment = sizeof(glm::mat4);
		if (minUboAlignment > 0) 
			dynamicAlignment = (dynamicAlignment + minUboAlignment - 1) & ~(minUboAlignment - 1);
		size_t bufferSize2 = OBJECT_INSTANCES * dynamicAlignment;

		uboDataDynamic.model = (glm::mat4*)_aligned_malloc(bufferSize2, dynamicAlignment);
		Buffer::createBuffer(m_device, bufferSize2, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, uniformBufferDynamic, uniformBufferMemoryDynamic);*/

		// Allocate data for the dynamic uniform buffer object
		// We allocate this manually as the alignment of the offset differs between GPUs

		// Calculate required alignment based on minimum device offset alignment
		size_t minUboAlignment = m_device.properties.limits.minUniformBufferOffsetAlignment;
		dynamicAlignment = sizeof(glm::mat4);
		if (minUboAlignment > 0) {
			dynamicAlignment = (dynamicAlignment + minUboAlignment - 1) & ~(minUboAlignment - 1);
		}

		size_t bufferSize = OBJECT_INSTANCES * dynamicAlignment;

		uboDataDynamic.model = (glm::mat4*)_aligned_malloc(bufferSize, dynamicAlignment);
		assert(uboDataDynamic.model);

		std::cout << "minUniformBufferOffsetAlignment = " << minUboAlignment << std::endl;
		std::cout << "dynamicAlignment = " << dynamicAlignment << std::endl;

		// Vertex shader uniform buffer block

		// Static shared uniform buffer object with projection and view matrix

		view.createBuffer(
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			sizeof(uboVS)
		);

		// Uniform buffer object with per-object matrices
		dynamic.createBuffer(
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
		bufferSize
		);

		// Map persistent
		VK_CHECK_RESULT(view.map());
		VK_CHECK_RESULT(dynamic.map());

		// Prepare per-object matrices with offsets and random rotations
		for (uint32_t i = 0; i < OBJECT_INSTANCES; i++) {
			rotations[i] = glm::vec3(0,0,0);
			rotationSpeeds[i] = glm::vec3(0,0,0);
		}

		/*updateUniformBuffers();
		updateDynamicUniformBuffer(true);*/
	}
	
	// Do stuff
	/*void  Descriptors::updateUniformBuffer(UniformBufferObject bufferObject)
	{
		//copy the data in the uniform buffer object to the uniform buffer
		void* data;
		vkMapMemory(m_device.device, uniformBufferMemory, 0, sizeof(bufferObject), 0, &data);
		memcpy(data, &bufferObject, sizeof(bufferObject));
		vkUnmapMemory(m_device.device, uniformBufferMemory);
	}*/

	// Do stuff
	void  Descriptors::UpdateModelBuffer(glm::mat4 mat1, glm::mat4 mat2)
	{
		/*uboDataDynamic.model[0] = mat1;
		uboDataDynamic.model[1] = mat2;

		//copy the data in the uniform buffer object to the uniform buffer
		void* data;
		vkMapMemory(m_device.device, uniformBufferMemoryDynamic, 0,  dynamicAlignment, 0, &data);
		memcpy(data, uboDataDynamic.model,sizeof(uboDataDynamic));
		vkUnmapMemory(m_device.device, uniformBufferMemoryDynamic);*/

		/*VkMappedMemoryRange memoryRange{};
		memoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		memoryRange.memory = uniformBufferMemoryDynamic;
		memoryRange.size =  sizeof(uboDataDynamic);
		std::cout << memoryRange.size << std::endl;
		vkFlushMappedMemoryRanges(m_device.device, 1, &memoryRange);*/
	}

	void Descriptors::updateUniformBuffers(Camera& camera)
	{
		// Fixed ubo with projection and view matrices
		uboVS.projection = camera.GetProj();
		uboVS.view = camera.GetView();

		memcpy(view.mappedData, &uboVS, sizeof(uboVS));
	}

	void Descriptors::updateDynamicUniformBuffer(bool force)
	{
		for (int i = 0; i < OBJECT_INSTANCES; ++i)
		{
			glm::mat4* modelMat = (glm::mat4*)(((uint64_t)uboDataDynamic.model + (i * dynamicAlignment)));
			*modelMat = glm::mat4(1.f);
			*modelMat = glm::translate(*modelMat, glm::vec3(0.5f*(i+1), 0, 0));
		}

		memcpy(dynamic.mappedData, uboDataDynamic.model, dynamic.m_size);

		// Flush to make changes visible to the host 
		VkMappedMemoryRange memoryRange = vk::init::mappedMemoryRange();
		memoryRange.memory = dynamic.memory;
		memoryRange.size = dynamic.m_size;
		vkFlushMappedMemoryRanges(m_device.device, 1, &memoryRange);
	}
}