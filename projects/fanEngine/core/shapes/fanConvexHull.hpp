#pragma once

#include <vector>
#include <cstdint>
#include "core/math/fanVector3.hpp"

namespace fan
{

    //==================================================================================================================================================================================================
    // Converts a points cloud to a convex hull
    // ComputeQuickHull is always faster than ComputeBulletHull ( up to x300 times faster ).
    // ComputeBulletHull gives more accurate results with less triangles
    //==================================================================================================================================================================================================
    struct ConvexHull
    {
        void ComputeQuickHull( const std::vector<Vector3>& _pointCloud );
        void Clear();
        bool IsEmpty() const { return mIndices.size() == 0; }
        bool RayCast( const Vector3& _origin, const Vector3& _direction, Vector3& _outIntersection ) const;

        std::vector<Vector3>  mVertices;
        std::vector<uint32_t> mIndices;
    };
}
