#pragma once

#include "core/fanCorePrecompiled.hpp"

namespace fan
{
	inline std::vector< btVector3 > GetCube( const btVector3 _halfExtent );
	inline std::vector< btVector3 > GetSphere( const float _radius, const int _numSubdivisions );
	inline std::vector< btVector3 > GetCone( const float _radius, const float _height, const int _numSubdivisions );
}

#include "core/math/fanBasicModels.inl"