#pragma once

namespace fan {
	namespace shape
	{
		//================================================================================================================================
		//================================================================================================================================
		class Shape
		{
		public:
			virtual bool RayCast(const btVector3 _origin, const btVector3 _direction, btVector3& _outIntersection) const = 0;
		};
	}
}

