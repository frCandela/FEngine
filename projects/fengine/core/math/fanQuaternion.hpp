#pragma once

#include "core/math/fanVector3.hpp"

namespace fan
{
    //==========================================================================================================================
    //==========================================================================================================================
    struct Quaternion
    {
        Fixed   n;
        Vector3 v;

        Quaternion() : n( 1 ), v() {} // identity
        Quaternion( Fixed _e0, Fixed _e1, Fixed _e2, Fixed _e3 ) : n( _e0 ), v( _e1, _e2, _e3 ) {}
        Quaternion( Fixed _n, Vector3 _v ) : n( _n ), v( _v ) {}

        static const Quaternion sIdentity;

        Fixed SqrMagnitude() const { return n * n + v.x * v.x + v.y * v.y + v.z * v.z; }
        Fixed Magnitude() const { return Fixed::Sqrt( SqrMagnitude() ); }
        Quaternion Conjugate() const { return { n, -v }; }
        Quaternion Inverse() const { return Conjugate() / SqrMagnitude(); }
        Fixed Angle() const { return FIXED( 2 ) * Fixed::ACos( n ); }
        Vector3 Axis() const { return v.Normalized(); }
        Vector3 Euler() const;
        static Quaternion AngleAxis( const Fixed _degrees, const Vector3& _axis );
        static Quaternion Euler( const Vector3& _degrees );
        static Quaternion Euler( const Fixed _x, const Fixed _y, const Fixed _z ) { return Euler( Vector3( _x, _y, _z ) ); }
        static Quaternion Multiply( const Quaternion& q, const Vector3& v );
        static Quaternion Multiply( const Vector3& v, const Quaternion& q );

        bool operator==( const Quaternion& _quat ) const { return _quat.n == n && _quat.v == v; }
        bool operator!=( const Quaternion& _quat ) const { return !( *this == _quat ); }

        Quaternion operator+( const Quaternion& _quat ) const { return Quaternion( n + _quat.n, v + _quat.v ); }
        Quaternion& operator+=( const Quaternion& _quat ) { return ( *this = *this + _quat ); }

        Quaternion operator-( const Quaternion& _quat ) const { return Quaternion( n - _quat.n, v - _quat.v ); }
        Quaternion& operator-=( const Quaternion& _quat ) { return ( *this = *this - _quat ); }
        Quaternion operator-() const { return Quaternion( -n, -v.x, -v.y, -v.z ); } // unary (-)

        Quaternion operator*( const Fixed& _value ) const { return Quaternion( n * _value, v * _value ); }
        Quaternion& operator*=( const Fixed& _value ) { return ( *this = *this * _value ); }
        Quaternion operator*( const Quaternion& _quat ) const;

        Quaternion operator/( const Fixed& _value ) const { return Quaternion( n / _value, v / _value ); }
        Quaternion& operator/=( const Fixed& _value ) { return ( *this = *this / _value ); }
    };

    //=============================================================
    //=============================================================
    inline Quaternion operator*( const Fixed& _value, const Quaternion& _quat ) { return _quat * _value; }

    //=============================================================
    //=============================================================
    inline Vector3 operator*( const Quaternion& _quat, const Vector3& _vec )
    {
        return ( Quaternion::Multiply( _quat, _vec ) * _quat.Conjugate() ).v;
    }
}