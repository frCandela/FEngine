#include "fanGlobalIncludes.h"

#include "core/math/shapes/fanPlane.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	Plane::Plane(const btVector3 _point, const btVector3 _normal) :
		m_point(_point),
		m_normal(_normal)
	{

	}

	//================================================================================================================================
	//================================================================================================================================
	bool Plane::RayCast(const btVector3 _origin, const btVector3 _dir, btVector3& _outIntersection) const {

		float dirDotNormal = _dir.dot(m_normal);
		if (dirDotNormal == 0) {
			return false;
		}
		_outIntersection = _origin - (_origin + m_point).dot(m_normal) / dirDotNormal * _dir;
		return true;
	}
}
