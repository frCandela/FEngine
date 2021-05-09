#include "engine/physics/fanCollisionDetection.hpp"

#include "core/time/fanProfiler.hpp"
#include "engine/physics/fanFxRigidbody.hpp"
#include "fanFxTransform.hpp"
#include "engine/physics/fanFxSphereCollider.hpp"
#include "engine/physics/fanFxBoxCollider.hpp"
#include "engine/singletons/fanRenderDebug.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void CollisionDetection::SphereWithSphere( FxRigidbody& _rb0, FxSphereCollider& _sphere0, FxRigidbody& _rb1, FxSphereCollider& _sphere1, std::vector<Contact>& _outContacts )
    {
        Vector3 p0 = _rb0.mTransform->mPosition + _rb0.mTransform->TransformDirection( _sphere0.mOffset );
        Vector3 p1 = _rb1.mTransform->mPosition + _rb1.mTransform->TransformDirection( _sphere1.mOffset );

        Vector3 midline  = p0 - p1;
        Fixed   distance = midline.Magnitude();
        if( Fixed::IsFuzzyZero( distance ) || distance > _sphere0.mRadius + _sphere1.mRadius ){ return; }

        Contact contact;
        contact.rigidbody[0] = &_rb0;
        contact.rigidbody[1] = &_rb1;
        contact.normal      = midline / distance;
        contact.penetration = ( _sphere0.mRadius + _sphere1.mRadius ) - distance;
        contact.position    = p1 + ( _sphere1.mRadius - FIXED( 0.5 ) * contact.penetration ) * contact.normal;
        _outContacts.push_back( contact );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void CollisionDetection::SphereWithPlane( FxRigidbody& _rigidbody, FxSphereCollider& _sphere, const Vector3& _normal, const Fixed _offset, std::vector<Contact>& _outContacts )
    {
        Vector3 position    = _rigidbody.mTransform->mPosition + _rigidbody.mTransform->TransformDirection( _sphere.mOffset );
        Fixed   penetration = Vector3::Dot( position, _normal ) - _offset - _sphere.mRadius;
        if( penetration >= 0 ){ return; }

        Contact contact;
        contact.rigidbody[0] = &_rigidbody;
        contact.normal      = _normal;
        contact.penetration = -penetration;
        contact.position    = position - _sphere.mRadius * contact.normal;
        _outContacts.push_back( contact );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void CollisionDetection::BoxWithPlane( FxRigidbody& _rigidbody, FxBoxCollider& _box, const Vector3& _normal, const Fixed _offset, std::vector<Contact>& _outContacts )
    {
        Vector3  vertices[8] = {
                ( *_rigidbody.mTransform ) * Vector3( _box.mHalfExtents.x, _box.mHalfExtents.y, _box.mHalfExtents.z ),
                ( *_rigidbody.mTransform ) * Vector3( _box.mHalfExtents.x, _box.mHalfExtents.y, -_box.mHalfExtents.z ),
                ( *_rigidbody.mTransform ) * Vector3( _box.mHalfExtents.x, -_box.mHalfExtents.y, _box.mHalfExtents.z ),
                ( *_rigidbody.mTransform ) * Vector3( _box.mHalfExtents.x, -_box.mHalfExtents.y, -_box.mHalfExtents.z ),
                ( *_rigidbody.mTransform ) * Vector3( -_box.mHalfExtents.x, _box.mHalfExtents.y, _box.mHalfExtents.z ),
                ( *_rigidbody.mTransform ) * Vector3( -_box.mHalfExtents.x, _box.mHalfExtents.y, -_box.mHalfExtents.z ),
                ( *_rigidbody.mTransform ) * Vector3( -_box.mHalfExtents.x, -_box.mHalfExtents.y, _box.mHalfExtents.z ),
                ( *_rigidbody.mTransform ) * Vector3( -_box.mHalfExtents.x, -_box.mHalfExtents.y, -_box.mHalfExtents.z ),
        };
        for( int i           = 0; i < 8; i++ )
        {
            const Vector3& vertex = vertices[i];
            Fixed distance = Vector3::Dot( vertex, _normal );
            if( distance <= _offset )
            {
                Contact contact;
                contact.rigidbody[0] = &_rigidbody;
                contact.normal      = _normal;
                contact.penetration = _offset - distance;
                contact.position    = vertex;
                _outContacts.push_back( contact );
            }
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void CollisionDetection::SphereWithBox( FxRigidbody& _rbSphere, FxSphereCollider& _sphere,
                                            FxRigidbody& _rbBox, FxBoxCollider& _box, std::vector<Contact>& _outContacts )
    {
        // calculates in box space
        Vector3 centerSphere         = _rbSphere.mTransform->mPosition + _sphere.mOffset;
        Vector3 relativeCenterSphere = _rbBox.mTransform->InverseTransformPoint( centerSphere );

        // early out bounds
        if( Fixed::Abs( relativeCenterSphere.x ) - _sphere.mRadius > _box.mHalfExtents.x ||
            Fixed::Abs( relativeCenterSphere.y ) - _sphere.mRadius > _box.mHalfExtents.y ||
            Fixed::Abs( relativeCenterSphere.z ) - _sphere.mRadius > _box.mHalfExtents.z )
        {
            return;
        }

        // finds the closest point on the box
        Vector3 closestPoint = relativeCenterSphere;
        if( relativeCenterSphere.x > _box.mHalfExtents.x ){ closestPoint.x = _box.mHalfExtents.x; }
        else if( relativeCenterSphere.x < -_box.mHalfExtents.x ){ closestPoint.x = -_box.mHalfExtents.x; }

        if( relativeCenterSphere.y > _box.mHalfExtents.y ){ closestPoint.y = _box.mHalfExtents.y; }
        else if( relativeCenterSphere.y < -_box.mHalfExtents.y ){ closestPoint.y = -_box.mHalfExtents.y; }

        if( relativeCenterSphere.z > _box.mHalfExtents.z ){ closestPoint.z = _box.mHalfExtents.z; }
        else if( relativeCenterSphere.z < -_box.mHalfExtents.z ){ closestPoint.z = -_box.mHalfExtents.z; }

        Fixed sqrDistance = ( relativeCenterSphere - closestPoint ).SqrMagnitude();
        if( sqrDistance > _sphere.mRadius * _sphere.mRadius )
        {
            return;
        }

        const Vector3 closestPointWorld = ( *_rbBox.mTransform ) * closestPoint;
        const Vector3 normal            = ( closestPointWorld - centerSphere ).Normalized();
        if( Fixed::IsFuzzyZero( normal.SqrMagnitude() - 1 ) )
        {
            Contact contact;
            contact.rigidbody[0] = &_rbBox;
            contact.rigidbody[1] = &_rbSphere;
            contact.normal      = normal;
            contact.penetration = _sphere.mRadius - Fixed::Sqrt( sqrDistance );
            contact.position    = closestPointWorld;
            _outContacts.push_back( contact );
        }
    }
    RenderDebug* tmprd;
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    Fixed ProjectToAxis( const Matrix4& _transform, const FxBoxCollider& _box, const Vector3& _axis )
    {
        return _box.mHalfExtents.x * Fixed::Abs( Vector3::Dot( _transform.GetX(), _axis ) ) +
               _box.mHalfExtents.y * Fixed::Abs( Vector3::Dot( _transform.GetY(), _axis ) ) +
               _box.mHalfExtents.z * Fixed::Abs( Vector3::Dot( _transform.GetZ(), _axis ) );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool BoxesOverlapOnAxis( const Vector3& _axis, const Matrix4& _boxTransform0, const FxBoxCollider& _box0, const Matrix4& _boxTransform1, const FxBoxCollider& _box1 )
    {
        const Fixed   projection0 = ProjectToAxis( _boxTransform0, _box0, _axis );
        const Fixed   projection1 = ProjectToAxis( _boxTransform1, _box1, _axis );
        const Vector3 toCenter    = _boxTransform1.GetOrigin() - _boxTransform0.GetOrigin();
        const Fixed   distance    = Fixed::Abs( Vector3::Dot( toCenter, _axis ) );
        return distance < projection0 + projection1;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool BoxWithPoint( FxRigidbody& _rb0, const FxBoxCollider& _box0, Matrix4& _transform0, FxRigidbody& _rb1, const Vector3& _point1, Contact& _outContact )
    {
        const Vector3 relativePoint = _rb0.mTransform->InverseTransformPoint( _point1 );

        Fixed minDepth = _box0.mHalfExtents.x - Fixed::Abs( relativePoint.x );
        if( minDepth < 0 ){ return false; }
        Vector3 normal = relativePoint.x > 0 ? _transform0.GetX() : -_transform0.GetX();

        Fixed depth = _box0.mHalfExtents.y - Fixed::Abs( relativePoint.y );
        if( depth < 0 ){ return false; }
        if( depth < minDepth )
        {
            minDepth = depth;
            normal   = relativePoint.y > 0 ? _transform0.GetY() : -_transform0.GetY();
        }

        depth = _box0.mHalfExtents.z - Fixed::Abs( relativePoint.z );
        if( depth < 0 ){ return false; }
        if( depth < minDepth )
        {
            minDepth = depth;
            normal   = relativePoint.z > 0 ? _transform0.GetZ() : -_transform0.GetZ();
        }

        _outContact.rigidbody[0] = &_rb0;
        _outContact.rigidbody[1] = &_rb1;
        _outContact.normal      = -normal;
        _outContact.penetration = minDepth;
        _outContact.position    = _point1;

        // tmprd->DebugPoint( _outContact.position, Color::sRed );
        //tmprd->DebugLine( _outContact.position, _outContact.position + _outContact.relativeContactPosition[0], Color::sGreen );
        //tmprd->DebugLine( _outContact.position, _outContact.position + _outContact.penetration * _outContact.normal, Color::sRed );

        return true;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void BoxVerticesWithBox( FxRigidbody& _rb0, FxBoxCollider& _box0, Matrix4& _transform0, FxRigidbody& _rb1, FxBoxCollider& _box1, Matrix4& _transform1, std::vector<Contact>& _outContacts )
    {
        Vector3  vertices0[8] = {
                _transform0 * Vector3( _box0.mHalfExtents.x, _box0.mHalfExtents.y, _box0.mHalfExtents.z ),
                _transform0 * Vector3( _box0.mHalfExtents.x, _box0.mHalfExtents.y, -_box0.mHalfExtents.z ),
                _transform0 * Vector3( _box0.mHalfExtents.x, -_box0.mHalfExtents.y, _box0.mHalfExtents.z ),
                _transform0 * Vector3( _box0.mHalfExtents.x, -_box0.mHalfExtents.y, -_box0.mHalfExtents.z ),
                _transform0 * Vector3( -_box0.mHalfExtents.x, _box0.mHalfExtents.y, _box0.mHalfExtents.z ),
                _transform0 * Vector3( -_box0.mHalfExtents.x, _box0.mHalfExtents.y, -_box0.mHalfExtents.z ),
                _transform0 * Vector3( -_box0.mHalfExtents.x, -_box0.mHalfExtents.y, _box0.mHalfExtents.z ),
                _transform0 * Vector3( -_box0.mHalfExtents.x, -_box0.mHalfExtents.y, -_box0.mHalfExtents.z ),
        };
        for( int i            = 0; i < 8; ++i )
        {
            Contact contact;
            if( BoxWithPoint( _rb1, _box1, _transform1, _rb0, vertices0[i], contact ) )
            {
                _outContacts.push_back( contact );
            }
        }
    }

    RenderDebug* CollisionDetection::tmpRd;

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void CollisionDetection::BoxWithBox( FxRigidbody& _rb0, FxBoxCollider& _box0, FxRigidbody& _rb1, FxBoxCollider& _box1, std::vector<Contact>& _outContacts )
    {
        SCOPED_PROFILE( box_with_box )

        tmprd                 = tmpRd;

        // early out test
        Matrix4  transform0( _rb0.mTransform->mRotation, _rb0.mTransform->mPosition );
        Matrix4  transform1( _rb1.mTransform->mRotation, _rb1.mTransform->mPosition );
        Vector3  axisList[15] = {
                transform0.GetX(),
                transform0.GetY(),
                transform0.GetZ(),
                transform1.GetX(),
                transform1.GetY(),
                transform1.GetZ(),
                Vector3::Cross( transform0.GetX(), transform1.GetX() ),
                Vector3::Cross( transform0.GetX(), transform1.GetY() ),
                Vector3::Cross( transform0.GetX(), transform1.GetZ() ),
                Vector3::Cross( transform0.GetY(), transform1.GetX() ),
                Vector3::Cross( transform0.GetY(), transform1.GetY() ),
                Vector3::Cross( transform0.GetY(), transform1.GetZ() ),
                Vector3::Cross( transform0.GetZ(), transform1.GetX() ),
                Vector3::Cross( transform0.GetZ(), transform1.GetY() ),
                Vector3::Cross( transform0.GetZ(), transform1.GetZ() ),
        };
        for( int i            = 0; i < 15; ++i )
        {
            if( !Vector3::IsFuzzyZero( axisList[i] ) && !BoxesOverlapOnAxis( axisList[i], transform0, _box0, transform1, _box1 ) )
            {
                return;
            }
        }

        BoxVerticesWithBox( _rb0, _box0, transform0, _rb1, _box1, transform1, _outContacts );
        BoxVerticesWithBox( _rb1, _box1, transform0, _rb0, _box0, transform1, _outContacts );

        struct Edge
        {
            Vector3 point;
            Vector3 dir;
            Fixed   lenght;
        };

        Edge edges0[12] = {
                { Vector3( _box0.mHalfExtents.x, _box0.mHalfExtents.y, _box0.mHalfExtents.z ),    Vector3( -1, 0, 0 ), 2 * _box0.mHalfExtents.x },
                { Vector3( _box0.mHalfExtents.x, _box0.mHalfExtents.y, _box0.mHalfExtents.z ),    Vector3( 0, -1, 0 ), 2 * _box0.mHalfExtents.y },
                { Vector3( _box0.mHalfExtents.x, _box0.mHalfExtents.y, _box0.mHalfExtents.z ),    Vector3( 0, 0, -1 ), 2 * _box0.mHalfExtents.z },
                { Vector3( -_box0.mHalfExtents.x, -_box0.mHalfExtents.y, -_box0.mHalfExtents.z ), Vector3( 1, 0, 0 ),  2 * _box0.mHalfExtents.x },
                { Vector3( -_box0.mHalfExtents.x, -_box0.mHalfExtents.y, -_box0.mHalfExtents.z ), Vector3( 0, 1, 0 ),  2 * _box0.mHalfExtents.y },
                { Vector3( -_box0.mHalfExtents.x, -_box0.mHalfExtents.y, -_box0.mHalfExtents.z ), Vector3( 0, 0, 1 ),  2 * _box0.mHalfExtents.z },
                { Vector3( -_box0.mHalfExtents.x, _box0.mHalfExtents.y, _box0.mHalfExtents.z ),   Vector3( 0, 0, -1 ), 2 * _box0.mHalfExtents.z },
                { Vector3( -_box0.mHalfExtents.x, -_box0.mHalfExtents.y, _box0.mHalfExtents.z ),  Vector3( 0, 1, 0 ),  2 * _box0.mHalfExtents.y },
                { Vector3( -_box0.mHalfExtents.x, -_box0.mHalfExtents.y, _box0.mHalfExtents.z ),  Vector3( 1, 0, 0 ),  2 * _box0.mHalfExtents.x },
                { Vector3( _box0.mHalfExtents.x, -_box0.mHalfExtents.y, _box0.mHalfExtents.z ),   Vector3( 0, 0, -1 ), 2 * _box0.mHalfExtents.z },
                { Vector3( _box0.mHalfExtents.x, _box0.mHalfExtents.y, -_box0.mHalfExtents.z ),   Vector3( -1, 0, 0 ), 2 * _box0.mHalfExtents.x },
                { Vector3( _box0.mHalfExtents.x, _box0.mHalfExtents.y, -_box0.mHalfExtents.z ),   Vector3( 0, -1, 0 ), 2 * _box0.mHalfExtents.y },
        };

        Edge edges1[12] = {
                { Vector3( _box1.mHalfExtents.x, _box1.mHalfExtents.y, _box1.mHalfExtents.z ),    Vector3( -1, 0, 0 ), 2 * _box1.mHalfExtents.x },
                { Vector3( _box1.mHalfExtents.x, _box1.mHalfExtents.y, _box1.mHalfExtents.z ),    Vector3( 0, -1, 0 ), 2 * _box1.mHalfExtents.y },
                { Vector3( _box1.mHalfExtents.x, _box1.mHalfExtents.y, _box1.mHalfExtents.z ),    Vector3( 0, 0, -1 ), 2 * _box1.mHalfExtents.z },
                { Vector3( -_box1.mHalfExtents.x, -_box1.mHalfExtents.y, -_box1.mHalfExtents.z ), Vector3( 1, 0, 0 ),  2 * _box1.mHalfExtents.x },
                { Vector3( -_box1.mHalfExtents.x, -_box1.mHalfExtents.y, -_box1.mHalfExtents.z ), Vector3( 0, 1, 0 ),  2 * _box1.mHalfExtents.y },
                { Vector3( -_box1.mHalfExtents.x, -_box1.mHalfExtents.y, -_box1.mHalfExtents.z ), Vector3( 0, 0, 1 ),  2 * _box1.mHalfExtents.z },
                { Vector3( -_box1.mHalfExtents.x, _box1.mHalfExtents.y, _box1.mHalfExtents.z ),   Vector3( 0, 0, -1 ), 2 * _box1.mHalfExtents.z },
                { Vector3( -_box1.mHalfExtents.x, -_box1.mHalfExtents.y, _box1.mHalfExtents.z ),  Vector3( 0, 1, 0 ),  2 * _box1.mHalfExtents.y },
                { Vector3( -_box1.mHalfExtents.x, -_box1.mHalfExtents.y, _box1.mHalfExtents.z ),  Vector3( 1, 0, 0 ),  2 * _box1.mHalfExtents.x },
                { Vector3( _box1.mHalfExtents.x, -_box1.mHalfExtents.y, _box1.mHalfExtents.z ),   Vector3( 0, 0, -1 ), 2 * _box1.mHalfExtents.z },
                { Vector3( _box1.mHalfExtents.x, _box1.mHalfExtents.y, -_box1.mHalfExtents.z ),   Vector3( -1, 0, 0 ), 2 * _box1.mHalfExtents.x },
                { Vector3( _box1.mHalfExtents.x, _box1.mHalfExtents.y, -_box1.mHalfExtents.z ),   Vector3( 0, -1, 0 ), 2 * _box1.mHalfExtents.y },
        };

        for( int i = 0; i < 12; i++ )
        {
            edges0[i].point = _rb0.mTransform->TransformPoint( edges0[i].point );
            edges0[i].dir   = _rb0.mTransform->TransformDirection( edges0[i].dir );
            edges1[i].point = _rb1.mTransform->TransformPoint( edges1[i].point );
            edges1[i].dir   = _rb1.mTransform->TransformDirection( edges1[i].dir );
        }

        for( int i = 0; i < 12; i++ )
        {
            Edge    e0 = edges0[i];
            Contact deepestContact;
            deepestContact.penetration = -1;
            for( int j = 0; j < 12; j++ )
            {
                Edge e1 = edges1[j];
                e0.dir.Normalize();
                e1.dir.Normalize();

                const Vector3 n  = Vector3::Cross( e0.dir, e1.dir );
                //const Vector3 n2 = Vector3::Cross( e1.dir, n);
                //const Vector3 c1 = e0.point + Vector3::Dot( ( e1.point - e0.point), n2) / Vector3::Dot( e0.dir, n2) * e0.dir;
                const Vector3 n1 = Vector3::Cross( e0.dir, n );
                Vector3       contactPoint;
                if( Vector3::IsFuzzyZero( n1 ) )
                {
                    // edge case parallel lines
                    Contact contact;
                    if( BoxWithPoint( _rb0, _box0, transform0, _rb1, e1.point, contact ) && contact.penetration > deepestContact.penetration )
                    {
                        deepestContact = contact;
                    }
                    if( BoxWithPoint( _rb0, _box0, transform0, _rb1, e1.point + e1.lenght * e1.dir, contact ) && contact.penetration > deepestContact.penetration )
                    {
                        deepestContact = contact;
                    }
                }
                else
                {
                    Fixed dot = Vector3::Dot( e1.dir, n1 );
                    if( dot == 0 )
                    {
                        continue;
                    }
                    contactPoint = e1.point + Vector3::Dot( ( e0.point - e1.point ), n1 ) / dot * e1.dir;
                    Fixed projection = Vector3::Dot( contactPoint - e1.point, e1.dir );
                    if( projection > 0 && projection < e1.lenght )
                    {
                        Contact contact;
                        if( BoxWithPoint( _rb0, _box0, transform0, _rb1, contactPoint, contact ) && contact.penetration > deepestContact.penetration )
                        {
                            deepestContact = contact;
                        }
                    }
                }
            }
            if( deepestContact.penetration > 0 )
            {
                _outContacts.push_back( deepestContact );
                // tmpRd->DebugPoint(deepestContact.position , Color::sRed);
            }
        }
    }
}