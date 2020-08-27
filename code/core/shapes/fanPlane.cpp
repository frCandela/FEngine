#include "core/shapes/fanPlane.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	Plane::Plane( const btVector3 _point, const btVector3 _normal ) :
            mPoint( _point ),
            mNormal( _normal )
	{}

	//========================================================================================================
	//========================================================================================================
	bool Plane::RayCast( const btVector3 _origin, const btVector3 _dir, btVector3& _outIntersection ) const
	{

		float dirDotNormal = _dir.dot( mNormal );
		if ( dirDotNormal == 0 )
		{
			return false;
		}
		_outIntersection = _origin - ( _origin + mPoint ).dot( mNormal ) / dirDotNormal * _dir;
		return true;
	}
}
