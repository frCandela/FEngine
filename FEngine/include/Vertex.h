#pragma once

#include "glm/glm.hpp"
#include <array>

struct Vertex 
{
	glm::vec2 pos;
	glm::vec3 color;

	//Tells Vulkan how to pass this data format to the vertex shader
	static VkVertexInputBindingDescription getBindingDescription() 
	{
		VkVertexInputBindingDescription bindingDescription = {};

		bindingDescription.binding = 0;								// index of the binding in the array of bindings
		bindingDescription.stride = sizeof(Vertex);					// number of bytes from one entry to the next
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescription;
	}

	//Describes how to handle vertex input
	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() 
	{
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};

		//Position
		attributeDescriptions[0].binding = 0;						// tells Vulkan from which binding the per-vertex data comes
		attributeDescriptions[0].location = 0;						// references the location directive of the input in the vertex shader
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;	//describes the type of data for the attribute
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		//Color
		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		return attributeDescriptions;
	}
};