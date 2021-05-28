#pragma once

#include <vector>
#include "glfw/glfw3.h"
#include "fanDisableWarnings.hpp"
#include "fanGlm.hpp"
#include "core/fanColor.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    // vulkan vertex definitions
    //==================================================================================================================================================================================================
    struct Vertex
    {
        glm::vec3 mPos;
        glm::vec3 mNormal;
        glm::vec3 mColor;
        glm::vec2 mUv;

        static std::vector<VkVertexInputBindingDescription> GetBindingDescription();
        static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();

        bool operator==( const Vertex& _other ) const
        {
            return mPos == _other.mPos &&
                   mNormal == _other.mNormal &&
                   mColor == _other.mColor && mUv == _other.mUv;
        }
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct DebugVertex
    {
        glm::vec3 mPos;
        glm::vec3 mNormal;
        glm::vec4 mColor;
        DebugVertex() {}
        DebugVertex( const glm::vec3 _pos, const glm::vec3 _normal, const glm::vec4 _color );
        static std::vector<VkVertexInputBindingDescription> GetBindingDescription();
        static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct DebugLineVertex
    {
        glm::vec3 mPos;
        glm::vec4 mColor;
        DebugLineVertex() {}
        DebugLineVertex( const glm::vec3 _pos, const glm::vec4 _color );
        static std::vector<VkVertexInputBindingDescription> GetBindingDescription();
        static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct DebugVertex2D
    {
        glm::vec2 mPos;
        glm::vec4 mColor;

        DebugVertex2D( const glm::vec2 _pos = glm::vec2( 0.f, 0.f ),
                       const glm::vec4 _color = Color::sWhite.ToGLM() );
        static std::vector<VkVertexInputBindingDescription> GetBindingDescription();
        static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct UIVertex
    {
        glm::vec2 mPos;
        glm::vec2 mUv;
        glm::vec4 mColor;

        UIVertex( const glm::vec2 _pos = glm::vec2( 0.f, 0.f ),
                  const glm::vec2 _uv = glm::vec2( 0.f, 0.f ),
                  const glm::vec4 _color = Color::sWhite.ToGLM() );
        static std::vector<VkVertexInputBindingDescription> GetBindingDescription();
        static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
    };
}

namespace std
{
    template<> struct hash<fan::Vertex>
    {
        size_t operator()( fan::Vertex const& vertex ) const
        {
            return ( ( hash<glm::vec3>()( vertex.mPos ) ^
                       ( hash<glm::vec3>()( vertex.mNormal ) << 1 ) ) >> 1 ) ^
                   ( hash<glm::vec2>()( vertex.mColor ) << 1 ) ^
                   ( hash<glm::vec2>()( vertex.mUv ) << 1 );
        }
    };
}