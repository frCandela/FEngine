#pragma once

#include "core/shapes/fanShape.hpp"

namespace fan
{
	//========================================================================================================
	// 3D plane
	// @todo we don't need the m_point, only the distance to the plane when following the normal
	//========================================================================================================
	struct Plane : public Shape
	{
	public:
		Plane( const btVector3 _point, const btVector3 _normal );
        bool RayCast( const btVector3 _origin,
                      const btVector3 _dir,
                      btVector3& _outIntersection ) const override;

		btVector3 mPoint;
		btVector3 mNormal;
	};
}
