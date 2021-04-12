#include "core/math/fanMatrix3.hpp"
#include "core/math/fanQuaternion.hpp"

namespace fan
{
    const Matrix3 Matrix3::sZero     = Matrix3();
    const Matrix3 Matrix3::sIdentity = Matrix3( 1, 0, 0,
                                                0, 1, 0,
                                                0, 0, 1 );

    //==========================================================================================================================
    //==========================================================================================================================
    Matrix3::Matrix3() :
            e11( 0 ), e12( 0 ), e13( 0 ),
            e21( 0 ), e22( 0 ), e23( 0 ),
            e31( 0 ), e32( 0 ), e33( 0 ) {}

    //==========================================================================================================================
    //==========================================================================================================================
    Matrix3::Matrix3( Fixed _r1c1, Fixed _r1c2, Fixed _r1c3,
                      Fixed _r2c1, Fixed _r2c2, Fixed _r2c3,
                      Fixed _r3c1, Fixed _r3c2, Fixed _r3c3 ) :
            e11( _r1c1 ), e12( _r1c2 ), e13( _r1c3 ),
            e21( _r2c1 ), e22( _r2c2 ), e23( _r2c3 ),
            e31( _r3c1 ), e32( _r3c2 ), e33( _r3c3 ) {}

    //==========================================================================================================================
    //==========================================================================================================================
    Matrix3::Matrix3( const Quaternion& _quat )
    {
        e11 = 1 - ( 2 * _quat.mAxis.y * _quat.mAxis.y + 2 * _quat.mAxis.z * _quat.mAxis.z );
        e12 = 2 * _quat.mAxis.x * _quat.mAxis.y + 2 * _quat.mAxis.z * _quat.mAngle;
        e13 = 2 * _quat.mAxis.x * _quat.mAxis.z - 2 * _quat.mAxis.y * _quat.mAngle;
        e21 = 2 * _quat.mAxis.x * _quat.mAxis.y - 2 * _quat.mAxis.z * _quat.mAngle;
        e22 = 1 - ( 2 * _quat.mAxis.x * _quat.mAxis.x + 2 * _quat.mAxis.z * _quat.mAxis.z );
        e23 = 2 * _quat.mAxis.y * _quat.mAxis.z + 2 * _quat.mAxis.x * _quat.mAngle;
        e31 = 2 * _quat.mAxis.x * _quat.mAxis.z + 2 * _quat.mAxis.y * _quat.mAngle;
        e32 = 2 * _quat.mAxis.y * _quat.mAxis.z - 2 * _quat.mAxis.x * _quat.mAngle;
        e33 = 1 - ( 2 * _quat.mAxis.x * _quat.mAxis.x + 2 * _quat.mAxis.y * _quat.mAxis.y );
    }



    //==========================================================================================================================
    // creates a transform rotation matrix from a set of orthonormal axis
    //==========================================================================================================================
    Matrix3::Matrix3( const Vector3& _v1, const Vector3& _v2, const Vector3& _v3 )
    {
        e11 = _v1.x;
        e21 = _v1.y;
        e31 = _v1.z;
        e12 = _v2.x;
        e22 = _v2.y;
        e32 = _v2.z;
        e13 = _v3.x;
        e23 = _v3.y;
        e33 = _v3.z;
    }

    //==========================================================================================================================
    //==========================================================================================================================
    bool Matrix3::operator==( const Matrix3& _mat3 ) const
    {
        return e11 == _mat3.e11 && e12 == _mat3.e12 && e13 == _mat3.e13 &&
               e21 == _mat3.e21 && e22 == _mat3.e22 && e23 == _mat3.e23 &&
               e31 == _mat3.e31 && e32 == _mat3.e32 && e33 == _mat3.e33;
    }

    //==========================================================================================================================
    //==========================================================================================================================
    Matrix3 Matrix3::operator+( const Matrix3& _mat3 ) const
    {
        return Matrix3( e11 + _mat3.e11, e12 + _mat3.e12, e13 + _mat3.e13,
                        e21 + _mat3.e21, e22 + _mat3.e22, e23 + _mat3.e23,
                        e31 + _mat3.e31, e32 + _mat3.e32, e33 + _mat3.e33 );
    }

    //==========================================================================================================================
    //==========================================================================================================================
    Matrix3& Matrix3::operator+=( const Matrix3& _mat3 )
    {
        e11 += _mat3.e11;
        e12 += _mat3.e12;
        e13 += _mat3.e13;
        e21 += _mat3.e21;
        e22 += _mat3.e22;
        e23 += _mat3.e23;
        e31 += _mat3.e31;
        e32 += _mat3.e32;
        e33 += _mat3.e33;
        return *this;
    }

    //==========================================================================================================================
    //==========================================================================================================================
    Matrix3 Matrix3::operator-( const Matrix3& _mat3 ) const
    {
        return Matrix3( e11 - _mat3.e11, e12 - _mat3.e12, e13 - _mat3.e13,
                        e21 - _mat3.e21, e22 - _mat3.e22, e23 - _mat3.e23,
                        e31 - _mat3.e31, e32 - _mat3.e32, e33 - _mat3.e33 );
    }

