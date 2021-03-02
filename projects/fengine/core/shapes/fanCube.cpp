#include <limits>
#include "core/shapes/fanCube.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	Cube::Cube( const btVector3 _position, float const _halfSize ) :
            mPosition( _position )
		, mHalfSize( _halfSize )
	{
		InitCube();
	}

	//========================================================================================================
	//========================================================================================================
	void Cube::SetPosition( const btVector3 _pos )
	{
		if ( mPosition != _pos )
		{
            mPosition = _pos;
			InitCube();
		}
	}

	//========================================================================================================
	//========================================================================================================
    bool Cube::RayCast( const btVector3 _origin,
                        const btVector3 _direction,
                        btVector3& _outIntersection ) const
    {
		btVector3 intersection;
		float closestDistance = std::numeric_limits<float>::max();
		for ( const Triangle& triangle : mTriangles )
		{
			if ( triangle.RayCast( _origin, _direction, intersection ) )
			{
				float distance = intersection.distance( _origin );
				if ( distance < closestDistance )
				{
					closestDistance = distance;
					_outIntersection = intersection;
				}
			}
		}
		return closestDistance != std::numeric_limits<float>::max();
	}

	//========================================================================================================
	//========================================================================================================
	void Cube::InitCube()
	{
		const btVector3 e1 = mPosition + btVector3( -mHalfSize, -mHalfSize, -mHalfSize );
		const btVector3 e2 = mPosition + btVector3( -mHalfSize, -mHalfSize, mHalfSize );
		const btVector3 e3 = mPosition + btVector3( -mHalfSize, mHalfSize, -mHalfSize );
		const btVector3 e4 = mPosition + btVector3( -mHalfSize, mHalfSize, mHalfSize );
		const btVector3 e5 = mPosition + btVector3( mHalfSize, -mHalfSize, -mHalfSize );
		const btVector3 e6 = mPosition + btVector3( mHalfSize, -mHalfSize, mHalfSize );
		const btVector3 e7 = mPosition + btVector3( mHalfSize, mHalfSize, -mHalfSize );
		const btVector3 e8 = mPosition + btVector3( mHalfSize, mHalfSize, mHalfSize );

        mTriangles[ 0 ]  = { e1, e2, e4 };
        mTriangles[ 1 ]  = { e1, e3, e4 };
        mTriangles[ 2 ]  = { e5, e6, e8 };
        mTriangles[ 3 ]  = { e5, e7, e8 };
        mTriangles[ 4 ]  = { e2, e6, e8 };
        mTriangles[ 5 ]  = { e2, e4, e8 };
        mTriangles[ 6 ]  = { e1, e5, e7 };
        mTriangles[ 7 ]  = { e1, e3, e7 };
        mTriangles[ 8 ]  = { e3, e4, e8 };
        mTriangles[ 9 ]  = { e3, e7, e8 };
        mTriangles[ 10 ] = { e1, e2, e6 };
        mTriangles[ 11 ] = { e1, e5, e6 };
	}
}

