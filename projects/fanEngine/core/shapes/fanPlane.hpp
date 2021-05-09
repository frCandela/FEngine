#pragma once

#include "core/math/fanVector3.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    // 3D plane
    // @todo better use point+distance
    //==================================================================================================================================================================================================
    struct Plane
    {
    public:
        Plane( const Vector3 _point, const Vector3 _normal );
        bool RayCast( const Vector3& _origin, const Vector3& _dir, Vector3& _outIntersection ) const;

        Vector3 mPoint;
        Vector3 mNormal;
    };
}
