#pragma once

#include "core/shapes/fanTriangle.hpp"
#include <array>

namespace fan
{
	//========================================================================================================
	// @todo remake this properly without using triangles and name it "Box"
	//========================================================================================================
	struct Cube : public Shape
	{
		Cube( const btVector3 _position, const float _halfSize );

		void SetPosition( const btVector3 _pos );

		const		std::array< Triangle, 12 >& GetTriangles() const { return mTriangles; }
		float		GetHalfSize() const { return mHalfSize; }
		btVector3	GetPosition() const { return mPosition; }

        virtual bool RayCast( const btVector3 _origin,
                              const btVector3 _direction,
                              btVector3& outIntersection ) const override;

        std::array< Triangle, 12 > mTriangles;
        btVector3                  mPosition;
        float                      mHalfSize;

	private:
		void InitCube();


	};
}
