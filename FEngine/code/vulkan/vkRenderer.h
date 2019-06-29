#pragma once

namespace vk {
	struct Vertex;
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
	class PostprocessPipeline;

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
		Device &		m_device;
		SwapChain  *	m_swapchain;
		ImguiPipeline * m_imguiPipeline;
		PostprocessPipeline * m_postprocessPipeline;

		VkCommandPool	m_commandPool;

		VkRenderPass	m_renderPass;
		VkRenderPass	m_renderPassPostprocess;

		VkPipelineLayout	m_pipelineLayout;
		VkPipeline			m_pipeline;



		VkDescriptorSetLayout	m_descriptorSetLayout;
		VkDescriptorPool		m_descriptorPool;
		VkDescriptorSet			m_descriptorSet;



		std::vector<VkCommandBuffer> m_primaryCommandBuffers;
		std::vector<VkCommandBuffer> m_geometryCommandBuffers;
		std::vector<VkCommandBuffer> m_imguiCommandBuffers;
		std::vector<VkCommandBuffer> m_postprocessCommandBuffers;

		std::vector< FrameBuffer * > m_forwardFrameBuffers;
		std::vector< FrameBuffer * > m_swapchainFramebuffers;

		Image *			m_depthImage;
		ImageView  *	m_depthImageView;

		Shader * m_fragmentShader = nullptr;
		Shader * m_vertexShader = nullptr;

		Buffer * m_uniformBuffer;
		Buffer * m_indexBuffer;
		Buffer * m_vertexBuffer;


		std::vector<Vertex>		m_vertices;
		std::vector<uint32_t>	m_indices;


		glm::vec4 m_clearColor;

		struct UniformsForward
		{
			glm::mat4 model;
			glm::mat4 view;
			glm::mat4 proj;
		} m_uniformsForward;

		static Renderer * ms_globalRenderer;
		
		bool ResetCommandPool();
		void UpdateUniformBuffer();		
		bool SubmitCommandBuffers();
		void ReloadShaders();

		void RecordCommandBufferPostProcess	( const int _index);
		void RecordCommandBufferImgui		( const int _index);
		void RecordCommandBufferGeometry	( const int _index);		
		void RecordPrimaryCommandBuffer		( const int _index);
		void RecordAllCommandBuffers();

		void CreateShaders();
		bool CreateDescriptors();
		bool CreateCommandBuffers();
		bool CreateCommandPool();
		bool CreateRenderPass();
		bool CreateRenderPassPostprocess();
		bool CreateDepthRessources();
		bool CreatePipeline();
		void CreateVertexBuffers();
		void CreateFramebuffers();
		void CreateSwapchainFramebuffers();

		void DeleteCommandPool();
		void DeleteRenderPass();
		void DeleteRenderPassPostprocess();
		void DeleteDepthRessources();
		void DeleteFramebuffers();
		void DeletePipeline();
		void DeleteDescriptors();
		void DeleteSwapchainFramebuffers();

	};
}