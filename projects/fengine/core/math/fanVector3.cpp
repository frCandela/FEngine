#include "core/math/fanVector3.hpp"

namespace fan
{
    const Vector3 Vector3::sBack	= Vector3(0, 0, -1);
    const Vector3 Vector3::sDown	= Vector3(0, -1, 0);
    const Vector3 Vector3::sForward	= Vector3(0, 0, 1);
    const Vector3 Vector3::sLeft    = Vector3(1, 0, 0);
    const Vector3 Vector3::sOne	    = Vector3(1, 1, 1);
    const Vector3 Vector3::sRight   = Vector3(-1, 0, 0);
    const Vector3 Vector3::sUp	    = Vector3(0, 1, 0);
    const Vector3 Vector3::sZero    = Vector3(0, 0, 0);

    //==========================================================================================================================
    //==========================================================================================================================
    void Vector3::Normalize()
    {
        const Fixed magnitude = Magnitude();
        if( magnitude > FX_BIAS ) // don't divide by zero
        {
            x /= magnitude;
            y /= magnitude;
            z /= magnitude;
        }
        if( Fixed::Abs( x ) < FX_BIAS ){ x = 0; }
        if( Fixed::Abs( y ) < FX_BIAS ){ y = 0; }
        if( Fixed::Abs( z ) < FX_BIAS ){ z = 0; }
    }

    //==========================================================================================================================
    //==========================================================================================================================
    Vector3 Vector3::Normalized() const
    {
        Vector3 cpy( *this );
        cpy.Normalize();
        return cpy;
    }

    //==========================================================================================================================
    //==========================================================================================================================
    Vector3 Vector3::Cross( const Vector3& _v1, const Vector3& _v2 )
    {
        return Vector3( _v1.y * _v2.z - _v1.z * _v2.y,
                        -_v1.x * _v2.z + _v1.z * _v2.x,
                        _v1.x * _v2.y - _v1.y * _v2.x );
    }
}