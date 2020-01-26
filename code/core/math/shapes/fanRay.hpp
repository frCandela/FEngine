#pragma once

#include "core/fanCorePrecompiled.hpp"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	struct Ray
	{
		Ray();
		Ray(const btVector3 _origin, const btVector3 _direction);

		void RayClosestPoints(const Ray _ray, btVector3& _outIntersectionRay, btVector3& _outIntersectionOtherRay) const;

		btVector3 origin;
		btVector3 direction;
	};
}

