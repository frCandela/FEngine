#pragma once

#include "core/shapes/fanShape.hpp"

namespace fan
{
	//========================================================================================================
	// Triangle shape used for ray casting
	//========================================================================================================
	struct Triangle : public Shape
	{

		Triangle() : mV0( 0, 0, 0 ), mV1( 0, 0, 0 ), mV2( 0, 0, 0 ) {}
		Triangle( const btVector3 _v0, const btVector3 _v1, const btVector3 _v2 );

		btVector3 GetCenter() const;
		btVector3 GetNormal() const;

        virtual bool RayCast( const btVector3 _origin,
                              const btVector3 _dir,
                              btVector3& _outIntersection ) const override;

		btVector3 mV0;
		btVector3 mV1;
		btVector3 mV2;
	};
}