#pragma once

namespace vk {
	struct Vertex
	{
		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec3 color;
		glm::vec2 texCoord;

		static std::vector <VkVertexInputBindingDescription>	GetBindingDescription();
		static std::vector<VkVertexInputAttributeDescription>	GetAttributeDescriptions();
	};

	struct DebugVertex
	{
		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec4 color;

		DebugVertex( const glm::vec3 _pos, const glm::vec3 _normal, const glm::vec4 _color );
		static std::vector <VkVertexInputBindingDescription>	GetBindingDescription();
		static std::vector<VkVertexInputAttributeDescription>	GetAttributeDescriptions();

	};
}