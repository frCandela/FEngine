#include "core/math/fanMatrix4.hpp"
#include "core/math/fanQuaternion.hpp"

namespace fan
{
    const Matrix4 Matrix4::sZero     = Matrix4();
    const Matrix4 Matrix4::sIdentity = Matrix4( 1, 0, 0, 0,
                                                0, 1, 0, 0,
                                                0, 0, 1, 0 );

    //==========================================================================================================================
    //==========================================================================================================================
    Matrix4::Matrix4() :
            e11( 0 ), e12( 0 ), e13( 0 ), e14( 0 ),
            e21( 0 ), e22( 0 ), e23( 0 ), e24( 0 ),
            e31( 0 ), e32( 0 ), e33( 0 ), e34( 0 ) {}

    //==========================================================================================================================
    //==========================================================================================================================
    Matrix4::Matrix4( Fixed _r1c1, Fixed _r1c2, Fixed _r1c3, Fixed _r1c4,
                      Fixed _r2c1, Fixed _r2c2, Fixed _r2c3, Fixed _r2c4,
                      Fixed _r3c1, Fixed _r3c2, Fixed _r3c3, Fixed _r3c4 ) :
            e11( _r1c1 ), e12( _r1c2 ), e13( _r1c3 ), e14( _r1c4 ),
            e21( _r2c1 ), e22( _r2c2 ), e23( _r2c3 ), e24( _r2c4 ),
            e31( _r3c1 ), e32( _r3c2 ), e33( _r3c3 ), e34( _r3c4 ) {}

    //==========================================================================================================================
    //==========================================================================================================================
    Matrix4::Matrix4( const Quaternion& _quat, const Vector3& _position )
    {
        e14 = _position.x;
        e24 = _position.y;
        e34 = _position.z;

        e11 = 1 - 2 * _quat.mAxis.y * _quat.mAxis.y - 2 * _quat.mAxis.z * _quat.mAxis.z;
        e12 = 2 * _quat.mAxis.x * _quat.mAxis.y - 2 * _quat.mAxis.z * _quat.mAngle;
        e13 = 2 * _quat.mAxis.x * _quat.mAxis.z + 2 * _quat.mAxis.y * _quat.mAngle;

        e21 = 2 * _quat.mAxis.x * _quat.mAxis.y + 2 * _quat.mAxis.z * _quat.mAngle;
        e22 = 1 - 2 * _quat.mAxis.x * _quat.mAxis.x - 2 * _quat.mAxis.z * _quat.mAxis.z;
        e23 = 2 * _quat.mAxis.y * _quat.mAxis.z - 2 * _quat.mAxis.x * _quat.mAngle;

        e31 = 2 * _quat.mAxis.x * _quat.mAxis.z - 2 * _quat.mAxis.y * _quat.mAngle;
        e32 = 2 * _quat.mAxis.y * _quat.mAxis.z + 2 * _quat.mAxis.x * _quat.mAngle;
        e33 = 1 - 2 * _quat.mAxis.x * _quat.mAxis.x - 2 * _quat.mAxis.y * _quat.mAxis.y;
    }

    //==========================================================================================================================
    //==========================================================================================================================
    bool Matrix4::operator==( const Matrix4& _mat3 ) const
    {
        return e11 == _mat3.e11 && e12 == _mat3.e12 && e13 == _mat3.e13 && e14 == _mat3.e14 &&
               e21 == _mat3.e21 && e22 == _mat3.e22 && e23 == _mat3.e23 && e24 == _mat3.e24 &&
               e31 == _mat3.e31 && e32 == _mat3.e32 && e33 == _mat3.e33 && e34 == _mat3.e34;
    }

    //==========================================================================================================================
    //==========================================================================================================================
    Matrix4 Matrix4::operator+( const Matrix4& _mat3 ) const
    {
        return Matrix4( e11 + _mat3.e11, e12 + _mat3.e12, e13 + _mat3.e13, e14 + _mat3.e14,
                        e21 + _mat3.e21, e22 + _mat3.e22, e23 + _mat3.e23, e24 + _mat3.e24,
                        e31 + _mat3.e31, e32 + _mat3.e32, e33 + _mat3.e33, e34 + _mat3.e34 );
    }

    //==========================================================================================================================
    //==========================================================================================================================
    Matrix4& Matrix4::operator+=( const Matrix4& _mat3 )
    {
        e11 += _mat3.e11;
        e12 += _mat3.e12;
        e13 += _mat3.e13;
        e14 += _mat3.e14;
        e21 += _mat3.e21;
        e22 += _mat3.e22;
        e23 += _mat3.e23;
        e24 += _mat3.e24;
        e31 += _mat3.e31;
        e32 += _mat3.e32;
        e33 += _mat3.e33;
        e34 += _mat3.e34;
        return *this;
    }

