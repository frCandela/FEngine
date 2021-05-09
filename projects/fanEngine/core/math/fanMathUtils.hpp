#pragma once

#include "glfw/glfw3.h"
#include "fanDisableWarnings.hpp"
#include "fanGlm.hpp"

namespace fan
{
    struct Vector3;
    struct Quaternion;

    struct Math
    {
        static glm::vec3 ToGLM( const Vector3& _vec );
        static glm::quat ToGLM( const Quaternion& _quat );
        static Vector3 ToFixed( const glm::vec3& _vec );
        static Quaternion ToFixed( const glm::quat& _quat );
    };

    inline glm::ivec2 ToGLM( const VkExtent2D _extent )
    {
        return glm::ivec2( _extent.width, _extent.height );
    }
}