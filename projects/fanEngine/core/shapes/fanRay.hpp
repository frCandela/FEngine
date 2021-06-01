#pragma once

#include "core/math/fanVector3.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    // straight line in a 3D space (origin + direction)
    //==================================================================================================================================================================================================
    struct Ray
    {
        Ray();
        Ray( const Vector3& _origin, const Vector3& _direction );

        void RayClosestPoints( const Ray _ray, Vector3& _outIntersectionRay, Vector3& _outIntersectionOtherRay ) const;

        Vector3 origin;
        Vector3 direction;

        bool operator==( const Ray& _other ) const
        {
            return origin == _other.origin && direction == _other.direction;
        }
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct RaycastResult
    {
        Fixed   mDistance;
        Vector3 mPosition;
        Vector3 mNormal;
    };
}

