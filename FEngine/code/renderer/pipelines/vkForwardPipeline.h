#pragma once

#include "core/memory/fanAlignedMemory.h"

namespace ressource { class Mesh; }
namespace scene { 
	class Model;  
	class Transform;
	class Material;
}
namespace vk {
	struct Vertex;
	struct MeshData;
	class Device;
	class Shader;
	class Image;
	class ImageView;
	class Buffer;
	class Texture;
	class Sampler;

	//================================================================================================================================
	//================================================================================================================================
	struct DrawData {
		MeshData *			meshData	= nullptr;
		scene::Model *		model		= nullptr;
		scene::Transform *	transform	= nullptr;
		scene::Material *	material	= nullptr;
	};

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
			glm::vec3	cameraPosition;
			float		ambiantIntensity;
			glm::vec3	lightColor;
			glm::int32	specularHardness;
			glm::vec3	lightPos;			
		};

		struct DynamicUniformsFrag	{
			glm::int32	textureIndex;
		};

		struct DynamicUniformsVert
		{
			glm::mat4 modelMat;
			glm::mat4 rotationMat;
		};

		ForwardPipeline(Device& _device, VkRenderPass& _renderPass);
		~ForwardPipeline();

		void Create( VkExtent2D _extent );
		void Draw(VkCommandBuffer _commandBuffer, const std::vector< DrawData >& _drawData );
		void Resize(VkExtent2D _extent);
		void ReloadShaders();

		VertUniforms	GetVertUniforms() const { return m_vertUniforms; }
		void			SetVertUniforms(const VertUniforms _uniforms);
		FragUniforms	GetFragUniforms() const { return m_fragUniforms; }
		void			SetFragUniforms(const FragUniforms _fragUniforms);

// 		void			SetDynamicUniformsVert(	const std::vector<DynamicUniformsVert> & _dynamicUniforms );
// 		void			SetDynamicUniformsFrag(	const std::vector<DynamicUniformsFrag> & _dynamicUniforms );
		void	SetDynamicUniformVert( const DynamicUniformsVert& _dynamicUniform, const uint32_t _index);
		void	SetDynamicUniformFrag( const DynamicUniformsFrag& _dynamicUniform, const uint32_t _index);
		void	UpdateDynamicUniformVert();
		void	UpdateDynamicUniformFrag();

		VkPipeline		GetPipeline() { return m_pipeline; }
		VkImageView		GetDepthImageView();

		static const uint32_t s_maximumNumModels = 128;

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

		VertUniforms m_vertUniforms;
		FragUniforms m_fragUniforms;

		Buffer *	m_dynamicUniformBufferVert;
		Buffer *	m_dynamicUniformBufferFrag;

		util::AlignedMemory<DynamicUniformsVert> m_dynamicUniformsVert;
		util::AlignedMemory<DynamicUniformsFrag> m_dynamicUniformsFrag;

		size_t m_dynamicAlignmentVert;
		size_t m_dynamicAlignmentFrag;

		void CreateShaders();
		bool CreateDescriptors();
		bool CreateDescriptorsScene();
		bool CreateDescriptorsTextures();
		bool CreateDepthRessources(	VkExtent2D _extent);
		bool CreatePipeline(		VkExtent2D _extent );

		void DeleteDepthRessources();
		void DeletePipeline();
		void DeleteDescriptors();

	};
}
