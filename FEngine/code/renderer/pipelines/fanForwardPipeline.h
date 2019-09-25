#pragma once

#include "core/memory/fanAlignedMemory.h"
#include "renderer/fanUniforms.h"

namespace fan
{
	class Mesh;
	class Model;
	class Transform;
	class Material;
	struct Vertex;
	struct MeshData;
	class Device;
	class Shader;
	class Image;
	class ImageView;
	class Buffer;
	class Texture;
	class Sampler;
	struct DrawData;

	//================================================================================================================================
	//================================================================================================================================
	class ForwardPipeline {
	public:
		struct VertUniforms
		{
			glm::mat4 view;
			glm::mat4 proj;
		};

		struct FragUniforms
		{
			glm::vec3	cameraPosition = glm::vec3(0,0,0);
		};

		struct DynamicUniformsMaterial {
			glm::vec3  color = glm::vec3(1);
			glm::int32 shininess;
			glm::int32 textureIndex;
		};

		struct DynamicUniformsVert
		{
			glm::mat4 modelMat;
			glm::mat4 rotationMat;
		};

		ForwardPipeline(Device& _device, VkRenderPass& _renderPass);
		~ForwardPipeline();

		void Create(VkExtent2D _extent);
		void Draw(VkCommandBuffer _commandBuffer, const std::vector< DrawData >& _drawData);
		void Resize(VkExtent2D _extent);
		void ReloadShaders();

		VertUniforms	GetVertUniforms() const { return m_vertUniforms; }
		void			SetVertUniforms(const VertUniforms _uniforms);
		FragUniforms	GetFragUniforms() const { return m_fragUniforms; }
		void			SetFragUniforms(const FragUniforms _fragUniforms);

		void	SetDynamicUniformVert(const DynamicUniformsVert& _dynamicUniform, const uint32_t _index);
		void	SetDynamicUniformFrag(const DynamicUniformsMaterial& _dynamicUniform, const uint32_t _index);

		void UpdateUniformBuffers( const LightsUniforms  _lightUniforms );

		VkPipeline		GetPipeline() { return m_pipeline; }
		VkImageView		GetDepthImageView();

	private:
		Device&					m_device;
		VkRenderPass&			m_renderPass;

		VkPipelineLayout		m_pipelineLayout;
		VkPipeline				m_pipeline;

		VkDescriptorSetLayout	m_descriptorSetLayoutScene;
		VkDescriptorPool		m_descriptorPoolScene;
		VkDescriptorSet			m_descriptorSetScene;

		VkDescriptorSetLayout	m_descriptorSetLayoutTextures;
		VkDescriptorPool		m_descriptorPoolTextures;
		VkDescriptorSet			m_descriptorSetTextures;

		Sampler *	m_sampler;
		Image *		m_depthImage;
		ImageView *	m_depthImageView;

		Shader *	m_fragmentShader = nullptr;
		Shader *	m_vertexShader = nullptr;

		Buffer *	m_vertUniformBuffer;
		Buffer *	m_fragUniformBuffer;
		Buffer *	m_pointLightUniformBuffer;
		Buffer *	m_dynamicUniformBufferVert;
		Buffer *	m_dynamicUniformBufferFrag;

		VertUniforms m_vertUniforms;
		FragUniforms m_fragUniforms;

		AlignedMemory<DynamicUniformsVert> m_dynamicUniformsVert;
		AlignedMemory<DynamicUniformsMaterial> m_dynamicUniformsFrag;

		size_t m_dynamicAlignmentVert;
		size_t m_dynamicAlignmentFrag;

		void CreateShaders();
		bool CreateDescriptors();
		bool CreateDescriptorsScene();
		bool CreateDescriptorsTextures();
		bool CreateDepthRessources(VkExtent2D _extent);
		bool CreatePipeline(VkExtent2D _extent);

		void DeleteDepthRessources();
		void DeletePipeline();
		void DeleteDescriptors();

	};
}