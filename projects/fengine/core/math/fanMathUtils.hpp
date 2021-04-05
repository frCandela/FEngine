#pragma once

#include "glfw/glfw3.h"
#include "LinearMath/btVector3.h"
#include "LinearMath/btQuaternion.h"
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

    static const btVector3 btVector3_Left    = btVector3( 1.f, 0.f, 0.f );
    static const btVector3 btVector3_Up      = btVector3( 0.f, 1.f, 0.f );
    static const btVector3 btVector3_Forward = btVector3( 0.f, 0.f, 1.f );
    static const btVector3 btVector3_Zero    = btVector3( 0.f, 0.f, 0.f );
    static const btVector3 btVector3_One     = btVector3( 1.f, 1.f, 1.f );

    //========================================================================================================
    //========================================================================================================
    inline btVector3 ToBullet( const glm::vec3 _vec ) { return btVector3( _vec.x, _vec.y, _vec.z ); }
    inline btVector3 ToBullet( const glm::vec4 _vec ) { return btVector3( _vec.x, _vec.y, _vec.z ); }
    inline glm::vec3 ToGLM( const btVector3 _vec ) { return glm::vec3( _vec[0], _vec[1], _vec[2] ); }

    inline glm::quat ToGLM( const btQuaternion _quat )
    {
        glm::quat rotation;
        rotation.x = _quat[0];
        rotation.y = _quat[1];
        rotation.z = _quat[2];
        rotation.w = _quat[3];
        return rotation;
    }

    inline glm::ivec2 ToGLM( const VkExtent2D _extent )
    {
        return glm::ivec2( _extent.width, _extent.height );
    }

    inline btVector3 btDegrees3( const btVector3 _radians )
    {
        return btVector3( btDegrees( _radians[0] ), btDegrees( _radians[1] ), btDegrees( _radians[2] ) );
    }
    inline btVector3 btRadians3( const btVector3 _degrees )
    {
        return btVector3( btRadians( _degrees[0] ), btRadians( _degrees[1] ), btRadians( _degrees[2] ) );
    }
}