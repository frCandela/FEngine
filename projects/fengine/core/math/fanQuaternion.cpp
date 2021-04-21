#include <core/fanAssert.hpp>
#include "core/math/fanQuaternion.hpp"

namespace fan
{
    const Quaternion Quaternion::sIdentity = Quaternion();

    //==========================================================================================================================
    //==========================================================================================================================
    Quaternion Quaternion::operator*( const Quaternion& _quat ) const
    {
        return Quaternion( mAngle * _quat.mAngle - mAxis.x * _quat.mAxis.x - mAxis.y * _quat.mAxis.y - mAxis.z * _quat.mAxis.z,
                           mAngle * _quat.mAxis.x + mAxis.x * _quat.mAngle + mAxis.y * _quat.mAxis.z - mAxis.z * _quat.mAxis.y,
                           mAngle * _quat.mAxis.y + mAxis.y * _quat.mAngle + mAxis.z * _quat.mAxis.x - mAxis.x * _quat.mAxis.z,
                           mAngle * _quat.mAxis.z + mAxis.z * _quat.mAngle + mAxis.x * _quat.mAxis.y - mAxis.y * _quat.mAxis.x );
    }

    //==========================================================================================================================
    //==========================================================================================================================
    Quaternion Quaternion::LookRotation( const Vector3& _direction, const Vector3 _up )
    {
        Vector3 forward = _direction.Normalized();
        Vector3 left    = Vector3::Cross( _up, forward ).Normalized();

        if( Vector3::IsFuzzyZero( left ) ) // _up collinear to _direction
        {
            Vector3 newUp( _up.z, -_up.x, _up.y ); // get random up vector
            left = Vector3::Cross( newUp, forward ).Normalized();
        }
        Vector3 up      = Vector3::Cross( forward, left );

        Fixed n = 1 + left.x + up.y + forward.z;
        if( Fixed::IsFuzzyZero( n ) ) // edge case, flip the base
        {
            left *= -1;
            up *= -1;
            n = 1 + left.x + up.y + forward.z;
        }

        if( Fixed::IsFuzzyZero( n ) )
        {
            Vector3::OrthoNormalize( forward, up );
            return Quaternion( 0, up ); // 180 rotation around up
        }

        fanAssert( n > 0 );

        Quaternion q;
        q.mAngle = Fixed::Sqrt( n ) / 2;
        Fixed w4 = q.mAngle * Fixed( 4 );
        q.mAxis.x = ( up.z - forward.y ) / w4;
        q.mAxis.y = ( forward.x - left.z ) / w4;
        q.mAxis.z = ( left.y - up.x ) / w4;

        return q.Normalized();
    }

    //==========================================================================================================================
    //==========================================================================================================================
    Quaternion Quaternion::Multiply( const Quaternion& q, const Vector3& v )
    {
        return Quaternion( -( q.mAxis.x * v.x + q.mAxis.y * v.y + q.mAxis.z * v.z ),
                           q.mAngle * v.x + q.mAxis.y * v.z - q.mAxis.z * v.y,
                           q.mAngle * v.y + q.mAxis.z * v.x - q.mAxis.x * v.z,
                           q.mAngle * v.z + q.mAxis.x * v.y - q.mAxis.y * v.x );
    }

    //==========================================================================================================================
    //==========================================================================================================================
    Quaternion Quaternion::Multiply( const Vector3& v, const Quaternion& q )
    {
        return Quaternion( -( q.mAxis.x * v.x + q.mAxis.y * v.y + q.mAxis.z * v.z ),
                           q.mAngle * v.x + q.mAxis.z * v.y - q.mAxis.y * v.z,
                           q.mAngle * v.y + q.mAxis.x * v.z - q.mAxis.z * v.x,
                           q.mAngle * v.z + q.mAxis.y * v.x - q.mAxis.x * v.y );
    }

    //==========================================================================================================================
    //==========================================================================================================================
    Quaternion Quaternion::Normalized() const
    {
        Fixed m = Magnitude();
        return Quaternion( mAngle / m, mAxis / m );
    }

