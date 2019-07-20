#pragma once

#include "util/shapes/fanShape.h"

namespace shape
{
	class Triangle : public Shape
	{
	public:
		Triangle() : m_v0(0,0,0), m_v1(0, 0, 0), m_v2(0, 0, 0) {}
		Triangle(const btVector3 _v0, const btVector3 _v1, const btVector3 _v2);

		btVector3 GetCenter() const;
		btVector3 GetNormal() const;

		virtual bool RayCast(const btVector3 _origin, const btVector3 _dir, btVector3& _outIntersection) const override;
	private:
		btVector3 m_v0;
		btVector3 m_v1;
		btVector3 m_v2;
	};
}
