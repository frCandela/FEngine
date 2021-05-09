#pragma once

#include <vector>
#include "fanDisableWarnings.hpp"
#include "fanGlm.hpp"
#include "core/shapes/fanTriangle.hpp"
#include "core/math/fanMathUtils.hpp"

namespace fan
{
    struct Matrix4;

    //==================================================================================================================================================================================================
    // axis aligned bounding box
    //==================================================================================================================================================================================================
    struct AABB
    {
        AABB();
        AABB( const Vector3 _low, const Vector3 _high );
        AABB( const std::vector<Vector3>& _pointCloud, const Matrix4& _tranform );

        void Clear()
        {
            mLow  = Vector3::sZero;
            mHigh = Vector3::sZero;
        }
        Vector3 GetLow() const { return mLow; }
        Vector3 GetHigh() const { return mHigh; }
        std::vector<Vector3> GetCorners() const;

        bool RayCast( const Vector3& _origin, const Vector3& _direction, Vector3& outIntersection ) const;

        Vector3 mLow;
        Vector3 mHigh;
    };
}
