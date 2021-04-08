#pragma once

#include "core/math/fanFixedPoint.hpp"

namespace fan
{
    struct Vector3;
    //==========================================================================================================================
    //     |e11 e12 e13 e14|
    // M = |e21 e22 e23 e24|
    //     |e31 e32 e33 e34|
    //     | 0   0   0   1 |
    //==========================================================================================================================
    struct Matrix4
    {
        Fixed e11, e12, e13, e14,
              e21, e22, e23, e24,
              e31, e32, e33, e34;

        Matrix4();
        Matrix4( Fixed _r1c1, Fixed _r1c2, Fixed _r1c3, Fixed _r1c4,
                 Fixed _r2c1, Fixed _r2c2, Fixed _r2c3, Fixed _r2c4,
                 Fixed _r3c1, Fixed _r3c2, Fixed _r3c3, Fixed _r3c4 );

        static const Matrix4 sZero;
        static const Matrix4 sIdentity;

        bool operator==( const Matrix4& _mat4 ) const;
        bool operator!=( const Matrix4& _mat4 ) const { return !( *this == _mat4 ); }
        Matrix4 operator+( const Matrix4& _mat3 ) const;
        Matrix4& operator+=( const Matrix4& _mat3 );
        Matrix4 operator-( const Matrix4& _mat3 ) const;
        Matrix4& operator-=( const Matrix4& _mat3 );
        Matrix4 operator-() const; // unary (-)
        Matrix4 operator*( const Fixed& _value ) const;
        //Matrix4& operator*=( const Fixed& _value );
        Matrix4 operator*( const Matrix4& _mat4 ) const;
        //Vector3 operator*( const Vector3& _vec3 ) const;
        Matrix4 operator/( const Fixed& _value ) const;
        Matrix4& operator/=( const Fixed& _value );

        Fixed Determinant() const;
        Matrix4 Transpose() const;
        //Matrix4 Inverse() const;
    };

    inline Matrix4 operator*( const Fixed& _value, const Matrix4& _mat3 ) { return _mat3 * _value; }
}