#pragma once

#include "core/math/fanFixedPoint.hpp"

namespace fan
{
    struct Vector3;
    struct Quaternion;

    //==========================================================================================================================
    //     |e11 e12 e13|
    // M = |e21 e22 e23|
    //     |e31 e32 e33|
    //==========================================================================================================================
    struct Matrix3
    {
        Fixed e11, e12, e13, e21, e22, e23, e31, e32, e33;

        Matrix3();
        Matrix3( Fixed _r1c1, Fixed _r1c2, Fixed _r1c3,
                 Fixed _r2c1, Fixed _r2c2, Fixed _r2c3,
                 Fixed _r3c1, Fixed _r3c2, Fixed _r3c3 );
        Matrix3( const Quaternion& _quat );
        Matrix3( const Vector3& _v1, const Vector3& _v2, const Vector3& _v3 );

        static const Matrix3 sZero;
        static const Matrix3 sIdentity;
        static Matrix3 SkewSymmetric( const Vector3& _vector );

        bool operator==( const Matrix3& _mat3 ) const;
        bool operator!=( const Matrix3& _mat3 ) const { return !( *this == _mat3 ); }
        Matrix3 operator+( const Matrix3& _mat3 ) const;
        Matrix3& operator+=( const Matrix3& _mat3 );
        Matrix3 operator-( const Matrix3& _mat3 ) const;
        Matrix3& operator-=( const Matrix3& _mat3 );
        Matrix3 operator-() const; // unary (-)
        Matrix3 operator*( const Fixed& _value ) const;
        Matrix3& operator*=( const Fixed& _value );
        Matrix3 operator*( const Matrix3& _mat3 ) const;
        Vector3 operator*( const Vector3& _vec3 ) const;
        Matrix3 operator/( const Fixed& _value ) const;

        Matrix3& operator/=( const Fixed& _value );
        Quaternion ToQuaternion() const;
        Fixed Determinant() const;
        Matrix3 Transpose() const;
        Matrix3 Inverse() const;
    };

    inline Matrix3 operator*( const Fixed& _value, const Matrix3& _mat3 ) { return _mat3 * _value; }
}