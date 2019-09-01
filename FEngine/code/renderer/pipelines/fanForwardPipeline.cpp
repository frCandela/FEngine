#include "fanGlobalIncludes.h"

#include "renderer/pipelines/fanForwardPipeline.h"
#include "renderer/fanRessourceManager.h"
#include "renderer/core/fanDevice.h"
#include "renderer/core/fanShader.h"
#include "renderer/core/fanImage.h"
#include "renderer/core/fanImageView.h" 
#include "renderer/core/fanBuffer.h"
#include "renderer/core/fanTexture.h"
#include "renderer/core/fanSampler.h"
#include "renderer/fanRenderer.h"
#include "renderer/util/fanVertex.h"
#include "scene/components/fanModel.h"
#include "renderer/fanMesh.h"

namespace fan
{
	namespace vk {
		//================================================================================================================================
		//================================================================================================================================
		ForwardPipeline::ForwardPipeline(Device& _device, VkRenderPass& _renderPass) :
			m_device(_device)
			, m_renderPass(_renderPass) {

			m_sampler = new Sampler(_device);
			m_sampler->CreateSampler(0, 8);

			// Calculate required alignment based on minimum device offset alignment
			size_t minUboAlignment = _device.GetDeviceProperties().limits.minUniformBufferOffsetAlignment;
			m_dynamicAlignmentVert = sizeof(DynamicUniformsVert);
			m_dynamicAlignmentFrag = sizeof(DynamicUniformsFrag);
			if (minUboAlignment > 0) {
				m_dynamicAlignmentVert = ((sizeof(DynamicUniformsVert) + minUboAlignment - 1) & ~(minUboAlignment - 1));
				m_dynamicAlignmentFrag = ((sizeof(DynamicUniformsFrag) + minUboAlignment - 1) & ~(minUboAlignment - 1));
			}
			m_dynamicUniformsVert.Resize(s_maximumNumModels * m_dynamicAlignmentVert, m_dynamicAlignmentVert);
			m_dynamicUniformsFrag.Resize(s_maximumNumModels * m_dynamicAlignmentFrag, m_dynamicAlignmentFrag);

			for (int uniformIndex = 0; uniformIndex < s_maximumNumModels; uniformIndex++) {
				m_dynamicUniformsFrag[uniformIndex].textureIndex = 0;
			}

			m_fragUniforms.ambiantIntensity = 0.2f;
			m_fragUniforms.lightColor = glm::vec3(1, 1, 1);
			m_fragUniforms.specularHardness = 32;
			m_fragUniforms.lightPos = glm::vec3(0, 2, 0);
		}

		//================================================================================================================================
		//================================================================================================================================
		ForwardPipeline::~ForwardPipeline() {
			DeletePipeline();
			DeleteDepthRessources();
			DeleteDescriptors();

			delete m_fragmentShader;
			delete m_vertexShader;
			delete m_sampler;
		}

		//================================================================================================================================
		//================================================================================================================================
		void ForwardPipeline::Create(VkExtent2D _extent) {
			CreateShaders();
			CreateDepthRessources(_extent);
			CreateDescriptors();
			CreatePipeline(_extent);
		}

		//================================================================================================================================
		//================================================================================================================================
		void ForwardPipeline::Resize(VkExtent2D _extent) {
			DeleteDepthRessources();
			DeleteDescriptors();
			DeletePipeline();

			CreateDepthRessources(_extent);
			CreateDescriptors();
			CreatePipeline(_extent);

			SetVertUniforms(m_vertUniforms);
			SetFragUniforms(m_fragUniforms);
			//SetDynamicUniformsVert({ {glm::mat4(1.0)},{glm::mat4(1.0)} });
		}

		//================================================================================================================================
		//================================================================================================================================
		void ForwardPipeline::ReloadShaders() {
			m_vertexShader->Reload();
			m_fragmentShader->Reload();
		}

		//================================================================================================================================
		//================================================================================================================================
		VkImageView	ForwardPipeline::GetDepthImageView() {
			return m_depthImageView->GetImageView();
		}

		//================================================================================================================================
		//================================================================================================================================
		void ForwardPipeline::SetVertUniforms(const VertUniforms _vertUniforms) {
			m_vertUniforms = _vertUniforms;
			m_vertUniformBuffer->SetData(&m_vertUniforms, sizeof(VertUniforms));
		}

		//================================================================================================================================
		//================================================================================================================================
		void ForwardPipeline::SetFragUniforms(const FragUniforms _fragUniforms) {
			m_fragUniforms = _fragUniforms;
			m_fragUniformBuffer->SetData(&m_fragUniforms, sizeof(FragUniforms));
		}

