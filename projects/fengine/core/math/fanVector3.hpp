#pragma once

#include "core/math/fanFixedPoint.hpp"

namespace fan
{
    //==========================================================================================================================
    //==========================================================================================================================
    struct Vector3
    {
        Fixed x;
        Fixed y;
        Fixed z;

        constexpr Vector3() : x( 0 ), y( 0 ), z( 0 ) {}
        constexpr Vector3( const Fixed& _x, const Fixed& _y, const Fixed& _z ) : x( _x ), y( _y ), z( _z ) {}
        constexpr Vector3( const Vector3& _other ) : x( _other.x ), y( _other.y ), z( _other.z ) {}

        Fixed SqrMagnitude() const { return x * x + y * y + z * z; }
        Fixed Magnitude() const { return Fixed::Sqrt( SqrMagnitude() ); }
        void Normalize();
        Vector3 Normalized() const;
        bool IsNormalized() const { return Fixed::IsFuzzyZero( SqrMagnitude() - 1 ); }
        static void OrthoNormalize( Vector3& _normal, Vector3& _tangent );
        static Fixed Dot( const Vector3& _v1, const Vector3& _v2 ) { return _v1.x * _v2.x + _v1.y * _v2.y + _v1.z * _v2.z; }
        static Fixed SignedAngle( const Vector3& _v1, const Vector3& _v2, const Vector3& _normal );
        static Vector3 Cross( const Vector3& _v1, const Vector3& _v2 );
        static bool IsFuzzyZero( const Vector3& _vec3 ) { return Fixed::IsFuzzyZero( _vec3.x ) && Fixed::IsFuzzyZero( _vec3.y ) && Fixed::IsFuzzyZero( _vec3.z ); }

        bool operator==( const Vector3& _vec3 ) const { return _vec3.x == x && _vec3.y == y && _vec3.z == z; }
        bool operator!=( const Vector3& _vec3 ) const { return !( *this == _vec3 ); }

        Vector3 operator*( const Fixed& _value ) const { return Vector3( _value * x, _value * y, _value * z ); }
        Vector3& operator*=( const Fixed& _value ) { return ( *this = *this * _value ); }
        Vector3 operator*( const Vector3& _vec3 ) const { return Vector3( _vec3.x * x, _vec3.y * y, _vec3.z * z ); }
        Vector3& operator*=( const Vector3& _vec3 ) { return ( *this = *this * _vec3 ); }

        Vector3 operator/( const Fixed& _value ) const { return Vector3( x / _value, y / _value, z / _value ); }
        Vector3& operator/=( const Fixed& _value ) { return ( *this = *this / _value ); }

        Vector3 operator+( const Vector3& _vec3 ) const { return Vector3( x + _vec3.x, y + _vec3.y, z + _vec3.z ); }
        Vector3& operator+=( const Vector3& _vec3 ) { return ( *this = *this + _vec3 ); }

        Vector3 operator-( const Vector3& _vec3 ) const { return Vector3( x - _vec3.x, y - _vec3.y, z - _vec3.z ); }
        Vector3& operator-=( const Vector3& _vec3 ) { return ( *this = *this - _vec3 ); }
        Vector3 operator-() const { return Vector3( -x, -y, -z ); } // unary (-)

        static const Vector3 sBack;
        static const Vector3 sDown;
        static const Vector3 sForward;
        static const Vector3 sLeft;
        static const Vector3 sOne;
        static const Vector3 sRight;
        static const Vector3 sUp;
        static const Vector3 sZero;
    };

    inline Vector3 operator*( const Fixed& _value, const Vector3& _vec3 ) { return _vec3 * _value; }
}