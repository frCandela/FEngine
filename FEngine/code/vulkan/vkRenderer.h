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

		VkPipelineLayout	m_pipelineLayout;
		VkPipeline			m_pipeline;

		VkDescriptorSetLayout	m_descriptorSetLayout;
		VkDescriptorPool		m_descriptorPool;
		VkDescriptorSet			m_descriptorSet;

		std::vector<VkCommandBuffer> m_primaryCommandBuffers;
		std::vector<VkCommandBuffer> m_geometryCommandBuffers;
		std::vector<VkCommandBuffer> m_imguiCommandBuffers;

		std::vector< FrameBuffer * > m_swapChainframeBuffers;

		Image *			m_depthImage;
		ImageView  *	m_depthImageView;

		Shader * m_fragmentShader;
		Shader * m_vertexShader;

		Buffer * m_uniformBuffer;
		Buffer * m_indexBuffer;
		Buffer * m_vertexBuffer;

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
		bool SubmitCommandBuffers();
		void ReloadShaders();

		bool CreateDescriptors();
		bool CreateCommandBuffers();
		bool CreateCommandPool();
		bool CreateRenderPass();
		bool CreateDepthRessources();
		void CreateFramebuffers();
		bool CreatePipeline();
		void CreateVertexBuffers();
		
		void DeleteCommandPool();
		void DeleteRenderPass();
		void DeleteDepthRessources();
		void DeleteFramebuffers();
		void DeletePipeline();
		void DeleteDescriptors();
	};
}