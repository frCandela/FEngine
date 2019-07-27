#include "fanIncludes.h"

#include "core/math/shapes/fanTriangle.h"

namespace shape
{
	//================================================================================================================================
	//================================================================================================================================
	Triangle::Triangle(const btVector3 _v0, const btVector3 _v1, const btVector3 _v2) :
		m_v0(_v0),
		m_v1(_v1),
		m_v2(_v2)
	{

	}

	//================================================================================================================================
	//================================================================================================================================
	btVector3 Triangle::GetCenter() const
	{
		return (m_v0 + m_v1 + m_v2) / 3.f;
	}

	//================================================================================================================================
	//================================================================================================================================
	btVector3 Triangle::GetNormal() const
	{
		return (m_v1 - m_v0).cross( m_v2 - m_v1);
	}

	//================================================================================================================================
	// Möller–Trumbore intersection algorithm
	//================================================================================================================================
	bool Triangle::RayCast(const btVector3 _origin, const btVector3 _dir, btVector3& _outIntersection) const
	{
		float EPSILON = 0.0000001f;
		btVector3 e1 = m_v1 - m_v0;	// edge 1
		btVector3 e2 = m_v2 - m_v0;	// edge 2
		btVector3 h = _dir.cross(e2);	// 
		float a = e1.dot(h);
		if (a > -EPSILON && a < EPSILON)	// d colinear to the e1 e2 plane
			return false;

		float f = 1 / a;
		btVector3 s = _origin - m_v0;
		float u = f * s.dot(h);
		if (u < 0.0 || u > 1.0)
			return false;

		btVector3 q = s.cross(e1);
		float v = f * _dir.dot(q);
		if (v < 0.0 || u + v > 1.0)
			return false;

		// At this stage we can compute t to find out where the intersection point is on the line.
		float t = f * e2.dot(q);
		if (t > EPSILON) // ray intersection
		{
			_outIntersection = _origin + _dir * t;
			return true;
		}

		// This means that there is a line intersection but not a ray intersection.
		return false;
	}
}