#include "fanIncludes.h"

#include "core/math/shapes/fanRay.h"

namespace fan {
	namespace shape {
		//================================================================================================================================
		//================================================================================================================================
		void Ray::RayClosestPoints(const Ray _ray, btVector3& _outIntersectionRay, btVector3& _outIntersectionOtherRay) const {
			const btVector3 w0 = origin - _ray.origin;
			const float a = direction.dot(direction);
			const float b = direction.dot(_ray.direction);
			const float c = _ray.direction.dot(_ray.direction);
			const float d = direction.dot(w0);
			const float e = _ray.direction.dot(w0);
			const float s = (b*e - c * d) / (a * c - b * b);
			const float t = (a*e - b * d) / (a*c - b * b);
			_outIntersectionRay = origin + s * direction;
			_outIntersectionOtherRay = _ray.origin + t * _ray.direction;
		}
	}
}