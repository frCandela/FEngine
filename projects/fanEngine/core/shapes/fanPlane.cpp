#include "core/shapes/fanPlane.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    Plane::Plane( const Vector3 _point, const Vector3 _normal ) :
            mPoint( _point ),
            mNormal( _normal ) {}

    //========================================================================================================
    //========================================================================================================
    bool Plane::RayCast( const Vector3& _origin, const Vector3& _dir, Vector3& _outIntersection ) const
    {

        Fixed dirDotNormal = Vector3::Dot( _dir, mNormal );
        if( dirDotNormal == 0 )
        {
            return false;
        }
        _outIntersection = _origin - Vector3::Dot( _origin + mPoint, mNormal ) / dirDotNormal * _dir;
        return true;
    }
}
