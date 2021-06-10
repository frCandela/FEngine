#include "render/fanVertex.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    std::vector<VkVertexInputBindingDescription> Vertex::GetBindingDescription()
    {
        std::vector<VkVertexInputBindingDescription> bindingDescription( 1 );

        bindingDescription[0].binding   = 0; // Index of the binding in the array
        bindingDescription[0].stride    = sizeof( Vertex );// Number of bytes from one entry to the next
        bindingDescription[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    std::vector<VkVertexInputAttributeDescription> Vertex::GetAttributeDescriptions()
    {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions( 4 );

        // Position
        attributeDescriptions[0].binding  = 0;  // Tells Vulkan from which binding the per-vertex data comes
        attributeDescriptions[0].location = 0; // References the location of the input in the vertex shader
        attributeDescriptions[0].format   = VK_FORMAT_R32G32B32_SFLOAT;    // type of data for the attribute
        attributeDescriptions[0].offset   = offsetof( Vertex, mPos );

        // Normal
        attributeDescriptions[1].binding  = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format   = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset   = offsetof( Vertex, mNormal );

        // Color
        attributeDescriptions[2].binding  = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format   = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[2].offset   = offsetof( Vertex, mColor );

        // uvs
        attributeDescriptions[3].binding  = 0;
        attributeDescriptions[3].location = 3;
        attributeDescriptions[3].format   = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[3].offset   = offsetof( Vertex, mUv );

        return attributeDescriptions;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    std::vector<VkVertexInputBindingDescription> VertexSkinned::GetBindingDescription()
    {
        std::vector<VkVertexInputBindingDescription> bindingDescription( 1 );

        bindingDescription[0].binding   = 0; // Index of the binding in the array
        bindingDescription[0].stride    = sizeof( VertexSkinned );// Number of bytes from one entry to the next
        bindingDescription[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    std::vector<VkVertexInputAttributeDescription> VertexSkinned::GetAttributeDescriptions()
    {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions( 6 );

        // Position
        attributeDescriptions[0].binding  = 0;  // Tells Vulkan from which binding the per-vertex data comes
        attributeDescriptions[0].location = 0; // References the location of the input in the vertex shader
        attributeDescriptions[0].format   = VK_FORMAT_R32G32B32_SFLOAT;    // type of data for the attribute
        attributeDescriptions[0].offset   = offsetof( VertexSkinned, mPos );

        // Normal
        attributeDescriptions[1].binding  = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format   = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset   = offsetof( VertexSkinned, mNormal );

        // Color
        attributeDescriptions[2].binding  = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format   = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[2].offset   = offsetof( VertexSkinned, mColor );

        // uvs
        attributeDescriptions[3].binding  = 0;
        attributeDescriptions[3].location = 3;
        attributeDescriptions[3].format   = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[3].offset   = offsetof( VertexSkinned, mUv );

        // Bone IDs
        attributeDescriptions[4].binding  = 0;
        attributeDescriptions[4].location = 4;
        attributeDescriptions[4].format   = VK_FORMAT_R8G8B8A8_SINT;
        attributeDescriptions[4].offset   = offsetof( VertexSkinned, mBoneIDs );

        // Bone weights
        attributeDescriptions[5].binding  = 0;
        attributeDescriptions[5].location = 5;
        attributeDescriptions[5].format   = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescriptions[5].offset   = offsetof( VertexSkinned, mBoneWeights );

        return attributeDescriptions;
    }

    //==================================================================================================================================================================================================
    // DebugVertex
    //==================================================================================================================================================================================================
    DebugVertex::DebugVertex( const glm::vec3 _pos, const glm::vec3 _normal, const glm::vec4 _color ) :
            mPos( _pos ),
            mNormal( _normal ),
            mColor( _color ) {}

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    std::vector<VkVertexInputBindingDescription> DebugVertex::GetBindingDescription()
    {
        std::vector<VkVertexInputBindingDescription> bindingDescription( 1 );

        bindingDescription[0].binding   = 0;
        bindingDescription[0].stride    = sizeof( DebugVertex );
        bindingDescription[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    std::vector<VkVertexInputAttributeDescription> DebugVertex::GetAttributeDescriptions()
    {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions( 3 );

        // Position
        attributeDescriptions[0].binding  = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format   = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset   = offsetof( DebugVertex, mPos );

        // Normal
        attributeDescriptions[1].binding  = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format   = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset   = offsetof( DebugVertex, mNormal );

        // Color
        attributeDescriptions[2].binding  = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format   = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescriptions[2].offset   = offsetof( DebugVertex, mColor );

        return attributeDescriptions;
    }

    //==================================================================================================================================================================================================
    // DebugVertex
    //==================================================================================================================================================================================================
    DebugLineVertex::DebugLineVertex( const glm::vec3 _pos, const glm::vec4 _color ) : mPos( _pos ), mColor( _color ) {}

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    std::vector<VkVertexInputBindingDescription> DebugLineVertex::GetBindingDescription()
    {
        std::vector<VkVertexInputBindingDescription> bindingDescription( 1 );

        bindingDescription[0].binding   = 0;
        bindingDescription[0].stride    = sizeof( DebugLineVertex );
        bindingDescription[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    std::vector<VkVertexInputAttributeDescription> DebugLineVertex::GetAttributeDescriptions()
    {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions( 2 );

        // Position
        attributeDescriptions[0].binding  = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format   = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset   = offsetof( DebugLineVertex, mPos );
        // Color
        attributeDescriptions[1].binding  = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format   = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescriptions[1].offset   = offsetof( DebugLineVertex, mColor );

        return attributeDescriptions;
    }

    //==================================================================================================================================================================================================
    // DebugVertex2D
    //==================================================================================================================================================================================================
    DebugVertex2D::DebugVertex2D( const glm::vec2 _pos, const glm::vec4 _color ) :
            mPos( _pos ),
            mColor( _color ) {}

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    std::vector<VkVertexInputBindingDescription> DebugVertex2D::GetBindingDescription()
    {
        std::vector<VkVertexInputBindingDescription> bindingDescription( 1 );
        bindingDescription[0].binding   = 0;
        bindingDescription[0].stride    = sizeof( DebugVertex2D );
        bindingDescription[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    std::vector<VkVertexInputAttributeDescription> DebugVertex2D::GetAttributeDescriptions()
    {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions( 2 );

        // Position
        attributeDescriptions[0].binding  = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format   = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset   = offsetof( DebugVertex2D, mPos );

        // Color
        attributeDescriptions[1].binding  = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format   = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescriptions[1].offset   = offsetof( DebugVertex2D, mColor );

        return attributeDescriptions;
    }

    //==================================================================================================================================================================================================
    // UIVertex
    //==================================================================================================================================================================================================
    UIVertex::UIVertex( const glm::vec2 _pos, const glm::vec2 _uv, const glm::vec4 _color ) :
            mPos( _pos ),
            mUv( _uv ),
            mColor( _color ) {}

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    std::vector<VkVertexInputBindingDescription> UIVertex::GetBindingDescription()
    {
        std::vector<VkVertexInputBindingDescription> bindingDescription( 1 );

        bindingDescription[0].binding   = 0;
        bindingDescription[0].stride    = sizeof( UIVertex );
        bindingDescription[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    std::vector<VkVertexInputAttributeDescription> UIVertex::GetAttributeDescriptions()
    {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions( 3 );

        // Position
        attributeDescriptions[0].binding  = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format   = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset   = offsetof( UIVertex, mPos );

        // UV
        attributeDescriptions[1].binding  = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format   = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[1].offset   = offsetof( UIVertex, mUv );

        // Color
        attributeDescriptions[2].binding  = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format   = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescriptions[2].offset   = offsetof( UIVertex, mColor );

        return attributeDescriptions;
    }
}