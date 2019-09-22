#pragma once


namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	struct Vertex
	{
		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec3 color;
		glm::vec2 uv;

		static std::vector <VkVertexInputBindingDescription>	GetBindingDescription();
		static std::vector<VkVertexInputAttributeDescription>	GetAttributeDescriptions();

		bool operator==(const Vertex& _other) const {
			return pos == _other.pos && normal == _other.normal && normal == _other.normal && uv == _other.uv;
		}
	};

	//================================================================================================================================
	//================================================================================================================================
	struct DebugVertex
	{
		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec4 color;

		DebugVertex(const glm::vec3 _pos, const glm::vec3 _normal, const glm::vec4 _color);
		static std::vector <VkVertexInputBindingDescription>	GetBindingDescription();
		static std::vector<VkVertexInputAttributeDescription>	GetAttributeDescriptions();
	};
}

namespace std {
	template<> struct hash<fan::Vertex> {
		size_t operator()(fan::Vertex const& vertex) const {
			return ((hash<glm::vec3>()(vertex.pos) ^
				(hash<glm::vec3>()(vertex.normal) << 1)) >> 1) ^
				(hash<glm::vec2>()(vertex.color) << 1) ^
				(hash<glm::vec2>()(vertex.uv) << 1);
		}
	};
}