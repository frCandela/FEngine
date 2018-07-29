#pragma once

#include "ForwardPipeline.h"

#include "vulkan/Device.h"
#include "vulkan/Buffer.hpp"
#include "vulkan/CommandPool.h"

namespace vk
{
	/// Buffer class : encapsulates a vulkan buffer
	class Mesh
	{
	public:
		Mesh(Device& device);
		~Mesh();
		Device & m_device;

		//Vertices and indices of the loaded model
		std::vector<ForwardPipeline::Vertex> vertices;
		std::vector<uint32_t> indices;

		vk::Buffer vertexBuffer;
		vk::Buffer indexBuffer;

		static void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, CommandPool& rCommandPool);

		/// Loads a model from an OBJ file
		void LoadModel(std::string path);
		void CreateBuffers(CommandPool& rCommandPool);

	private:
		/// Creates a vertex buffer from the loaded model data
		void CreateVertexBuffer(CommandPool& rCommandPool);

		/// Create the index buffer from the loaded model data
		void CreateIndexBuffer( CommandPool& rCommandPool);
	};
}