		// 	//================================================================================================================================
		// 	//================================================================================================================================
		// 	void ForwardPipeline::SetDynamicUniformsVert( const std::vector<DynamicUniformsVert> & _dynamicUniforms ) {
		// 		for (int dynamicUniformIndex = 0; dynamicUniformIndex < _dynamicUniforms.size(); dynamicUniformIndex++) {
		// 			m_dynamicUniformVert[dynamicUniformIndex] = _dynamicUniforms[dynamicUniformIndex];
		// 		}
		// 		m_dynamicUniformBufferVert->SetData(&m_dynamicUniformVert[0], m_dynamicUniformVert.GetSize());
		// 	}
		// 
		// 	//================================================================================================================================
		// 	//================================================================================================================================
		// 	void ForwardPipeline::SetDynamicUniformsFrag(const std::vector<DynamicUniformsFrag> & _dynamicUniforms) {
		// 		for (int dynamicUniformIndex = 0; dynamicUniformIndex < _dynamicUniforms.size(); dynamicUniformIndex++) {
		// 			m_dynamicUniformsFrag[dynamicUniformIndex] = _dynamicUniforms[dynamicUniformIndex];
		// 		}
		// 		m_dynamicUniformBufferFrag->SetData(&m_dynamicUniformsFrag[0], m_dynamicUniformsFrag.GetSize());
		// 	}

			//================================================================================================================================
			//================================================================================================================================
		void ForwardPipeline::SetDynamicUniformVert(const DynamicUniformsVert& _dynamicUniform, const uint32_t _index) {
			assert(_index < s_maximumNumModels);
			m_dynamicUniformsVert[_index] = _dynamicUniform;
		}

		//================================================================================================================================
		//================================================================================================================================
		void ForwardPipeline::SetDynamicUniformFrag(const DynamicUniformsFrag& _dynamicUniform, const uint32_t _index) {
			assert(_index < s_maximumNumModels);
			m_dynamicUniformsFrag[_index] = _dynamicUniform;
		}

		//================================================================================================================================
		//================================================================================================================================
		void ForwardPipeline::UpdateDynamicUniformVert() {
			m_dynamicUniformBufferVert->SetData(&m_dynamicUniformsVert[0], m_dynamicUniformsVert.GetSize());
		}

		//================================================================================================================================
		//================================================================================================================================
		void ForwardPipeline::UpdateDynamicUniformFrag() {
			m_dynamicUniformBufferFrag->SetData(&m_dynamicUniformsFrag[0], m_dynamicUniformsFrag.GetSize());
		}

		//================================================================================================================================
		//================================================================================================================================
		void ForwardPipeline::Draw(VkCommandBuffer _commandBuffer, const std::vector< DrawData >& _drawData) {
			vkCmdBindPipeline(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);

			VkDeviceSize offsets[] = { 0 };

			for (int drawDataIndex = 0; drawDataIndex < _drawData.size(); drawDataIndex++) {
				const DrawData&  drawData = _drawData[drawDataIndex];

				if (drawData.model != nullptr) {

					VkBuffer vertexBuffers[] = { drawData.meshData->vertexBuffer->GetBuffer() };
					vkCmdBindVertexBuffers(_commandBuffer, 0, 1, vertexBuffers, offsets);
					vkCmdBindIndexBuffer(_commandBuffer, drawData.meshData->indexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);

					std::vector<uint32_t> dynamicOffsets = {
						drawDataIndex  * static_cast<uint32_t>(m_dynamicAlignmentVert)
						,	drawDataIndex  * static_cast<uint32_t>(m_dynamicAlignmentFrag)
					};

					std::vector<VkDescriptorSet> descriptors = {
						m_descriptorSetScene
						, m_descriptorSetTextures
					};

					vkCmdBindDescriptorSets(
						_commandBuffer,
						VK_PIPELINE_BIND_POINT_GRAPHICS,
						m_pipelineLayout,
						0,
						static_cast<uint32_t>(descriptors.size()),
						descriptors.data(),
						static_cast<uint32_t>(dynamicOffsets.size()),
						dynamicOffsets.data()
					);
					vkCmdDrawIndexed(_commandBuffer, static_cast<uint32_t>(drawData.meshData->mesh->GetIndices().size()), 1, 0, 0, 0);
				}
			}
		}