    //==========================================================================================================================
    //==========================================================================================================================
    Matrix4 Matrix4::operator-( const Matrix4& _mat3 ) const
    {
        return Matrix4( e11 - _mat3.e11, e12 - _mat3.e12, e13 - _mat3.e13, e14 - _mat3.e14,
                        e21 - _mat3.e21, e22 - _mat3.e22, e23 - _mat3.e23, e24 - _mat3.e24,
                        e31 - _mat3.e31, e32 - _mat3.e32, e33 - _mat3.e33, e34 - _mat3.e34 );
    }

    //==========================================================================================================================
    //==========================================================================================================================
    Matrix4& Matrix4::operator-=( const Matrix4& _mat3 )
    {
        e11 -= _mat3.e11;
        e12 -= _mat3.e12;
        e13 -= _mat3.e13;
        e14 -= _mat3.e14;

        e21 -= _mat3.e21;
        e22 -= _mat3.e22;
        e23 -= _mat3.e23;
        e24 -= _mat3.e24;

        e31 -= _mat3.e31;
        e32 -= _mat3.e32;
        e33 -= _mat3.e33;
        e34 -= _mat3.e34;
        return *this;
    }

    //==========================================================================================================================
    //==========================================================================================================================
    Matrix4 Matrix4::operator-() const
    {
        return Matrix4( -e11, -e12, -e13, -e14,
                        -e21, -e22, -e23, -e24,
                        -e31, -e32, -e33, -e34 );
    }

    //==========================================================================================================================
    //==========================================================================================================================
    Matrix4 Matrix4::operator*( const Fixed& _value ) const
    {
        return Matrix4( e11 * _value, e12 * _value, e13 * _value, e14 * _value,
                        e21 * _value, e22 * _value, e23 * _value, e24 * _value,
                        e31 * _value, e32 * _value, e33 * _value, e34 * _value );
    }

    //==========================================================================================================================
    //==========================================================================================================================
    Matrix4& Matrix4::operator*=( const Fixed& _value )
    {
        e11 *= _value;
        e12 *= _value;
        e13 *= _value;
        e14 *= _value;
        e21 *= _value;
        e22 *= _value;
        e23 *= _value;
        e24 *= _value;
        e31 *= _value;
        e32 *= _value;
        e33 *= _value;
        e34 *= _value;
        return *this;
    }

    //==========================================================================================================================
    //==========================================================================================================================
    Matrix4 Matrix4::operator/( const Fixed& _value ) const
    {
        return Matrix4( e11 / _value, e12 / _value, e13 / _value, e14 / _value,
                        e21 / _value, e22 / _value, e23 / _value, e24 / _value,
                        e31 / _value, e32 / _value, e33 / _value, e34 / _value );
    }

    //==========================================================================================================================
    //==========================================================================================================================
    Matrix4& Matrix4::operator/=( const Fixed& _value )
    {
        e11 /= _value;
        e21 /= _value;
        e31 /= _value;
        e12 /= _value;
        e22 /= _value;
        e32 /= _value;
        e13 /= _value;
        e23 /= _value;
        e33 /= _value;
        e14 /= _value;
        e24 /= _value;
        e34 /= _value;
        return *this;
    }

    //==========================================================================================================================
    //==========================================================================================================================
    Matrix4 Matrix4::operator*( const Matrix4& _mat4 ) const
    {
        return Matrix4(
                e11 * _mat4.e11 + e12 * _mat4.e21 + e13 * _mat4.e31,
                e11 * _mat4.e12 + e12 * _mat4.e22 + e13 * _mat4.e32,
                e11 * _mat4.e13 + e12 * _mat4.e23 + e13 * _mat4.e33,
                e11 * _mat4.e14 + e12 * _mat4.e24 + e13 * _mat4.e34 + e14,

                e21 * _mat4.e11 + e22 * _mat4.e21 + e23 * _mat4.e31,
                e21 * _mat4.e12 + e22 * _mat4.e22 + e23 * _mat4.e32,
                e21 * _mat4.e13 + e22 * _mat4.e23 + e23 * _mat4.e33,
                e21 * _mat4.e14 + e22 * _mat4.e24 + e23 * _mat4.e34 + e24,

                e31 * _mat4.e11 + e32 * _mat4.e21 + e33 * _mat4.e31,
                e31 * _mat4.e12 + e32 * _mat4.e22 + e33 * _mat4.e32,
                e31 * _mat4.e13 + e32 * _mat4.e23 + e33 * _mat4.e33,
                e31 * _mat4.e14 + e32 * _mat4.e24 + e33 * _mat4.e34 + e34 );
    }

    //==========================================================================================================================
    //==========================================================================================================================
    Vector3 Matrix4::operator*( const Vector3& _vec3 ) const
    {
        return Vector3( e11 * _vec3.x + e12 * _vec3.y + e13 * _vec3.z + e14,
                        e21 * _vec3.x + e22 * _vec3.y + e23 * _vec3.z + e24,
                        e31 * _vec3.x + e32 * _vec3.y + e33 * _vec3.z + e34 );
    }