    //==========================================================================================================================
    //==========================================================================================================================
    Vector3 Quaternion::Euler() const
    {
        Fixed   r11, r21, r31, r32, r33, r12, r13;
        Fixed   q00, q11, q22, q33;
        Fixed   tmp;
        Vector3 u;
        q00 = mAngle * mAngle;
        q11 = mAxis.x * mAxis.x;
        q22 = mAxis.y * mAxis.y;
        q33 = mAxis.z * mAxis.z;
        r11 = q00 + q11 - q22 - q33;
        r21 = Fixed( 2 ) * ( mAxis.x * mAxis.y + mAngle * mAxis.z );
        r31 = Fixed( 2 ) * ( mAxis.x * mAxis.z - mAngle * mAxis.y );
        r32 = Fixed( 2 ) * ( mAxis.y * mAxis.z + mAngle * mAxis.x );
        r33 = q00 - q11 - q22 + q33;
        tmp = Fixed::Abs( r31 );
        tmp = tmp == 0 ? 1 : tmp;
        if( tmp > FIXED( 0.999999 ) )
        {
            r12 = Fixed( 2 ) * ( mAxis.x * mAxis.y - mAngle * mAxis.z );
            r13 = Fixed( 2 ) * ( mAxis.x * mAxis.z + mAngle * mAxis.y );
            u.x = Fixed::Degrees( 0 ); //roll
            u.y = Fixed::Degrees( ( -( FX_PI / 2 ) * r31 / tmp ) );   // pitch
            u.z = Fixed::Degrees( Fixed::ATan2( -r12, -r31 * r13 ) ); // yaw
            return u;
        }
        u.x = Fixed::Degrees( Fixed::ATan2( r32, r33 ) ); // roll
        u.y = Fixed::Degrees( Fixed::ASin( -r31 ) );      // pitch
        u.z = Fixed::Degrees( Fixed::ATan2( r21, r11 ) ); // yaw
        return u;
    }

    //==========================================================================================================================
    //==========================================================================================================================
    Quaternion Quaternion::AngleAxis( const Fixed _degrees, const Vector3& _axis )
    {
        return AngleAxisRadians( Fixed::Radians( _degrees ), _axis );
    }

    //==========================================================================================================================
    //==========================================================================================================================
    Quaternion Quaternion::AngleAxisRadians( const Fixed _radians, const Vector3& _axis )
    {
        const Fixed halfRadians = _radians / 2;
        Quaternion  q;
        q.mAngle = Fixed::Cos( halfRadians );
        q.mAxis  = Fixed::Sin( halfRadians ) * _axis.Normalized();
        return q;
    }

    //==========================================================================================================================
    //==========================================================================================================================
    Quaternion Quaternion::Euler( const Vector3& _degrees )
    {
        Quaternion q;
        Fixed      roll  = Fixed::Radians( _degrees.x );
        Fixed      pitch = Fixed::Radians( _degrees.y );
        Fixed      yaw   = Fixed::Radians( _degrees.z );
        Fixed      cyaw, cpitch, croll, syaw, spitch, sroll;
        Fixed      cyawcpitch, syawspitch, cyawspitch, syawcpitch;
        cyaw       = Fixed::Cos( FIXED( .5 ) * yaw );
        cpitch     = Fixed::Cos( FIXED( 0.5 ) * pitch );
        croll      = Fixed::Cos( FIXED( 0.5 ) * roll );
        syaw       = Fixed::Sin( FIXED( 0.5 ) * yaw );
        spitch     = Fixed::Sin( FIXED( 0.5 ) * pitch );
        sroll      = Fixed::Sin( FIXED( 0.5 ) * roll );
        cyawcpitch = cyaw * cpitch;
        syawspitch = syaw * spitch;
        cyawspitch = cyaw * spitch;
        syawcpitch = syaw * cpitch;
        q.mAngle  = cyawcpitch * croll + syawspitch * sroll;
        q.mAxis.x = cyawcpitch * sroll - syawspitch * croll;
        q.mAxis.y = cyawspitch * croll + syawcpitch * sroll;
        q.mAxis.z = syawcpitch * croll - cyawspitch * sroll;
        return q;
    }
}