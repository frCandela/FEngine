#pragma once

#include <vector>
#include "core/math/fanVector3.hpp"

namespace fan
{
    //========================================================================================================
    // spits out list of triangles for making different shapes
    //========================================================================================================
    inline std::vector<Vector3> GetCube( const Vector3 _halfExtent );
    inline std::vector<Vector3> GetSphere( const Fixed _radius, const int _numSubdivisions );
    inline std::vector<Vector3> GetCone( const Fixed _radius, const Fixed _height, const int _numSubdivisions );
}

#include "core/math/fanBasicModels.inl"