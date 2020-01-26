#include "render/util/fanVertex.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	std::vector <VkVertexInputBindingDescription> Vertex::GetBindingDescription()
	{
		std::vector <VkVertexInputBindingDescription> bindingDescription(1);

		bindingDescription[0].binding = 0;								// Index of the binding in the array of bindings
		bindingDescription[0].stride = sizeof(Vertex);					// Number of bytes from one entry to the next
		bindingDescription[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	//================================================================================================================================
	//================================================================================================================================
	std::vector<VkVertexInputAttributeDescription> Vertex::GetAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(4);

		// Position
		attributeDescriptions[0].binding = 0;							// Tells Vulkan from which binding the per-vertex data comes
		attributeDescriptions[0].location = 0;							// References the location directive of the input in the vertex shader
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;	// Describes the type of data for the attribute
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		// Normal
		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, normal);

		// Color
		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, color);

		// uvs
		attributeDescriptions[3].binding = 0;
		attributeDescriptions[3].location = 3;
		attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[3].offset = offsetof(Vertex, uv);

		return attributeDescriptions;
	}

	//================================================================================================================================
	// DebugVertex
	//================================================================================================================================
	DebugVertex::DebugVertex(const glm::vec3 _pos, const glm::vec3 _normal, const glm::vec4 _color) :
		pos(_pos)
		, normal(_normal)
		, color(_color)
	{

	}

	//================================================================================================================================
	//================================================================================================================================
	std::vector <VkVertexInputBindingDescription> DebugVertex::GetBindingDescription()
	{
		std::vector <VkVertexInputBindingDescription> bindingDescription(1);

		bindingDescription[0].binding = 0;								// Index of the binding in the array of bindings
		bindingDescription[0].stride = sizeof(DebugVertex);					// Number of bytes from one entry to the next
		bindingDescription[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	//================================================================================================================================
	//================================================================================================================================
	std::vector<VkVertexInputAttributeDescription> DebugVertex::GetAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(3);

		// Position
		attributeDescriptions[0].binding = 0;							// Tells Vulkan from which binding the per-vertex data comes
		attributeDescriptions[0].location = 0;							// References the location directive of the input in the vertex shader
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;	// Describes the type of data for the attribute
		attributeDescriptions[0].offset = offsetof(DebugVertex, pos);

		// Normal
		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(DebugVertex, normal);

		// Color
		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(DebugVertex, color);

		return attributeDescriptions;
	}

	//================================================================================================================================
	// UIVertex
	//================================================================================================================================
	UIVertex::UIVertex( const glm::vec2 _pos, const glm::vec2 _uv, const glm::vec4 _color ) 
		: pos( _pos )
		, uv( _uv )
		, color( _color )
	{
	}

	//================================================================================================================================
	//================================================================================================================================
	std::vector <VkVertexInputBindingDescription> UIVertex::GetBindingDescription()
	{
		std::vector <VkVertexInputBindingDescription> bindingDescription( 1 );

		bindingDescription[0].binding = 0;
		bindingDescription[0].stride = sizeof( UIVertex );
		bindingDescription[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	//================================================================================================================================
	//================================================================================================================================
	std::vector<VkVertexInputAttributeDescription> UIVertex::GetAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions( 3 );

		// Position
		attributeDescriptions[0].binding = 0;						
		attributeDescriptions[0].location = 0;						
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;	
		attributeDescriptions[0].offset = offsetof( UIVertex, pos );

		// UV
		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[1].offset = offsetof( UIVertex, uv );

		// Color
		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attributeDescriptions[2].offset = offsetof( UIVertex, color );

		return attributeDescriptions;
	}
}