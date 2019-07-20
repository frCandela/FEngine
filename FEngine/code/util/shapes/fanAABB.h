#pragma once

#include "util/shapes/fanTriangle.h"

namespace shape
{
	class Plane;

	class AABB : public Shape {
	public:
		AABB() : AABB( btVector3::Zero(), btVector3::Zero() ){}
		AABB(const btVector3 _low , const btVector3 _high);

		btVector3 GetLow() const { return m_low; }
		btVector3 GetHigh() const { return m_high; }
		std::vector< btVector3 >	GetCorners() const;

		virtual bool RayCast(const btVector3 _origin, const btVector3 _direction, btVector3& outIntersection) const override;

	private:
		btVector3 m_low;
		btVector3 m_high;
	};
}
