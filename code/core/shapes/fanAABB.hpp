#pragma once

#include <vector>
#include "fanDisableWarnings.hpp"
WARNINGS_GLM_PUSH()
#include "glm/glm.hpp"
WARNINGS_POP()
#include "core/shapes/fanTriangle.hpp"
#include "core/math/fanMathUtils.hpp"

namespace fan
{
	//========================================================================================================
	// axis aligned bounding box
	//========================================================================================================
	struct AABB : public Shape
	{
		AABB();
		AABB( const btVector3 _low, const btVector3 _high );
		AABB( const std::vector<btVector3> _pointCloud, const glm::mat4 _modelMatrix );

		void					    Clear() { mLow = btVector3_Zero; mHigh = btVector3_Zero; }
		btVector3					GetLow() const { return mLow; }
		btVector3					GetHigh() const { return mHigh; }
		std::vector< btVector3 >	GetCorners() const;

        virtual bool RayCast( const btVector3 _origin,
                              const btVector3 _direction,
                              btVector3& outIntersection ) const override;

		btVector3 mLow;
		btVector3 mHigh;
	};
}
