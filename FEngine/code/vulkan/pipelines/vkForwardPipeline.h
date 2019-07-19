#pragma once

#include "util/fanAlignedMemory.h"

namespace scene { 
	class Mesh;  
	class Transform;
}

namespace vk {
	struct Vertex;
	class Device;
	class Shader;
	class Image;
	class ImageView;
	class Buffer;


	struct MeshData {
		scene::Mesh * mesh;
		scene::Transform * transform;
		vk::Buffer * indexBuffer;
		vk::Buffer * vertexBuffer;
	};

	class ForwardPipeline {
	public:

		struct VertUniforms
		{
			glm::mat4 view;
			glm::mat4 proj;
		};
		struct FragUniforms
		{
			glm::vec3 cameraPosition;
			float ambiantIntensity;
			glm::vec3 lightColor;
			glm::int32 specularHardness;
			glm::vec3 lightPos;
		};
		struct DynamicUniforms
		{
			glm::mat4 modelMat;
			glm::mat4 rotationMat;
		};


		ForwardPipeline(Device& _device, VkRenderPass& _renderPass);
		~ForwardPipeline();

		void Create( VkExtent2D _extent );
		void Draw(VkCommandBuffer _commandBuffer, const std::vector<MeshData>& _meshData );
		void Resize(VkExtent2D _extent);
		void ReloadShaders();

		VertUniforms GetVertUniforms() const { return m_vertUniforms; }
		void SetVertUniforms(const VertUniforms _uniforms);
		FragUniforms GetFragUniforms() const { return m_fragUniforms; }
		void SetFragUniforms(const FragUniforms _fragUniforms);
		void SetDynamicUniforms( const std::vector<DynamicUniforms> & _dynamicUniforms );

		VkPipeline		GetPipeline() { return m_pipeline; }
		VkImageView		GetDepthImageView();

	private:
		Device& m_device;
		VkRenderPass& m_renderPass;

		VkPipelineLayout	m_pipelineLayout;
		VkPipeline			m_pipeline;

		VkDescriptorSetLayout	m_descriptorSetLayout;
		VkDescriptorPool		m_descriptorPool;
		VkDescriptorSet			m_descriptorSet;

		Image *			m_depthImage;
		ImageView  *	m_depthImageView;

		Shader * m_fragmentShader = nullptr;
		Shader * m_vertexShader = nullptr;

		Buffer * m_dynamicUniformBuffer;
		Buffer * m_vertUniformBuffer;
		Buffer * m_fragUniformBuffer;

		Buffer * m_indexBuffer;
		Buffer * m_vertexBuffer;

		VertUniforms m_vertUniforms;
		FragUniforms m_fragUniforms;
		util::AlignedMemory<DynamicUniforms> m_dynamicUniformsArray;
		size_t m_dynamicAlignment;

		void CreateShaders();
		bool CreateDescriptors();
		bool CreateDepthRessources(	VkExtent2D _extent);
		bool CreatePipeline(		VkExtent2D _extent );

		void DeleteDepthRessources();
		void DeletePipeline();
		void DeleteDescriptors();

	};
}