    //==========================================================================================================================
    //==========================================================================================================================
    Matrix3& Matrix3::operator-=( const Matrix3& _mat3 )
    {
        e11 -= _mat3.e11;
        e12 -= _mat3.e12;
        e13 -= _mat3.e13;
        e21 -= _mat3.e21;
        e22 -= _mat3.e22;
        e23 -= _mat3.e23;
        e31 -= _mat3.e31;
        e32 -= _mat3.e32;
        e33 -= _mat3.e33;
        return *this;
    }

    //==========================================================================================================================
    //==========================================================================================================================
    Matrix3 Matrix3::operator-() const
    {
        return Matrix3( -e11, -e12, -e13,
                        -e21, -e22, -e23,
                        -e31, -e32, -e33 );
    }

    //==========================================================================================================================
    //==========================================================================================================================
    Matrix3 Matrix3::operator*( const Fixed& _value ) const
    {
        return Matrix3( e11 * _value, e12 * _value, e13 * _value,
                        e21 * _value, e22 * _value, e23 * _value,
                        e31 * _value, e32 * _value, e33 * _value );
    }

    //==========================================================================================================================
    //==========================================================================================================================
    Matrix3& Matrix3::operator*=( const Fixed& _value )
    {
        e11 *= _value;
        e12 *= _value;
        e13 *= _value;
        e21 *= _value;
        e22 *= _value;
        e23 *= _value;
        e31 *= _value;
        e32 *= _value;
        e33 *= _value;
        return *this;
    }

    //==========================================================================================================================
    //==========================================================================================================================
    Matrix3 Matrix3::operator/( const Fixed& _value ) const
    {
        return Matrix3( e11 / _value, e12 / _value, e13 / _value,
                        e21 / _value, e22 / _value, e23 / _value,
                        e31 / _value, e32 / _value, e33 / _value );
    }

    //==========================================================================================================================
    //==========================================================================================================================
    Matrix3& Matrix3::operator/=( const Fixed& _value )
    {
        e11 /= _value;
        e12 /= _value;
        e13 /= _value;
        e21 /= _value;
        e22 /= _value;
        e23 /= _value;
        e31 /= _value;
        e32 /= _value;
        e33 /= _value;
        return *this;
    }

    //==========================================================================================================================
    //==========================================================================================================================
    Matrix3 Matrix3::operator*( const Matrix3& _mat3 ) const
    {
        return Matrix3(
                e11 * _mat3.e11 + e12 * _mat3.e21 + e13 * _mat3.e31,
                e11 * _mat3.e12 + e12 * _mat3.e22 + e13 * _mat3.e32,
                e11 * _mat3.e13 + e12 * _mat3.e23 + e13 * _mat3.e33,
                e21 * _mat3.e11 + e22 * _mat3.e21 + e23 * _mat3.e31,
                e21 * _mat3.e12 + e22 * _mat3.e22 + e23 * _mat3.e32,
                e21 * _mat3.e13 + e22 * _mat3.e23 + e23 * _mat3.e33,
                e31 * _mat3.e11 + e32 * _mat3.e21 + e33 * _mat3.e31,
                e31 * _mat3.e12 + e32 * _mat3.e22 + e33 * _mat3.e32,
                e31 * _mat3.e13 + e32 * _mat3.e23 + e33 * _mat3.e33 );
    }

    //==========================================================================================================================
    //==========================================================================================================================
    Vector3 Matrix3::operator*( const Vector3& _vec3 ) const
    {
        return Vector3( e11 * _vec3.x + e12 * _vec3.y + e13 * _vec3.z,
                        e21 * _vec3.x + e22 * _vec3.y + e23 * _vec3.z,
                        e31 * _vec3.x + e32 * _vec3.y + e33 * _vec3.z );
    }

    //==========================================================================================================================
    //==========================================================================================================================
    Fixed Matrix3::Determinant() const
    {
        return e11 * e22 * e33 -
               e11 * e32 * e23 +
               e21 * e32 * e13 -
               e21 * e12 * e33 +
               e31 * e12 * e23 -
               e31 * e22 * e13;
    }

    //==========================================================================================================================
    //==========================================================================================================================
    Matrix3 Matrix3::Transpose() const
    {
        return Matrix3( e11, e21, e31,
                        e12, e22, e32,
                        e13, e23, e33 );
    }
    //==========================================================================================================================
    //==========================================================================================================================
    Matrix3 Matrix3::Inverse() const
    {
        Fixed d = Determinant();
        if( d == 0 ){ d = 1; }
        return Matrix3( ( e22 * e33 - e23 * e32 ) / d,
                        -( e12 * e33 - e13 * e32 ) / d,
                        ( e12 * e23 - e13 * e22 ) / d,
                        -( e21 * e33 - e23 * e31 ) / d,
                        ( e11 * e33 - e13 * e31 ) / d,
                        -( e11 * e23 - e13 * e21 ) / d,
                        ( e21 * e32 - e22 * e31 ) / d,
                        -( e11 * e32 - e12 * e31 ) / d,
                        ( e11 * e22 - e12 * e21 ) / d );
    }
}