    //==========================================================================================================================
    //==========================================================================================================================
    Vector3 Matrix4::GetX() const { return Vector3( e11, e21, e31 ); }
    Vector3 Matrix4::GetY() const { return Vector3( e12, e22, e32 ); }
    Vector3 Matrix4::GetZ() const { return Vector3( e13, e23, e33 ); }
    Vector3 Matrix4::GetOrigin() const { return Vector3( e14, e24, e34 ); }

    //==========================================================================================================================
    //==========================================================================================================================
    void Matrix4::SetOrigin( const Vector3& _origin )
    {
        e14 = _origin.x;
        e24 = _origin.y;
        e34 = _origin.z;
    }

    //==========================================================================================================================
    //==========================================================================================================================
    Quaternion Matrix4::ToQuaternion() const
    {
        Quaternion q;
        q.mAngle = Fixed::Sqrt( 1 + e11 + e22 + e33 ) / 2;
        Fixed w4 = ( 4 * q.mAngle );
        q.mAxis.x = ( e32 - e23 ) / w4;
        q.mAxis.y = ( e13 - e31 ) / w4;
        q.mAxis.z = ( e21 - e12 ) / w4;
        return q;
    }

    //==========================================================================================================================
    // multiply a vector with the inverse of the matrix
    // inverse(M) == transpose(M) for rotation matrices
    //==========================================================================================================================
    Vector3 Matrix4::InverseTransform( const Vector3& _vector3 ) const
    {
        Vector3 tmp = _vector3;
        tmp.x -= e14;
        tmp.y -= e24;
        tmp.z -= e34;
        return Vector3( e11 * tmp.x + e21 * tmp.y + e31 * tmp.z,
                        e12 * tmp.x + e22 * tmp.y + e32 * tmp.z,
                        e13 * tmp.x + e23 * tmp.y + e33 * tmp.z );
    }

    //==========================================================================================================================
    //==========================================================================================================================
    Vector3 Matrix4::TransformDirection( const Vector3& _vector3 ) const
    {
        return Vector3( e11 * _vector3.x + e12 * _vector3.y + e13 * _vector3.z,
                        e21 * _vector3.x + e22 * _vector3.y + e23 * _vector3.z,
                        e31 * _vector3.x + e32 * _vector3.y + e33 * _vector3.z );
    }

    //==========================================================================================================================
    // multiply tmp with the inverse of the rotation matrix ( inverse(M) == transpose(M) for rotation matrices )
    //==========================================================================================================================
    Vector3 Matrix4::InverseTransformDirection( const Vector3& _vector3 ) const
    {
        return Vector3( e11 * _vector3.x + e21 * _vector3.y + e31 * _vector3.z,
                        e12 * _vector3.x + e22 * _vector3.y + e32 * _vector3.z,
                        e13 * _vector3.x + e23 * _vector3.y + e33 * _vector3.z );
    }

    //==========================================================================================================================
    //==========================================================================================================================
    Fixed Matrix4::Determinant() const
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
    Matrix4 Matrix4::Transpose() const
    {
        return Matrix4( e11, e21, e31, 0,
                        e12, e22, e32, 0,
                        e13, e23, e33, 0 );
    }

    //==========================================================================================================================
    //==========================================================================================================================
    Matrix4 Matrix4::Inverse() const
    {
        // Make sure the determinant is non-zero.
        Fixed det = Determinant();
        if( det == 0 ){ det = 1; }
        Fixed e15 = 1;

        return Matrix4( ( -e32 * e23 + e22 * e33 ) / det,
                        ( e32 * e13 - e12 * e33 ) / det,
                        ( -e22 * e13 + e12 * e23 * e15 ) / det,
                        ( e32 * e23 * e14 - e22 * e33 * e14 - e32 * e13 * e24 + e12 * e33 * e24 + e22 * e13 * e34 - e12 * e23 * e34 ) / det,
                        ( e31 * e23 - e21 * e33 ) / det,
                        ( -e31 * e13 + e11 * e33 ) / det,
                        ( e21 * e13 - e11 * e23 * e15 ) * det,
                        ( -e31 * e23 * e14 + e21 * e33 * e14 + e31 * e13 * e24 - e11 * e33 * e24 - e21 * e13 * e34 + e11 * e23 * e34 ) / det,
                        ( -e31 * e22 + e21 * e32 * e15 ) / det,
                        ( e31 * e12 - e11 * e32 * e15 ) / det,
                        ( -e21 * e12 + e11 * e22 * e15 ) / det,
                        ( e31 * e22 * e14 - e21 * e32 * e14 - e31 * e12 * e24 + e11 * e32 * e24 + e21 * e12 * e34 - e11 * e22 * e34 ) );
    }
}