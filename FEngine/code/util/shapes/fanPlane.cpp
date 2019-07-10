#include "fanIncludes.h"

#include "util/shapes/fanPlane.h"

namespace shape {
	Plane::Plane(const btVector3 _point, const btVector3 _normal) :
		m_point(_point),
		m_normal(_normal)
	{

	}

	bool Plane::RayCast(const btVector3 _origin, const btVector3 _dir, btVector3& _outIntersection) const {
		const float dirDotNormal = _dir.dot(m_normal);

		// dir parallel to plane
		if (dirDotNormal == 0.f) {
			return false;
		}

		const float distance = (m_point - _origin).dot(m_normal) / dirDotNormal;

		_outIntersection = _origin + _dir.normalized() * distance;
		return true;
	}
}