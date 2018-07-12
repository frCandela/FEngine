#pragma once

#ifndef GLFW_INCLUDE_VULKAN
	#define GLFW_INCLUDE_VULKAN
	#include <GLFW/glfw3.h>
#endif // !GLFW_INCLUDE_VULKAN

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"
#include "glm/glm.hpp"
#include <array>

namespace vk
{
	struct Vertex
	{
		glm::vec3 pos;
		glm::vec3 color;
		glm::vec2 texCoord;

		bool operator==(const Vertex& other) const
		{
			return pos == other.pos && color == other.color && texCoord == other.texCoord;
		}

		// Tells Vulkan how to pass this data format to the vertex shader
		static VkVertexInputBindingDescription getBindingDescription()
		{
			VkVertexInputBindingDescription bindingDescription = {};

			bindingDescription.binding = 0;								// Index of the binding in the array of bindings
			bindingDescription.stride = sizeof(Vertex);					// Number of bytes from one entry to the next
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		// Describes how to handle vertex input
		static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions()
		{
			std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = {};

			// Position
			attributeDescriptions[0].binding = 0;							// Tells Vulkan from which binding the per-vertex data comes
			attributeDescriptions[0].location = 0;							// References the location directive of the input in the vertex shader
			attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;	// Describes the type of data for the attribute
			attributeDescriptions[0].offset = offsetof(Vertex, pos);

			// Color
			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, color);

			// Texture coordinates
			attributeDescriptions[2].binding = 0;
			attributeDescriptions[2].location = 2;
			attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

			return attributeDescriptions;
		}
	};
}

namespace std {
	template<> struct hash<vk::Vertex> {
		size_t operator()(vk::Vertex const& vertex) const {
			return ((hash<glm::vec3>()(vertex.pos) ^
				(hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
				(hash<glm::vec2>()(vertex.texCoord) << 1);
		}
	};
}

