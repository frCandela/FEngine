#pragma once

namespace vk {
	class Instance;
	class Window;
	class Device;
	class SwapChain;
	class ImguiPipeline;
	class FrameBuffer;
	class Image;
	class ImageView;
	class Shader;
	class Buffer;
	class Sampler;
	struct Vertex;


	class Renderer {
	public:
		Renderer(const VkExtent2D _size);
		~Renderer();

		void Run();

		VkCommandBuffer BeginSingleTimeCommands();
		void EndSingleTimeCommands(VkCommandBuffer commandBuffer);

		static Renderer * GetGlobalRenderer() {	return ms_globalRenderer; }

	private:
		Instance *		m_instance;
		Window *		m_window;
		Device *		m_device;
		SwapChain  *	m_swapchain;
		ImguiPipeline * m_imguiPipeline;

		VkCommandPool	m_commandPool;

		VkRenderPass	m_renderPass;
		VkRenderPass	m_renderPassPostprocess;

		VkPipelineLayout	m_pipelineLayout;
		VkPipeline			m_pipeline;
		VkPipelineLayout	m_pipelineLayoutPostprocess;
		VkPipeline			m_pipelinePostprocess;


		VkDescriptorSetLayout	m_descriptorSetLayout;
		VkDescriptorPool		m_descriptorPool;
		VkDescriptorSet			m_descriptorSet;

		VkDescriptorSetLayout	m_descriptorSetLayoutPostprocess;
		VkDescriptorPool		m_descriptorPoolPostprocess;
		VkDescriptorSet			m_descriptorSetPostprocess;

		std::vector<VkCommandBuffer> m_primaryCommandBuffers;
		std::vector<VkCommandBuffer> m_geometryCommandBuffers;
		std::vector<VkCommandBuffer> m_imguiCommandBuffers;
		std::vector<VkCommandBuffer> m_postprocessCommandBuffers;

		std::vector< FrameBuffer * > m_forwardFrameBuffers;
		std::vector< FrameBuffer * > m_postProcessFramebuffers;

		Image *			m_depthImage;
		ImageView  *	m_depthImageView;
		Sampler *		m_samplerPostprocess;
		Image *			m_imagePostprocess;
		ImageView *		m_imageViewPostprocess;

		Shader * m_fragmentShader = nullptr;
		Shader * m_vertexShader = nullptr;
		Shader * m_fragmentShaderPostprocess = nullptr;
		Shader * m_vertexShaderPostprocess = nullptr;

		Buffer * m_uniformBuffer;
		Buffer * m_indexBuffer;
		Buffer * m_vertexBuffer;
		Buffer * m_vertexBufferPostprocess;

		std::vector<Vertex>		m_vertices;
		std::vector<uint32_t>	m_indices;

		struct UniformBufferObject
		{
			glm::mat4 model;
			glm::mat4 view;
			glm::mat4 proj;
		} m_ubo;

		static Renderer * ms_globalRenderer;
		
		bool ResetCommandPool();
		void UpdateUniformBuffer();		
		void RecordAllCommandBuffers();
		void RecordPrimaryCommandBuffer(const int _index);
		void RecordCommandBufferImgui(const int _index);
		void RecordCommandBufferGeometry(const int _index);
		void RecordCommandBufferPostProcess(const int _index);		
		bool SubmitCommandBuffers();
		void ReloadShaders();

		bool CreateShaders();
		bool CreateDescriptors();
		bool CreateDescriptorsPostprocess();
		bool CreateCommandBuffers();
		bool CreateCommandPool();
		bool CreateRenderPass();
		bool CreateRenderPassPostprocess();
		bool CreateDepthRessources();
		void CreateFramebuffers();
		void CreateFramebuffersPostprocess();
		bool CreatePipeline();
		bool CreatePipelinePostprocess();
		void CreateVertexBuffers();
		void CreatePostprocess();
		
		void DeleteCommandPool();
		void DeleteRenderPass();
		void DeleteRenderPassPostprocess();
		void DeleteDepthRessources();
		void DeleteFramebuffers();
		void DeleteFramebuffersPostprocess();
		void DeletePipeline();
		void DeletePipelinePostprocess();
		void DeleteDescriptors();
		void DeleteDescriptorsPostprocess();
	};
}