		//================================================================================================================================
		//================================================================================================================================
		void ForwardPipeline::CreateShaders() {
			delete m_fragmentShader;
			delete m_vertexShader;

			m_fragmentShader = new Shader(m_device);
			m_fragmentShader->Create("code/shaders/forward.frag");

			m_vertexShader = new Shader(m_device);
			m_vertexShader->Create("code/shaders/forward.vert");
		}

		//================================================================================================================================
		//================================================================================================================================
		bool ForwardPipeline::CreateDescriptors() {
			return 		CreateDescriptorsScene()
				&& CreateDescriptorsTextures();
		}
		//================================================================================================================================
		//================================================================================================================================
		bool ForwardPipeline::CreateDescriptorsTextures() {
			std::vector< vk::Texture * > & textures = Renderer::GetRenderer().GetRessourceManager()->GetTextures();

			// LAYOUTS
			VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
			samplerLayoutBinding.binding = 0;
			samplerLayoutBinding.descriptorCount = static_cast<uint32_t>(textures.size());
			samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			samplerLayoutBinding.pImmutableSamplers = nullptr;
			samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

			std::vector< VkDescriptorSetLayoutBinding > layoutBindings = {
				samplerLayoutBinding
			};

			VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
			descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			descriptorSetLayoutCreateInfo.pNext = nullptr;
			descriptorSetLayoutCreateInfo.flags = 0;
			descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
			descriptorSetLayoutCreateInfo.pBindings = layoutBindings.data();

			if (vkCreateDescriptorSetLayout(m_device.vkDevice, &descriptorSetLayoutCreateInfo, nullptr, &m_descriptorSetLayoutTextures) != VK_SUCCESS) {
				fan::Debug::Error("Could not allocate descriptor set layout.");
				return false;
			}
			fan::Debug::Get() << fan::Debug::Severity::log << std::hex << "VkDescriptorSetLayout\t" << m_descriptorSetLayoutTextures << std::dec << std::endl;

			// Pool
			std::vector< VkDescriptorPoolSize > poolSizes(1);
			poolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			poolSizes[0].descriptorCount = static_cast<uint32_t>(textures.size());

			VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
			descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			descriptorPoolCreateInfo.pNext = nullptr;
			descriptorPoolCreateInfo.flags = 0;
			descriptorPoolCreateInfo.maxSets = 1;
			descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
			descriptorPoolCreateInfo.pPoolSizes = poolSizes.data();

			if (vkCreateDescriptorPool(m_device.vkDevice, &descriptorPoolCreateInfo, nullptr, &m_descriptorPoolTextures) != VK_SUCCESS) {
				fan::Debug::Error("Could not allocate descriptor pool.");
				return false;
			}
			fan::Debug::Get() << fan::Debug::Severity::log << std::hex << "VkDescriptorPool\t" << m_descriptorPoolTextures << std::dec << std::endl;

			//================================================================
			// DescriptorSet layout
			std::vector< VkDescriptorSetLayout > descriptorSetLayouts = {
				m_descriptorSetLayoutTextures
			};

			VkDescriptorSetAllocateInfo descriptorSetAllocateInfo;
			descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			descriptorSetAllocateInfo.pNext = nullptr;
			descriptorSetAllocateInfo.descriptorPool = m_descriptorPoolTextures;
			descriptorSetAllocateInfo.descriptorSetCount = static_cast<uint32_t>(descriptorSetLayouts.size());
			descriptorSetAllocateInfo.pSetLayouts = descriptorSetLayouts.data();

			std::vector<VkDescriptorSet> descriptorSets(descriptorSetLayouts.size());
			if (vkAllocateDescriptorSets(m_device.vkDevice, &descriptorSetAllocateInfo, descriptorSets.data()) != VK_SUCCESS) {
				fan::Debug::Error("Could not allocate descriptor set.");
				return false;
			}
			m_descriptorSetTextures = descriptorSets[0];
			fan::Debug::Get() << fan::Debug::Severity::log << std::hex << "VkDescriptorSet\t\t" << m_descriptorSetTextures << std::dec << std::endl;


			//================================================================
			// Textures		
			std::vector<VkDescriptorImageInfo> imageInfoArray;
			imageInfoArray.reserve(textures.size());
			for (int textureIndex = 0; textureIndex < textures.size(); textureIndex++) {
				VkDescriptorImageInfo imageInfo;
				imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				imageInfo.imageView = textures[textureIndex]->GetImageView();
				imageInfo.sampler = m_sampler->GetSampler();
				imageInfoArray.push_back(imageInfo);
			}

			VkWriteDescriptorSet textureWriteDescriptorSet = {};
			textureWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			textureWriteDescriptorSet.pNext = nullptr;
			textureWriteDescriptorSet.dstSet = m_descriptorSetTextures;
			textureWriteDescriptorSet.dstBinding = 0;
			textureWriteDescriptorSet.dstArrayElement = 0;
			textureWriteDescriptorSet.descriptorCount = static_cast<uint32_t>(imageInfoArray.size());
			textureWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			textureWriteDescriptorSet.pImageInfo = imageInfoArray.data();
			textureWriteDescriptorSet.pBufferInfo = nullptr;
			textureWriteDescriptorSet.pTexelBufferView = nullptr;

			//================================================================
			// Update DescriptorSets
			std::vector<VkWriteDescriptorSet> writeDescriptors = {
				 textureWriteDescriptorSet
			};

			vkUpdateDescriptorSets(
				m_device.vkDevice,
				static_cast<uint32_t>(writeDescriptors.size()),
				writeDescriptors.data(),
				0,
				nullptr
			);

			return true;
		}

