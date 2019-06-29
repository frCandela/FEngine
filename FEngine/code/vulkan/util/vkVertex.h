#pragma once

namespace vk {
	struct Vertex
	{
		glm::vec3 pos;
		glm::vec3 color;
		glm::vec2 texCoord;

		static std::vector <VkVertexInputBindingDescription>	GetBindingDescription();
		static std::vector<VkVertexInputAttributeDescription>	GetAttributeDescriptions();
	};
}