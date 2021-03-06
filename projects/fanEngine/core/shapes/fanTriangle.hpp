#pragma once

#include "core/math/fanVector3.hpp"
#include "core/shapes/fanRay.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    // Triangle shape used for ray casting
    //==================================================================================================================================================================================================
    struct Triangle
    {

        Triangle() : mV0( 0, 0, 0 ), mV1( 0, 0, 0 ), mV2( 0, 0, 0 ) {}
        Triangle( const Vector3 _v0, const Vector3 _v1, const Vector3 _v2 );

        Vector3 GetCenter() const;
        Vector3 GetNormal() const;

        bool RayCast( const Ray _ray, RaycastResult& _outResult ) const;

        Vector3 mV0;
        Vector3 mV1;
        Vector3 mV2;
    };
}