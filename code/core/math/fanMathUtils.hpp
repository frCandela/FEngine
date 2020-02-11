#pragma once

#include "core/fanCorePrecompiled.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
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

	const float PI = SIMD_PI;

	inline btVector3 btDegrees3( const  btVector3 _radians ) { return btVector3( btDegrees( _radians[ 0 ] ), btDegrees( _radians[ 1 ] ), btDegrees( _radians[ 2 ] ) ); }
	inline btVector3 btRadians3( const  btVector3 _degrees ) { return btVector3( btRadians( _degrees[ 0 ] ), btRadians( _degrees[ 1 ] ), btRadians( _degrees[ 2 ] ) ); }
}