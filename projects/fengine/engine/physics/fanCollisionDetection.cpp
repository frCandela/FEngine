#include "engine/physics/fanCollisionDetection.hpp"

#include "engine/physics/fanFxPhysicsWorld.hpp"
#include "engine/physics/fanFxRigidbody.hpp"
#include "engine/components/fanFxTransform.hpp"
#include "engine/physics/fanFxSphereCollider.hpp"
#include "engine/physics/fanFxBoxCollider.hpp"
#include "core/math/fanMathUtils.hpp"
#include "engine/singletons/fanRenderDebug.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void CollisionDetection::SphereWithSphere( FxRigidbody& _rb0, FxSphereCollider& _sphere0, FxTransform& _transform0,
                                               FxRigidbody& _rb1, FxSphereCollider& _sphere1, FxTransform& _transform1, FxPhysicsWorld& _physicsWorld )
    {
        Vector3 p0 = _transform0.mPosition + _transform0.TransformDirection( _sphere0.mOffset );
        Vector3 p1 = _transform1.mPosition + _transform1.TransformDirection( _sphere1.mOffset );

        Vector3 midline  = p0 - p1;
        Fixed   distance = midline.Magnitude();
        if( Fixed::IsFuzzyZero( distance ) || distance > _sphere0.mRadius + _sphere1.mRadius ){ return; }

        Contact contact;
        contact.rigidbody[0] = &_rb0;
        contact.rigidbody[1] = &_rb1;
        contact.transform[0] = &_transform0;
        contact.transform[1] = &_transform1;
        contact.normal      = midline / distance;
        contact.penetration = ( _sphere0.mRadius + _sphere1.mRadius ) - distance;
        contact.position    = p1 + ( _sphere1.mRadius - FIXED( 0.5 ) * contact.penetration ) * contact.normal;
        contact.restitution = _physicsWorld.mRestitution;
        Vector3 tangent1, tangent2;
        Vector3::MakeOrthonormalBasis( contact.normal, tangent1, tangent2 );
        contact.contactToWorld = Matrix3( contact.normal, tangent1, tangent2 );
        contact.relativeContactPosition[0] = contact.position - _transform0.mPosition;
        contact.relativeContactPosition[1] = contact.position - _transform1.mPosition;

        _physicsWorld.mCollisionDetection.mContacts.push_back( contact );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void CollisionDetection::SphereWithPlane( FxRigidbody& _rigidbody, FxSphereCollider& _sphere, FxTransform& _transform, const Vector3& _normal, const Fixed _offset, FxPhysicsWorld& _physicsWorld )
    {
        Vector3 position    = _transform.mPosition + _transform.TransformDirection( _sphere.mOffset );
        Fixed   penetration = Vector3::Dot( position, _normal ) - _offset - _sphere.mRadius;
        if( penetration >= 0 ){ return; }

        Contact contact;
        contact.rigidbody[0] = &_rigidbody;
        contact.transform[0] = &_transform;
        contact.normal      = _normal;
        contact.penetration = -penetration;
        contact.position    = position - _sphere.mRadius * contact.normal;
        contact.restitution = _physicsWorld.mRestitution;
        Vector3 tangent0, tangent1;
        Vector3::MakeOrthonormalBasis( contact.normal, tangent0, tangent1 );
        contact.contactToWorld = Matrix3( contact.normal, tangent0, tangent1 );
        contact.relativeContactPosition[0] = contact.position - _transform.mPosition;

        _physicsWorld.mCollisionDetection.mContacts.push_back( contact );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void CollisionDetection::BoxWithPlane( FxRigidbody& _rigidbody, FxBoxCollider& _box, FxTransform& _transform, const Vector3& _normal, const Fixed _offset, FxPhysicsWorld& _physicsWorld )
    {
        Vector3  vertices[8] = {
                _rigidbody.mTransform * Vector3( _box.mHalfExtents.x, _box.mHalfExtents.y, _box.mHalfExtents.z ),
                _rigidbody.mTransform * Vector3( _box.mHalfExtents.x, _box.mHalfExtents.y, -_box.mHalfExtents.z ),
                _rigidbody.mTransform * Vector3( _box.mHalfExtents.x, -_box.mHalfExtents.y, _box.mHalfExtents.z ),
                _rigidbody.mTransform * Vector3( _box.mHalfExtents.x, -_box.mHalfExtents.y, -_box.mHalfExtents.z ),
                _rigidbody.mTransform * Vector3( -_box.mHalfExtents.x, _box.mHalfExtents.y, _box.mHalfExtents.z ),
                _rigidbody.mTransform * Vector3( -_box.mHalfExtents.x, _box.mHalfExtents.y, -_box.mHalfExtents.z ),
                _rigidbody.mTransform * Vector3( -_box.mHalfExtents.x, -_box.mHalfExtents.y, _box.mHalfExtents.z ),
                _rigidbody.mTransform * Vector3( -_box.mHalfExtents.x, -_box.mHalfExtents.y, -_box.mHalfExtents.z ),
        };
        for( int i           = 0; i < 8; i++ )
        {
            const Vector3& vertex = vertices[i];
            Fixed distance = Vector3::Dot( vertex, _normal );
            if( distance <= _offset )
            {
                Contact contact;
                contact.rigidbody[0] = &_rigidbody;
                contact.transform[0] = &_transform;
                contact.normal      = _normal;
                contact.penetration = _offset - distance;
                contact.position    = vertex;
                contact.restitution = _physicsWorld.mRestitution;
                Vector3 tangent0, tangent1;
                Vector3::MakeOrthonormalBasis( contact.normal, tangent0, tangent1 );
                contact.contactToWorld = Matrix3( contact.normal, tangent0, tangent1 );
                contact.relativeContactPosition[0] = contact.position - _transform.mPosition;
                _physicsWorld.mCollisionDetection.mContacts.push_back( contact );
            }
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void CollisionDetection::SphereWithBox( FxRigidbody& _rbSphere, FxSphereCollider& _sphere, FxTransform& _transformSphere,
                                            FxRigidbody& _rbBox, FxBoxCollider& _box, FxTransform& _transformBox, FxPhysicsWorld& _physicsWorld )
    {
        // calculates in box space
        Vector3 centerSphere         = _transformSphere.mPosition + _sphere.mOffset;
        Vector3 relativeCenterSphere = _rbBox.mTransform.InverseTransform( centerSphere );

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

        Vector3 closestPointWorld = _rbBox.mTransform * closestPoint;

        Contact contact;
        contact.rigidbody[0] = &_rbBox;
        contact.rigidbody[1] = &_rbSphere;
        contact.transform[0] = &_transformBox;
        contact.transform[1] = &_transformSphere;
        contact.normal      = ( closestPointWorld - centerSphere ).Normalized();
        contact.penetration = _sphere.mRadius - Fixed::Sqrt( sqrDistance );
        contact.position    = closestPointWorld;
        contact.restitution = _physicsWorld.mRestitution;
        Vector3 tangent1, tangent2;
        Vector3::MakeOrthonormalBasis( contact.normal, tangent1, tangent2 );
        contact.contactToWorld = Matrix3( contact.normal, tangent1, tangent2 );
        contact.relativeContactPosition[0] = contact.position - _transformBox.mPosition;
        contact.relativeContactPosition[1] = contact.position - _transformSphere.mPosition;

        _physicsWorld.mCollisionDetection.mContacts.push_back( contact );
    }

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
    bool BoxesOverlapOnAxis( const Vector3& _axis, const FxRigidbody& _rb0, const FxBoxCollider& _box0, const FxRigidbody& _rb1, const FxBoxCollider& _box1 )
    {
        const Fixed   projection0 = ProjectToAxis( _rb0.mTransform, _box0, _axis );
        const Fixed   projection1 = ProjectToAxis( _rb1.mTransform, _box1, _axis );
        const Vector3 toCenter    = _rb1.mTransform.GetOrigin() - _rb0.mTransform.GetOrigin();
        const Fixed   distance    = Fixed::Abs( Vector3::Dot( toCenter, _axis ) );
        return distance < projection0 + projection1;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool BoxWithPoint( FxRigidbody& _rb0, const FxBoxCollider& _box0, const Vector3& _point, Contact& _outContact )
    {
        const Vector3 relativePoint = _rb0.mTransform.InverseTransform( _point );

        Fixed minDepth = _box0.mHalfExtents.x - Fixed::Abs( relativePoint.x );
        if( minDepth < 0 ){ return false; }
        Vector3 normal = relativePoint.x > 0 ? _rb0.mTransform.GetX() : -_rb0.mTransform.GetX();

        Fixed depth = _box0.mHalfExtents.y - Fixed::Abs( relativePoint.y );
        if( depth < 0 ){ return false; }
        if( depth < minDepth )
        {
            minDepth = depth;
            normal   = relativePoint.y > 0 ? _rb0.mTransform.GetY() : -_rb0.mTransform.GetY();
        }

        depth = _box0.mHalfExtents.z - Fixed::Abs( relativePoint.z );
        if( depth < 0 ){ return false; }
        if( depth < minDepth )
        {
            minDepth = depth;
            normal   = relativePoint.z > 0 ? _rb0.mTransform.GetZ() : -_rb0.mTransform.GetZ();
        }

        _outContact.rigidbody[0] = &_rb0;
        //_outContact.transform[0] = &_transform;
        _outContact.normal      = normal;
        _outContact.penetration = depth;
        _outContact.position    = _point;
        Vector3 tangent0, tangent1;
        Vector3::MakeOrthonormalBasis( _outContact.normal, tangent0, tangent1 );
        _outContact.contactToWorld = Matrix3( _outContact.normal, tangent0, tangent1 );
        _outContact.relativeContactPosition[0] = _outContact.position - _rb0.mTransform.GetOrigin();
        return true;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void BoxVerticesWithBox( FxRigidbody& _rb0, FxBoxCollider& _box0, FxRigidbody& _rb1, FxBoxCollider& _box1, FxPhysicsWorld& _physicsWorld )
    {
        Vector3  vertices0[8] = {
                _rb0.mTransform * Vector3( _box0.mHalfExtents.x, _box0.mHalfExtents.y, _box0.mHalfExtents.z ),
                _rb0.mTransform * Vector3( _box0.mHalfExtents.x, _box0.mHalfExtents.y, -_box0.mHalfExtents.z ),
                _rb0.mTransform * Vector3( _box0.mHalfExtents.x, -_box0.mHalfExtents.y, _box0.mHalfExtents.z ),
                _rb0.mTransform * Vector3( _box0.mHalfExtents.x, -_box0.mHalfExtents.y, -_box0.mHalfExtents.z ),
                _rb0.mTransform * Vector3( -_box0.mHalfExtents.x, _box0.mHalfExtents.y, _box0.mHalfExtents.z ),
                _rb0.mTransform * Vector3( -_box0.mHalfExtents.x, _box0.mHalfExtents.y, -_box0.mHalfExtents.z ),
                _rb0.mTransform * Vector3( -_box0.mHalfExtents.x, -_box0.mHalfExtents.y, _box0.mHalfExtents.z ),
                _rb0.mTransform * Vector3( -_box0.mHalfExtents.x, -_box0.mHalfExtents.y, -_box0.mHalfExtents.z ),
        };
        for( int i            = 0; i < 8; ++i )
        {
            Contact contact;
            if( BoxWithPoint( _rb1, _box1, vertices0[i], contact ) )
            {
                _physicsWorld.mCollisionDetection.mContacts.push_back( contact );
            }
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void CollisionDetection::BoxWithBox( FxRigidbody& _rb0, FxBoxCollider& _box0, FxRigidbody& _rb1, FxBoxCollider& _box1, FxPhysicsWorld& _physicsWorld )
    {
        // early out test
        Vector3  axisList[15] = {
                _rb0.mTransform.GetX(),
                _rb0.mTransform.GetY(),
                _rb0.mTransform.GetZ(),
                _rb1.mTransform.GetX(),
                _rb1.mTransform.GetY(),
                _rb1.mTransform.GetZ(),
                Vector3::Cross( _rb0.mTransform.GetX(), _rb1.mTransform.GetX() ),
                Vector3::Cross( _rb0.mTransform.GetX(), _rb1.mTransform.GetY() ),
                Vector3::Cross( _rb0.mTransform.GetX(), _rb1.mTransform.GetZ() ),
                Vector3::Cross( _rb0.mTransform.GetY(), _rb1.mTransform.GetX() ),
                Vector3::Cross( _rb0.mTransform.GetY(), _rb1.mTransform.GetY() ),
                Vector3::Cross( _rb0.mTransform.GetY(), _rb1.mTransform.GetZ() ),
                Vector3::Cross( _rb0.mTransform.GetZ(), _rb1.mTransform.GetX() ),
                Vector3::Cross( _rb0.mTransform.GetZ(), _rb1.mTransform.GetY() ),
                Vector3::Cross( _rb0.mTransform.GetZ(), _rb1.mTransform.GetZ() ),
        };
        for( int i            = 0; i < 15; ++i )
        {
            if( !Vector3::IsFuzzyZero( axisList[i] ) && !BoxesOverlapOnAxis( axisList[i], _rb0, _box0, _rb1, _box1 ) )
            {
                return;
            }
        }
        BoxVerticesWithBox( _rb0, _box0, _rb1, _box1, _physicsWorld );
        BoxVerticesWithBox( _rb1, _box1, _rb0, _box0, _physicsWorld );
    }
}