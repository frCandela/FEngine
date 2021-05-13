#pragma once

#include "core/shapes/fanTriangle.hpp"
#include <array>

namespace fan
{
    //==================================================================================================================================================================================================
    // @todo remake this properly without using triangles and name it "Box"
    //==================================================================================================================================================================================================
    struct Cube
    {
        Cube( const Vector3 _position, const Fixed _halfSize );

        void SetPosition( const Vector3 _pos );

        const std::array<Triangle, 12>& GetTriangles() const { return mTriangles; }
        Fixed GetHalfSize() const { return mHalfSize; }
        Vector3 GetPosition() const { return mPosition; }

        bool RayCast( const Vector3 _origin, const Vector3 _direction, Vector3& outIntersection ) const;

        std::array<Triangle, 12> mTriangles;
        Vector3                  mPosition;
        Fixed                    mHalfSize;

    private:
        void InitCube();
    };
}
