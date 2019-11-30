#pragma once

namespace fan
{
	class Window;
	class Mesh;
	class UIMesh;
	class Instance;
	class Device;
	class SwapChain;
	class ImguiPipeline;
	class FrameBuffer;
	class Image;
	class ImageView;
	class Shader;
	class Buffer;
	class Sampler;
	class DescriptorTextures;
	class DescriptorSampler;
	class PostprocessPipeline;
	class ForwardPipeline;
	class UIPipeline;
	class Color;
	class RessourceManager;
	class RendererDebug;

	// Used to set uniforms
	struct DrawMesh {
		Mesh * mesh;
		glm::mat4 modelMatrix;
		glm::mat4 normalMatrix;
		glm::vec4 color;
		uint32_t shininess;
		uint32_t textureIndex;
	};

	struct DrawUIMesh {
		UIMesh * mesh;
		glm::vec2 position;
		glm::vec2 scale;
		glm::vec4 color;
		uint32_t textureIndex;
	};

	// Used to batch rendering
	struct DrawData
	{
		Mesh * mesh;
		uint32_t textureIndex;
	};

	// Used to batch rendering
	struct UIDrawData
	{
		UIMesh * mesh;
		uint32_t textureIndex;
	};

	//================================================================================================================================
	//================================================================================================================================
	class Renderer {
	private:


	public:
		Renderer(const VkExtent2D _size, const glm::ivec2 _position);
		~Renderer();

		bool WindowIsOpen();
		void DrawFrame();
		void WaitIdle();

		void ReloadIcons();
		void ReloadShaders();

		Window *				GetWindow() { return m_window; }
		RendererDebug&			GetRendererDebug() { return *m_rendererDebug; }
		ImguiPipeline *			GetImguiPipeline() { return m_imguiPipeline; }
		PostprocessPipeline *	GetPostprocessPipeline() { return m_postprocessPipeline; }
		ForwardPipeline *		GetForwardPipeline() { return m_forwardPipeline; }
		glm::vec4				GetClearColor() const { return m_clearColor; }

		void SetClearColor(glm::vec4 _color) { m_clearColor = _color; }
		void SetMainCamera( const glm::mat4 _projection, const glm::mat4 _view, const glm::vec3 _position );
		void SetDirectionalLight( const int _index, const glm::vec4 _direction, const glm::vec4 _ambiant, const glm::vec4 _diffuse, const glm::vec4 _specular );
		void SetNumDirectionalLights( const uint32_t _num );
		void SetPointLight( const int _index, const glm::vec3 _position, const glm::vec3 _diffuse, const glm::vec3 _specular, const glm::vec3 _ambiant, const glm::vec3 _constantLinearQuadratic );
		void SetNumPointLights( const uint32_t _num );
		void SetDrawData( const std::vector<DrawMesh> & _drawData );
		void SetUIDrawData( const std::vector<DrawUIMesh> & _drawData );

		float GetWindowAspectRatio() const;
		void Clear() { m_meshDrawArray.clear(); }

		const std::vector< DrawData > & GetMeshArray() const { return m_meshDrawArray; }
	private:
		RendererDebug * m_rendererDebug;

		std::vector< DrawData > m_meshDrawArray;
		std::vector< UIDrawData > m_uiMeshDrawArray;
			   
		// VULKAN OBJECTS
		Instance *		m_instance;
		Window *		m_window;
		Device *		m_device;
		SwapChain  *	m_swapchain;

		ImguiPipeline *			m_imguiPipeline;
		PostprocessPipeline *	m_postprocessPipeline;
		ForwardPipeline *		m_forwardPipeline;
		UIPipeline *			m_uiPipeline;

		DescriptorTextures * m_imagesDescriptor = nullptr;

		DescriptorSampler * m_samplerDescriptorTextures	= nullptr;
		Sampler *			m_samplerTextures			= nullptr;

		DescriptorSampler * m_samplerDescriptorUI	= nullptr;
		Sampler *			m_samplerUI				= nullptr;

		VkRenderPass	m_gameRenderPass;
		VkRenderPass	m_renderPassPostprocess;
		VkRenderPass	m_renderPassImgui;

		std::vector<VkCommandBuffer> m_primaryCommandBuffers;
		std::vector<VkCommandBuffer> m_geometryCommandBuffers;
		std::vector<VkCommandBuffer> m_imguiCommandBuffers;
		std::vector<VkCommandBuffer> m_uiCommandBuffers;
		std::vector<VkCommandBuffer> m_postprocessCommandBuffers;


		FrameBuffer * m_gameFrameBuffers;
		FrameBuffer * m_postProcessFramebuffers;
		FrameBuffer * m_swapchainFramebuffers;

		Buffer * m_quadVertexBuffer;
		glm::vec4 m_clearColor;

		void UpdateUniformBuffers( const size_t _index );
		
		bool SubmitCommandBuffers();
		void CreateQuadVertexBuffer();

		void RecordCommandBufferPostProcess(const size_t _index);
		void RecordCommandBufferImgui(const size_t _index);
		void RecordCommandBufferUI(const size_t _index);		
		void RecordCommandBufferGeometry(const size_t _index);
		void RecordPrimaryCommandBuffer(const size_t _index);
		void RecordAllCommandBuffers();

		bool CreateCommandBuffers();
		void CreateGameFramebuffers();
		void CreatePostProcessFramebuffers();		
		void CreateSwapchainFramebuffers();

		bool CreateRenderPass();
		bool CreateRenderPassPostprocess();
		bool CreateRenderPassImGui();

		bool CreateTextureDescriptor();
		void BindTexture( VkCommandBuffer _commandBuffer, const uint32_t _textureIndex, DescriptorSampler* _samplerDescriptor, VkPipelineLayout _pipelineLayout );

		void DeleteRenderPass();
		void DeleteRenderPassPostprocess();
	};
}
