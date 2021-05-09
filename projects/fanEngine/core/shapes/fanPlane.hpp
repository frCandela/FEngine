#pragma once

#include <LinearMath/btVector3.h>

namespace fan
{
	//========================================================================================================
	// 3D plane
	// @todo better use point+distance
	//========================================================================================================
	struct Plane
	{
	public:
		Plane( const btVector3 _point, const btVector3 _normal );
        bool RayCast( const btVector3 _origin, const btVector3 _dir, btVector3& _outIntersection ) const;

		btVector3 mPoint;
		btVector3 mNormal;
	};
}
