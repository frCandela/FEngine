#pragma once

#include "glfw/glfw3.h"
#include "bullet/LinearMath/btVector3.h"
#include "bullet/LinearMath/btQuaternion.h"
#include "fanDisableWarnings.hpp"
WARNINGS_GLM_PUSH()
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
WARNINGS_POP()

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	inline btVector3 ToBullet( const glm::vec3 _vec ) { return btVector3( _vec.x, _vec.y, _vec.z ); }
	inline btVector3 ToBullet( const glm::vec4 _vec ) { return btVector3( _vec.x, _vec.y, _vec.z ); }
	inline glm::vec3 ToGLM( const btVector3 _vec ) { return glm::vec3( _vec[ 0 ], _vec[ 1 ], _vec[ 2 ] ); }
	inline glm::quat ToGLM( const btQuaternion _quat )
	{
		glm::quat rotation;
		rotation.x = _quat[ 0 ];
		rotation.y = _quat[ 1 ];
		rotation.z = _quat[ 2 ];
		rotation.w = _quat[ 3 ];
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
    inline float SignedAngle( const btVector3& _vector1, const btVector3& _vector2, const btVector3& _axis )
    {
        float angle = _vector1.angle( _vector2 ) *
                      ( _vector1.cross( _vector2 ).dot( _axis ) > 0.0f ? 1.f : -1.f );
        return std::fmodf( angle + SIMD_2_PI, SIMD_2_PI );
    }
}