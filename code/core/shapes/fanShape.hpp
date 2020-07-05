#pragma once

#include "bullet/LinearMath/btVector3.h"

namespace fan
{
	//================================================================================================================================
	// base class for shapes (  used for raycasting )
	//================================================================================================================================
	class Shape
	{
	public:
		virtual bool RayCast( const btVector3 _origin, const btVector3 _direction, btVector3& _outIntersection ) const = 0;
	};
}