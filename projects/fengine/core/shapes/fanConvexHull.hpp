#pragma once

#include "core/shapes/fanShape.hpp"
#include <vector>
#include <cstdint>

namespace fan
{
	//========================================================================================================
	// Converts a points cloud to a convex hull
	// ComputeQuickHull is always faster than ComputeBulletHull ( up to x300 times faster ).
	// ComputeBulletHull gives more accurate results with less triangles
	//========================================================================================================
	struct ConvexHull : public Shape
	{
		void ComputeBulletHull( const std::vector<btVector3>& _pointCloud );
		void ComputeQuickHull( const std::vector<btVector3>& _pointCloud );
		void Clear();
		bool IsEmpty() const { return mIndices.size() == 0; }
        bool RayCast( const btVector3 _origin,
                      const btVector3 _direction,
                      btVector3& _outIntersection ) const override;

		std::vector<btVector3> mVertices;
		std::vector<uint32_t>  mIndices;
	};
}