		//================================================================================================================================
		//================================================================================================================================
		bool ForwardPipeline::CreateDescriptorsScene() {
			// LAYOUTS
			VkDescriptorSetLayoutBinding uboLayoutBinding;
			uboLayoutBinding.binding = 0;
			uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			uboLayoutBinding.descriptorCount = 1;
			uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			uboLayoutBinding.pImmutableSamplers = nullptr;

			VkDescriptorSetLayoutBinding dynamicLayoutBinding;
			dynamicLayoutBinding.binding = 1;
			dynamicLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
			dynamicLayoutBinding.descriptorCount = 1;
			dynamicLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			dynamicLayoutBinding.pImmutableSamplers = nullptr;

			VkDescriptorSetLayoutBinding fragLayoutBinding;
			fragLayoutBinding.binding = 2;
			fragLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			fragLayoutBinding.descriptorCount = 1;
			fragLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
			fragLayoutBinding.pImmutableSamplers = nullptr;

			VkDescriptorSetLayoutBinding dynamicLayoutBindingFrag;
			dynamicLayoutBindingFrag.binding = 3;
			dynamicLayoutBindingFrag.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
			dynamicLayoutBindingFrag.descriptorCount = 1;
			dynamicLayoutBindingFrag.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
			dynamicLayoutBindingFrag.pImmutableSamplers = nullptr;

			std::vector< VkDescriptorSetLayoutBinding > layoutBindings = {
				  uboLayoutBinding
				, dynamicLayoutBinding
				, fragLayoutBinding
				, dynamicLayoutBindingFrag
			};

			VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
			descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			descriptorSetLayoutCreateInfo.pNext = nullptr;
			descriptorSetLayoutCreateInfo.flags = 0;
			descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
			descriptorSetLayoutCreateInfo.pBindings = layoutBindings.data();

			if (vkCreateDescriptorSetLayout(m_device.vkDevice, &descriptorSetLayoutCreateInfo, nullptr, &m_descriptorSetLayoutScene) != VK_SUCCESS) {
				fan::Debug::Error("Could not allocate descriptor set layout.");
				return false;
			}
			fan::Debug::Get() << fan::Debug::Severity::log << std::hex << "VkDescriptorSetLayout\t" << m_descriptorSetLayoutScene << std::dec << std::endl;

			// Pool
			std::vector< VkDescriptorPoolSize > poolSizes(4);
			poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			poolSizes[0].descriptorCount = 1;
			poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
			poolSizes[1].descriptorCount = 1;
			poolSizes[2].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			poolSizes[2].descriptorCount = 1;
			poolSizes[3].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
			poolSizes[3].descriptorCount = 1;

			VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
			descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			descriptorPoolCreateInfo.pNext = nullptr;
			descriptorPoolCreateInfo.flags = 0;
			descriptorPoolCreateInfo.maxSets = 1;
			descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
			descriptorPoolCreateInfo.pPoolSizes = poolSizes.data();

			if (vkCreateDescriptorPool(m_device.vkDevice, &descriptorPoolCreateInfo, nullptr, &m_descriptorPoolScene) != VK_SUCCESS) {
				fan::Debug::Error("Could not allocate descriptor pool.");
				return false;
			}
			fan::Debug::Get() << fan::Debug::Severity::log << std::hex << "VkDescriptorPool\t" << m_descriptorPoolScene << std::dec << std::endl;

			//================================================================
			// DescriptorSet layout
			std::vector< VkDescriptorSetLayout > descriptorSetLayouts = {
				m_descriptorSetLayoutScene
			};

			VkDescriptorSetAllocateInfo descriptorSetAllocateInfo;
			descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			descriptorSetAllocateInfo.pNext = nullptr;
			descriptorSetAllocateInfo.descriptorPool = m_descriptorPoolScene;
			descriptorSetAllocateInfo.descriptorSetCount = static_cast<uint32_t>(descriptorSetLayouts.size());
			descriptorSetAllocateInfo.pSetLayouts = descriptorSetLayouts.data();

			std::vector<VkDescriptorSet> descriptorSets(descriptorSetLayouts.size());
			if (vkAllocateDescriptorSets(m_device.vkDevice, &descriptorSetAllocateInfo, descriptorSets.data()) != VK_SUCCESS) {
				fan::Debug::Error("Could not allocate descriptor set.");
				return false;
			}
			m_descriptorSetScene = descriptorSets[0];
			fan::Debug::Get() << fan::Debug::Severity::log << std::hex << "VkDescriptorSet\t\t" << m_descriptorSetScene << std::dec << std::endl;


			//================================================================
			// Uniform buffers

			// Vert
			VkWriteDescriptorSet uboWriteDescriptorSet = {};
			{
				m_vertUniformBuffer = new Buffer(m_device);
				m_vertUniformBuffer->Create(
					sizeof(VertUniforms),
					VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
					VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				);

				VkDescriptorBufferInfo uboDescriptorBufferInfo = {};
				uboDescriptorBufferInfo.buffer = m_vertUniformBuffer->GetBuffer();
				uboDescriptorBufferInfo.offset = 0;
				uboDescriptorBufferInfo.range = sizeof(VertUniforms);


				uboWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				uboWriteDescriptorSet.pNext = nullptr;
				uboWriteDescriptorSet.dstSet = m_descriptorSetScene;
				uboWriteDescriptorSet.dstBinding = 0;
				uboWriteDescriptorSet.dstArrayElement = 0;
				uboWriteDescriptorSet.descriptorCount = 1;
				uboWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				uboWriteDescriptorSet.pImageInfo = nullptr;
				uboWriteDescriptorSet.pBufferInfo = &uboDescriptorBufferInfo;
				//uboWriteDescriptorSet.pTexelBufferView = nullptr;
			}

			// Frag
			VkWriteDescriptorSet fragWriteDescriptorSet = {};
			{
				m_fragUniformBuffer = new Buffer(m_device);
				m_fragUniformBuffer->Create(
					sizeof(FragUniforms),
					VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
					VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				);

				VkDescriptorBufferInfo fragDescriptorBufferInfo = {};
				fragDescriptorBufferInfo.buffer = m_fragUniformBuffer->GetBuffer();
				fragDescriptorBufferInfo.offset = 0;
				fragDescriptorBufferInfo.range = sizeof(FragUniforms);

				fragWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				fragWriteDescriptorSet.pNext = nullptr;
				fragWriteDescriptorSet.dstSet = m_descriptorSetScene;
				fragWriteDescriptorSet.dstBinding = 2;
				fragWriteDescriptorSet.dstArrayElement = 0;
				fragWriteDescriptorSet.descriptorCount = 1;
				fragWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				fragWriteDescriptorSet.pImageInfo = nullptr;
				fragWriteDescriptorSet.pBufferInfo = &fragDescriptorBufferInfo;
				//uboWriteDescriptorSet.pTexelBufferView = nullptr;
			}

			// Dynamic vert
			VkWriteDescriptorSet dynamicWriteDescriptorSet = {};
			{
				m_dynamicUniformBufferVert = new Buffer(m_device);
				m_dynamicUniformBufferVert->Create(
					m_dynamicUniformsVert.GetSize() * m_dynamicAlignmentVert,
					VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
					VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				);

				VkDescriptorBufferInfo dynamicDescriptorBufferInfo = {};
				dynamicDescriptorBufferInfo.buffer = m_dynamicUniformBufferVert->GetBuffer();
				dynamicDescriptorBufferInfo.offset = 0;
				dynamicDescriptorBufferInfo.range = m_dynamicAlignmentVert;


				dynamicWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				dynamicWriteDescriptorSet.pNext = nullptr;
				dynamicWriteDescriptorSet.dstSet = m_descriptorSetScene;
				dynamicWriteDescriptorSet.dstBinding = 1;
				dynamicWriteDescriptorSet.dstArrayElement = 0;
				dynamicWriteDescriptorSet.descriptorCount = 1;
				dynamicWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
				dynamicWriteDescriptorSet.pImageInfo = nullptr;
				dynamicWriteDescriptorSet.pBufferInfo = &dynamicDescriptorBufferInfo;
				//uboWriteDescriptorSet.pTexelBufferView = nullptr;
			}

			// Dynamic frag
			VkWriteDescriptorSet dynamicFragWriteDescriptorSet = {};
			{
				m_dynamicUniformBufferFrag = new Buffer(m_device);
				m_dynamicUniformBufferFrag->Create(
					m_dynamicUniformsFrag.GetSize() * m_dynamicAlignmentFrag,
					VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
					VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				);

				VkDescriptorBufferInfo dynamicDescriptorBufferInfo = {};
				dynamicDescriptorBufferInfo.buffer = m_dynamicUniformBufferFrag->GetBuffer();
				dynamicDescriptorBufferInfo.offset = 0;
				dynamicDescriptorBufferInfo.range = m_dynamicAlignmentFrag;

				dynamicFragWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				dynamicFragWriteDescriptorSet.pNext = nullptr;
				dynamicFragWriteDescriptorSet.dstSet = m_descriptorSetScene;
				dynamicFragWriteDescriptorSet.dstBinding = 3;
				dynamicFragWriteDescriptorSet.dstArrayElement = 0;
				dynamicFragWriteDescriptorSet.descriptorCount = 1;
				dynamicFragWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
				dynamicFragWriteDescriptorSet.pImageInfo = nullptr;
				dynamicFragWriteDescriptorSet.pBufferInfo = &dynamicDescriptorBufferInfo;
				//uboWriteDescriptorSet.pTexelBufferView = nullptr;
			}

			//================================================================
			// Update DescriptorSets
			std::vector<VkWriteDescriptorSet> writeDescriptors = {
				  uboWriteDescriptorSet
				, dynamicWriteDescriptorSet
				, fragWriteDescriptorSet
				, dynamicFragWriteDescriptorSet
			};

			vkUpdateDescriptorSets(
				m_device.vkDevice,
				static_cast<uint32_t>(writeDescriptors.size()),
				writeDescriptors.data(),
				0,
				nullptr
			);

			return true;
		}

