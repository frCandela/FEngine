#include "core/shapes/fanRay.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    Ray::Ray() : origin( Vector3::sZero ), direction( Vector3::sForward ) {}

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    Ray::Ray( const Vector3& _origin, const Vector3& _direction ) : origin( _origin ), direction( _direction ) {}

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Ray::RayClosestPoints( const Ray _ray, Vector3& _outIntersectionRay, Vector3& _outIntersectionOtherRay ) const
    {
        const Vector3 w0 = origin - _ray.origin;
        const Fixed   a  = Vector3::Dot( direction, direction );
        const Fixed   b  = Vector3::Dot( direction, _ray.direction );
        const Fixed   c  = Vector3::Dot( _ray.direction, _ray.direction );
        const Fixed   d  = Vector3::Dot( direction, w0 );
        const Fixed   e  = Vector3::Dot( _ray.direction, w0 );
        const Fixed   s  = ( b * e - c * d ) / ( a * c - b * b );
        const Fixed   t  = ( a * e - b * d ) / ( a * c - b * b );
        _outIntersectionRay      = origin + s * direction;
        _outIntersectionOtherRay = _ray.origin + t * _ray.direction;
    }
}
