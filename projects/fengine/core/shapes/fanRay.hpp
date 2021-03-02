#pragma once

#include "LinearMath/btVector3.h"

namespace fan
{
	//========================================================================================================
	// straight line in a 3D space (origin + direction)
	//========================================================================================================
	struct Ray
	{
		Ray();
		Ray( const btVector3 _origin, const btVector3 _direction );

        void RayClosestPoints( const Ray _ray,
                               btVector3& _outIntersectionRay,
                               btVector3& _outIntersectionOtherRay ) const;

		btVector3 origin;
		btVector3 direction;

		bool operator==( const Ray& _other ) const
		{
			return origin == _other.origin && direction == _other.direction;
		}
	};
}