		//================================================================================================================================
		//================================================================================================================================
		bool ForwardPipeline::CreateDepthRessources(VkExtent2D _extent) {
			VkFormat depthFormat = m_device.FindDepthFormat();
			m_depthImage = new Image(m_device);
			m_depthImageView = new ImageView(m_device);
			m_depthImage->Create(depthFormat, _extent, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			m_depthImageView->Create(m_depthImage->GetImage(), depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_VIEW_TYPE_2D);

			VkCommandBuffer cmd = Renderer::GetRenderer().BeginSingleTimeCommands();
			m_depthImage->TransitionImageLayout(cmd, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);
			Renderer::GetRenderer().EndSingleTimeCommands(cmd);

			return true;
		}

		//================================================================================================================================
		//================================================================================================================================
		bool ForwardPipeline::CreatePipeline(VkExtent2D _extent) {
			VkPipelineShaderStageCreateInfo vertshaderStageCreateInfos = {};
			vertshaderStageCreateInfos.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			vertshaderStageCreateInfos.pNext = nullptr;
			vertshaderStageCreateInfos.flags = 0;
			vertshaderStageCreateInfos.stage = VK_SHADER_STAGE_VERTEX_BIT;
			vertshaderStageCreateInfos.module = m_vertexShader->GetModule();
			vertshaderStageCreateInfos.pName = "main";
			vertshaderStageCreateInfos.pSpecializationInfo = nullptr;

			VkPipelineShaderStageCreateInfo fragShaderStageCreateInfos = {};
			fragShaderStageCreateInfos.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			fragShaderStageCreateInfos.pNext = nullptr;
			fragShaderStageCreateInfos.flags = 0;
			fragShaderStageCreateInfos.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			fragShaderStageCreateInfos.module = m_fragmentShader->GetModule();
			fragShaderStageCreateInfos.pName = "main";
			fragShaderStageCreateInfos.pSpecializationInfo = nullptr;

			std::vector < VkPipelineShaderStageCreateInfo> shaderStages = { vertshaderStageCreateInfos, fragShaderStageCreateInfos };
			std::vector < VkVertexInputBindingDescription > bindingDescription = Vertex::GetBindingDescription();
			std::vector < VkVertexInputAttributeDescription > attributeDescriptions = Vertex::GetAttributeDescriptions();

			VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {};
			vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			vertexInputStateCreateInfo.pNext = nullptr;
			vertexInputStateCreateInfo.flags = 0;
			vertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescription.size());
			vertexInputStateCreateInfo.pVertexBindingDescriptions = bindingDescription.data();
			vertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());;
			vertexInputStateCreateInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

			VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = {};
			inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			inputAssemblyStateCreateInfo.pNext = nullptr;
			inputAssemblyStateCreateInfo.flags = 0;
			inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

			std::vector< VkViewport > viewports(1);
			viewports[0].x = 0.f;
			viewports[0].y = 0.f;
			viewports[0].width = static_cast<float> (_extent.width);
			viewports[0].height = static_cast<float> (_extent.height);
			viewports[0].minDepth = 0.0f;
			viewports[0].maxDepth = 1.0f;

			std::vector<VkRect2D> scissors(1);
			scissors[0].offset = { 0, 0 };
			scissors[0].extent = _extent;

			VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
			viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			viewportStateCreateInfo.pNext = nullptr;
			viewportStateCreateInfo.flags = 0;
			viewportStateCreateInfo.viewportCount = static_cast<uint32_t> (viewports.size());
			viewportStateCreateInfo.pViewports = viewports.data();
			viewportStateCreateInfo.scissorCount = static_cast<uint32_t> (scissors.size());;
			viewportStateCreateInfo.pScissors = scissors.data();

			VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {};
			rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			rasterizationStateCreateInfo.pNext = nullptr;
			rasterizationStateCreateInfo.flags = 0;
			rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
			rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
			rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
			rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
			rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
			rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
			rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
			rasterizationStateCreateInfo.depthBiasClamp = 0.0f;
			rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;
			rasterizationStateCreateInfo.lineWidth = 1.0f;

			VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = {};
			multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			multisampleStateCreateInfo.pNext = nullptr;
			multisampleStateCreateInfo.flags = 0;
			multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
			multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
			//multisampleStateCreateInfo.minSampleShading=;
			//multisampleStateCreateInfo.pSampleMask=;
			//multisampleStateCreateInfo.alphaToCoverageEnable=;
			//multisampleStateCreateInfo.alphaToOneEnable=;

			VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo = {};
			depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
			depthStencilStateCreateInfo.pNext = nullptr;
			depthStencilStateCreateInfo.flags = 0;
			depthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
			depthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
			depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
			depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
			depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
			//depthStencilStateCreateInfo.front=;
			//depthStencilStateCreateInfo.back=;
			//depthStencilStateCreateInfo.minDepthBounds=;
			//depthStencilStateCreateInfo.maxDepthBounds=;

			std::vector<VkPipelineColorBlendAttachmentState> attachmentBlendStates(1);
			attachmentBlendStates[0].blendEnable = VK_FALSE;
			attachmentBlendStates[0].srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
			attachmentBlendStates[0].dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
			attachmentBlendStates[0].colorBlendOp = VK_BLEND_OP_ADD;
			attachmentBlendStates[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			attachmentBlendStates[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
			attachmentBlendStates[0].alphaBlendOp = VK_BLEND_OP_ADD;
			attachmentBlendStates[0].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

			VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {};
			colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			colorBlendStateCreateInfo.pNext = nullptr;
			colorBlendStateCreateInfo.flags = 0;
			colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
			colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
			colorBlendStateCreateInfo.attachmentCount = static_cast<uint32_t>(attachmentBlendStates.size());
			colorBlendStateCreateInfo.pAttachments = attachmentBlendStates.data();
			colorBlendStateCreateInfo.blendConstants[0] = 0.0f;
			colorBlendStateCreateInfo.blendConstants[1] = 0.0f;
			colorBlendStateCreateInfo.blendConstants[2] = 0.0f;
			colorBlendStateCreateInfo.blendConstants[3] = 0.0f;

			std::vector<VkDynamicState> dynamicStates = {
			};

			VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
			dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			dynamicStateCreateInfo.pNext = nullptr;
			dynamicStateCreateInfo.flags = 0;
			dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
			dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();

			std::vector<VkDescriptorSetLayout> descriptorSetLayouts = {
				m_descriptorSetLayoutScene
				, m_descriptorSetLayoutTextures
			};
			std::vector<VkPushConstantRange> pushConstantRanges(0);

			VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
			pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipelineLayoutCreateInfo.pNext = nullptr;
			pipelineLayoutCreateInfo.flags = 0;
			pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
			pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayouts.data();
			pipelineLayoutCreateInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
			pipelineLayoutCreateInfo.pPushConstantRanges = pushConstantRanges.data();

			if (vkCreatePipelineLayout(m_device.vkDevice, &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
				fan::Debug::Error("Could not allocate command pool.");
				return false;
			}
			fan::Debug::Get() << fan::Debug::Severity::log << std::hex << "VkPipelineLayout\t" << m_pipelineLayout << std::dec << std::endl;

			VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
			graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			graphicsPipelineCreateInfo.pNext = nullptr;
			graphicsPipelineCreateInfo.flags = 0;
			graphicsPipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
			graphicsPipelineCreateInfo.pStages = shaderStages.data();
			graphicsPipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
			graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
			graphicsPipelineCreateInfo.pTessellationState = nullptr;
			graphicsPipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
			graphicsPipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
			graphicsPipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
			graphicsPipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;
			graphicsPipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
			graphicsPipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
			graphicsPipelineCreateInfo.layout = m_pipelineLayout;
			graphicsPipelineCreateInfo.renderPass = m_renderPass;
			graphicsPipelineCreateInfo.subpass = 0;
			graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
			graphicsPipelineCreateInfo.basePipelineIndex = -1;

			std::vector<VkGraphicsPipelineCreateInfo> graphicsPipelineCreateInfos = {
				graphicsPipelineCreateInfo
			};

			std::vector<VkPipeline> graphicsPipelines(graphicsPipelineCreateInfos.size());

			if (vkCreateGraphicsPipelines(
				m_device.vkDevice,
				VK_NULL_HANDLE,
				static_cast<uint32_t>(graphicsPipelineCreateInfos.size()),
				graphicsPipelineCreateInfos.data(),
				nullptr,
				graphicsPipelines.data()
			) != VK_SUCCESS) {
				fan::Debug::Error("Could not allocate graphicsPipelines.");
				return false;
			}

			m_pipeline = graphicsPipelines[0];

			for (int pipelineIndex = 0; pipelineIndex < graphicsPipelines.size(); pipelineIndex++) {
				fan::Debug::Get() << fan::Debug::Severity::log << std::hex << "VkPipeline\t\t" << graphicsPipelines[pipelineIndex] << std::dec << std::endl;
			}
			return true;
		}

		//================================================================================================================================
		//================================================================================================================================
		void ForwardPipeline::DeleteDepthRessources() {
			delete m_depthImageView;
			delete m_depthImage;
		}

		//================================================================================================================================
		//================================================================================================================================
		void ForwardPipeline::DeletePipeline() {
			if (m_pipelineLayout != VK_NULL_HANDLE) {
				vkDestroyPipelineLayout(m_device.vkDevice, m_pipelineLayout, nullptr);
				m_pipelineLayout = VK_NULL_HANDLE;
			}

			if (m_pipeline != VK_NULL_HANDLE) {
				vkDestroyPipeline(m_device.vkDevice, m_pipeline, nullptr);
				m_pipeline = VK_NULL_HANDLE;
			}
		}

		//================================================================================================================================
		//================================================================================================================================
		void ForwardPipeline::DeleteDescriptors() {

			if (m_descriptorPoolScene != VK_NULL_HANDLE) {
				vkDestroyDescriptorPool(m_device.vkDevice, m_descriptorPoolScene, nullptr);
				m_descriptorPoolScene = VK_NULL_HANDLE;
			}

			if (m_descriptorSetLayoutScene != VK_NULL_HANDLE) {
				vkDestroyDescriptorSetLayout(m_device.vkDevice, m_descriptorSetLayoutScene, nullptr);
				m_descriptorSetLayoutScene = VK_NULL_HANDLE;
			}

			if (m_descriptorPoolTextures != VK_NULL_HANDLE) {
				vkDestroyDescriptorPool(m_device.vkDevice, m_descriptorPoolTextures, nullptr);
				m_descriptorPoolTextures = VK_NULL_HANDLE;
			}

			if (m_descriptorSetLayoutTextures != VK_NULL_HANDLE) {
				vkDestroyDescriptorSetLayout(m_device.vkDevice, m_descriptorSetLayoutTextures, nullptr);
				m_descriptorSetLayoutTextures = VK_NULL_HANDLE;
			}

			delete m_vertUniformBuffer;
			delete m_fragUniformBuffer;
			delete m_dynamicUniformBufferVert;
			delete m_dynamicUniformBufferFrag;
		}
	}
}