#include "core/math/fanQuaternion.hpp"

namespace fan
{
    const Quaternion Quaternion::sIdentity = Quaternion();

    //==========================================================================================================================
    //==========================================================================================================================
    Quaternion Quaternion::operator*( const Quaternion& _quat ) const
    {
        return Quaternion( n * _quat.n - v.x * _quat.v.x - v.y * _quat.v.y - v.z * _quat.v.z,
                           n * _quat.v.x + v.x * _quat.n + v.y * _quat.v.z - v.z * _quat.v.y,
                           n * _quat.v.y + v.y * _quat.n + v.z * _quat.v.x - v.x * _quat.v.z,
                           n * _quat.v.z + v.z * _quat.n + v.x * _quat.v.y - v.y * _quat.v.x );
    }

    //==========================================================================================================================
    //==========================================================================================================================
    Quaternion Quaternion::Multiply( const Quaternion& q, const Vector3& v )
    {
        return Quaternion( -( q.v.x * v.x + q.v.y * v.y + q.v.z * v.z ),
                           q.n * v.x + q.v.y * v.z - q.v.z * v.y,
                           q.n * v.y + q.v.z * v.x - q.v.x * v.z,
                           q.n * v.z + q.v.x * v.y - q.v.y * v.x );
    }

    //==========================================================================================================================
    //==========================================================================================================================
    Quaternion Quaternion::Multiply( const Vector3& v, const Quaternion& q )
    {
        return Quaternion( -( q.v.x * v.x + q.v.y * v.y + q.v.z * v.z ),
                           q.n * v.x + q.v.z * v.y - q.v.y * v.z,
                           q.n * v.y + q.v.x * v.z - q.v.z * v.x,
                           q.n * v.z + q.v.y * v.x - q.v.x * v.y );
    }

    //==========================================================================================================================
    //==========================================================================================================================
    Vector3 Quaternion::Euler() const
    {
        Fixed   r11, r21, r31, r32, r33, r12, r13;
        Fixed   q00, q11, q22, q33;
        Fixed   tmp;
        Vector3 u;
        q00 = n * n;
        q11 = v.x * v.x;
        q22 = v.y * v.y;
        q33 = v.z * v.z;
        r11 = q00 + q11 - q22 - q33;
        r21 = Fixed( 2 ) * ( v.x * v.y + n * v.z );
        r31 = Fixed( 2 ) * ( v.x * v.z - n * v.y );
        r32 = Fixed( 2 ) * ( v.y * v.z + n * v.x );
        r33 = q00 - q11 - q22 + q33;
        tmp = Fixed::Abs( r31 );
        if( tmp > FIXED( 0.999999 ) )
        {
            r12 = Fixed( 2 ) * ( v.x * v.y - n * v.z );
            r13 = Fixed( 2 ) * ( v.x * v.z + n * v.y );
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
        const Fixed halfRadians = Fixed::Radians( _degrees ) / 2;
        Quaternion  q;
        q.n = Fixed::Cos( halfRadians );
        q.v = Fixed::Sin( halfRadians ) * _axis.Normalized();
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
        q.n   = cyawcpitch * croll + syawspitch * sroll;
        q.v.x = cyawcpitch * sroll - syawspitch * croll;
        q.v.y = cyawspitch * croll + syawcpitch * sroll;
        q.v.z = syawcpitch * croll - cyawspitch * sroll;
        return q;
    }
}