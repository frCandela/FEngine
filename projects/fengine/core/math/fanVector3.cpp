#include <core/fanAssert.hpp>
#include "core/math/fanVector3.hpp"

namespace fan
{
    const Vector3 Vector3::sBack    = Vector3( 0, 0, -1 );
    const Vector3 Vector3::sDown    = Vector3( 0, -1, 0 );
    const Vector3 Vector3::sForward = Vector3( 0, 0, 1 );
    const Vector3 Vector3::sLeft    = Vector3( 1, 0, 0 );
    const Vector3 Vector3::sOne     = Vector3( 1, 1, 1 );
    const Vector3 Vector3::sRight   = Vector3( -1, 0, 0 );
    const Vector3 Vector3::sUp      = Vector3( 0, 1, 0 );
    const Vector3 Vector3::sZero    = Vector3( 0, 0, 0 );

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
    // Makes vectors normalized and orthogonal to each other.
    // Normalizes normal. Normalizes tangent and makes sure it is orthogonal to normal.
    // Uses Gram–Schmidt process
    //==========================================================================================================================
    void Vector3::OrthoNormalize( Vector3& _normal, Vector3& _tangent )
    {
        fanAssertMsg( _normal.IsNormalized(), "Vector3::OrthoNormalize: _normal is not normalized" );
        Fixed dot = Vector3::Dot( _tangent, _normal );
        _tangent -= dot * _normal;
        _tangent.Normalize();
    }

    //==========================================================================================================================
    //==========================================================================================================================
    Fixed Vector3::SignedAngle( const Vector3& _v1, const Vector3& _v2, const Vector3& _normal )
    {
        const Fixed   angle = Fixed::ACos( Vector3::Dot( _v1.Normalized(), _v2.Normalized() ) );
        const Vector3 cross = Vector3::Cross( _v1, _v2 );
        if( Vector3::Dot( _normal, cross ) < 0 )
        {
            return -Fixed::Degrees( angle );
        }
        return Fixed::Degrees( angle );
    }

    //==========================================================================================================================
    //==========================================================================================================================
    Vector3 Vector3::Cross( const Vector3& _v1, const Vector3& _v2 )
    {
        return Vector3( _v1.y * _v2.z - _v1.z * _v2.y,
                        -_v1.x * _v2.z + _v1.z * _v2.x,
                        _v1.x * _v2.y - _v1.y * _v2.x );
    }

    //==========================================================================================================================
    // x is given, y and z are created
    //==========================================================================================================================
    void Vector3::MakeOrthonormalBasis( const Vector3& _x, Vector3& _y, Vector3& _z )
    {
        fanAssert( _x.IsNormalized() );
        if( Fixed::Abs( _x.x ) > Fixed::Abs( _x.y ) )
        {
            _z = Vector3( -_x.z, 0, _x.x ); // cross(_x,up)
            _z.Normalize();
            _y = Vector3( -_z.z * _x.y, -_z.x * _x.z + _z.z * _x.x, _z.x * _x.y );// cross(_z,x)
        }
        else
        {
            _z = Vector3( 0, _x.z, -_x.y ); // cross( _x, left );
            _z.Normalize();
            _y = Vector3( _z.y * _x.z - _z.z * _x.y, _z.z * _x.x, -_z.y * _x.x ); //cross( _z, _x );
        }
    }
}