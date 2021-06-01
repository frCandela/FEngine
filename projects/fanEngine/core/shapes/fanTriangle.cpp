#include "core/shapes/fanTriangle.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    Triangle::Triangle( const Vector3 _v0, const Vector3 _v1, const Vector3 _v2 ) :
            mV0( _v0 ),
            mV1( _v1 ),
            mV2( _v2 )
    {
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    Vector3 Triangle::GetCenter() const
    {
        return ( mV0 + mV1 + mV2 ) / 3;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    Vector3 Triangle::GetNormal() const
    {
        return Vector3::Cross( mV1 - mV0, mV2 - mV1 );
    }

    //==================================================================================================================================================================================================
    // Moller-Trumbore intersection algorithm
    //==================================================================================================================================================================================================
    bool Triangle::RayCast( const Ray _ray, RaycastResult& _outResult ) const
    {
        Fixed   EPSILON = Fixed::sFuzzyZero;
        Vector3 e1      = mV1 - mV0;    // edge 1
        Vector3 e2      = mV2 - mV0;    // edge 2
        Vector3 h       = Vector3::Cross( _ray.direction, e2 );    //
        Fixed   a       = Vector3::Dot( e1, h );
        if( a > -EPSILON && a < EPSILON )
        {    // d colinear to the e1 e2 plane
            return false;
        }

        Fixed   f = 1 / a;
        Vector3 s = _ray.origin - mV0;
        Fixed   u = f * Vector3::Dot( s, h );
        if( u < 0 || u > 1 )
        {
            return false;
        }

        Vector3 q = Vector3::Cross( s, e1 );
        Fixed   v = f * Vector3::Dot( _ray.direction, q );
        if( v < 0 || u + v > 1 )
        {
            return false;
        }

        // At this stage we can compute t to find out where the intersection point is on the line.
        Fixed t = f * Vector3::Dot( e2, q );
        if( t > EPSILON ) // ray intersection
        {
            _outResult.mPosition = _ray.origin + _ray.direction * t;
            _outResult.mDistance = t;
            _outResult.mNormal   = Vector3::Cross( e1, e2 ).Normalized();
            return true;
        }

        // This means that there is a line intersection but not a ray intersection.
        return false;
    }
}
