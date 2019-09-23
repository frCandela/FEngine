#pragma once

#include "core/memory/fanAlignedMemory.h"

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
		static const uint32_t s_maximumNumModels = 128;
		static const uint32_t s_maximumNumLights = 16;

		struct VertUniforms
		{
			glm::mat4 view;
			glm::mat4 proj;
		};

		struct PointLight {
			alignas(16) glm::vec3		position;
			alignas(4)  glm::float32	constant;
			alignas(16) glm::vec3		diffuse;
			alignas(4)  glm::float32	linear;
			alignas(16) glm::vec3		specular;
			alignas(4)  glm::float32	quadratic;
			alignas(16) glm::vec3		ambiant;
		};

		struct LightsUniforms {
			alignas(16) PointLight lights[ForwardPipeline::s_maximumNumLights];
			alignas(4)  uint32_t lightNum;
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

		LightsUniforms	GetLightUniforms() const { return m_pointLightUniform; }
		void			SetLightUniforms(const LightsUniforms& _lights);

		void	SetDynamicUniformVert(const DynamicUniformsVert& _dynamicUniform, const uint32_t _index);
		void	SetDynamicUniformFrag(const DynamicUniformsMaterial& _dynamicUniform, const uint32_t _index);

		void UpdateUniformBuffers();

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
		LightsUniforms m_pointLightUniform;


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