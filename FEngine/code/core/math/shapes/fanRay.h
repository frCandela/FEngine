#pragma once

namespace fan {
	namespace shape {
		//================================================================================================================================
		//================================================================================================================================
		struct Ray
		{
			btVector3 origin;
			btVector3 direction;

			void RayClosestPoints(const Ray _ray, btVector3& _outIntersectionRay, btVector3& _outIntersectionOtherRay) const;
		};
	}
}
