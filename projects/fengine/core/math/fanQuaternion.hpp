#pragma once

#include "core/math/fanVector3.hpp"

namespace fan
{
    //==========================================================================================================================
    //==========================================================================================================================
    struct Quaternion
    {
        static const Quaternion sIdentity;

        Fixed   mAngle; // cos( angle / 2 )
        Vector3 mAxis;  // sin( angle / 2 ) * Axis

        Quaternion() : mAngle( 1 ), mAxis() {} // identity
        Quaternion( Fixed _angle, Fixed _x, Fixed _y, Fixed _z ) : mAngle( _angle ), mAxis( _x, _y, _z ) {}

        Quaternion( Fixed _n, Vector3 _v ) : mAngle( _n ), mAxis( _v ) {}

        Fixed SqrMagnitude() const { return mAngle * mAngle + mAxis.x * mAxis.x + mAxis.y * mAxis.y + mAxis.z * mAxis.z; }
        Fixed Magnitude() const { return Fixed::Sqrt( SqrMagnitude() ); }
        Quaternion Conjugate() const { return { mAngle, -mAxis }; }
        Quaternion Inverse() const { return Conjugate() / SqrMagnitude(); }
        Quaternion Normalized() const;
        void Normalize() { *this = Normalized(); }
        Fixed Angle() const { return FIXED( 2 ) * Fixed::ACos( mAngle ); }
        Vector3 Axis() const { return mAxis.Normalized(); }
        Vector3 Euler() const;

        static Quaternion AngleAxis( const Fixed _degrees, const Vector3& _axis );
        static Quaternion AngleAxisRadians( const Fixed _radians, const Vector3& _axis );
        static Quaternion Euler( const Vector3& _degrees );
        static Quaternion Euler( const Fixed _x, const Fixed _y, const Fixed _z ) { return Euler( Vector3( _x, _y, _z ) ); }
        static Quaternion LookRotation( const Vector3& _direction, const Vector3 _up = Vector3::sUp );
        static Quaternion Multiply( const Quaternion& q, const Vector3& v );
        static Quaternion Multiply( const Vector3& v, const Quaternion& q );

        bool operator==( const Quaternion& _quat ) const { return _quat.mAngle == mAngle && _quat.mAxis == mAxis; }
        bool operator!=( const Quaternion& _quat ) const { return !( *this == _quat ); }

        Quaternion operator+( const Quaternion& _quat ) const { return Quaternion( mAngle + _quat.mAngle, mAxis + _quat.mAxis ); }
        Quaternion& operator+=( const Quaternion& _quat ) { return ( *this = *this + _quat ); }

        Quaternion operator-( const Quaternion& _quat ) const { return Quaternion( mAngle - _quat.mAngle, mAxis - _quat.mAxis ); }
        Quaternion& operator-=( const Quaternion& _quat ) { return ( *this = *this - _quat ); }
        Quaternion operator-() const { return Quaternion( -mAngle, -mAxis.x, -mAxis.y, -mAxis.z ); } // unary (-)

        Quaternion operator*( const Fixed& _value ) const { return Quaternion( mAngle * _value, mAxis * _value ); }
        Quaternion& operator*=( const Fixed& _value ) { return ( *this = *this * _value ); }
        Quaternion operator*( const Quaternion& _quat ) const;

        Quaternion operator/( const Fixed& _value ) const { return Quaternion( mAngle / _value, mAxis / _value ); }
        Quaternion& operator/=( const Fixed& _value ) { return ( *this = *this / _value ); }
    };

    //=============================================================
    //=============================================================
    inline Quaternion operator*( const Fixed& _value, const Quaternion& _quat ) { return _quat * _value; }

    //=============================================================
    //=============================================================
    inline Vector3 operator*( const Quaternion& _quat, const Vector3& _vec )
    {
        return ( Quaternion::Multiply( _quat, _vec ) * _quat.Conjugate() ).mAxis;
    }
}