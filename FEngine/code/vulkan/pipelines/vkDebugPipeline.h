#pragma once

namespace vk {
	struct Vertex;
	class Device;
	class Shader;
	class Image;
	class ImageView;
	class Buffer;

	class DebugPipeline {
	public:

		struct Uniforms
		{
			glm::mat4 model;
			glm::mat4 view;
			glm::mat4 proj;
			glm::vec4 color;
		};

		DebugPipeline(Device& _device, VkRenderPass& _renderPass, const int _swapchainImagesCount);
		~DebugPipeline();

		void Create( VkExtent2D _extent );
		void Draw(VkCommandBuffer _commandBuffer, int _index);
		void Resize(VkExtent2D _extent);
		void ReloadShaders();
		void UpdateBuffer(const int _index);
		bool HasNothingToDraw() const { return m_vertices.empty(); }
		void Clear() { m_vertices.clear(); }
		void DebugLine(glm::vec3 start, glm::vec3 end, glm::vec4 color);

		Uniforms	GetUniforms() const { return m_uniforms; }
		void		SetUniforms(const Uniforms _uniforms);

		VkPipeline		GetPipeline() { return m_pipeline; }

	private:
		struct Vertex
		{
			glm::vec3 pos;
			glm::vec3 color;

			Vertex( glm::vec3 _pos, glm::vec3 _color );
			static std::vector <VkVertexInputBindingDescription>	GetBindingDescription();
			static std::vector<VkVertexInputAttributeDescription>	GetAttributeDescriptions();

		};

		Device& m_device;
		VkRenderPass& m_renderPass;

		VkPipelineLayout	m_pipelineLayout;
		VkPipeline			m_pipeline;

		VkDescriptorSetLayout	m_descriptorSetLayout;
		VkDescriptorPool		m_descriptorPool;
		VkDescriptorSet			m_descriptorSet;

		Shader * m_fragmentShader = nullptr;
		Shader * m_vertexShader = nullptr;

		Buffer * m_uniformBuffer;
		std::vector<Buffer *> m_vertexBuffers;

		std::vector<DebugPipeline::Vertex> m_vertices;

		Uniforms m_uniforms;

		void CreateShaders();
		bool CreateDescriptors();
		bool CreatePipeline(		VkExtent2D _extent );		

		void DeletePipeline();
		void DeleteDescriptors();

		
	};
}
