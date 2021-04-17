#include "engine/physics/fanCollisionDetection.hpp"

#include "engine/physics/fanFxPhysicsWorld.hpp"
#include "engine/physics/fanFxRigidbody.hpp"
#include "engine/components/fanFxTransform.hpp"
#include "engine/physics/fanFxSphereCollider.hpp"
#include "engine/physics/fanFxBoxCollider.hpp"
#include "core/math/fanMathUtils.hpp"

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
                _transform.TransformPoint( Vector3( _box.mHalfExtents.x, _box.mHalfExtents.y, _box.mHalfExtents.z ) ),
                _transform.TransformPoint( Vector3( _box.mHalfExtents.x, _box.mHalfExtents.y, -_box.mHalfExtents.z ) ),
                _transform.TransformPoint( Vector3( _box.mHalfExtents.x, -_box.mHalfExtents.y, _box.mHalfExtents.z ) ),
                _transform.TransformPoint( Vector3( _box.mHalfExtents.x, -_box.mHalfExtents.y, -_box.mHalfExtents.z ) ),
                _transform.TransformPoint( Vector3( -_box.mHalfExtents.x, _box.mHalfExtents.y, _box.mHalfExtents.z ) ),
                _transform.TransformPoint( Vector3( -_box.mHalfExtents.x, _box.mHalfExtents.y, -_box.mHalfExtents.z ) ),
                _transform.TransformPoint( Vector3( -_box.mHalfExtents.x, -_box.mHalfExtents.y, _box.mHalfExtents.z ) ),
                _transform.TransformPoint( Vector3( -_box.mHalfExtents.x, -_box.mHalfExtents.y, -_box.mHalfExtents.z ) ),
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
        Vector3 relativeCenterSphere = _transformBox.InverseTransformPoint( centerSphere );

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

        Fixed sqrDistance = ( relativeCenterSphere - closestPoint).SqrMagnitude();
        if( sqrDistance > _sphere.mRadius * _sphere.mRadius )
        {
            return;
        }

        Vector3 closestPointWorld = _transformBox.TransformPoint( closestPoint );

        Contact contact;
        contact.rigidbody[0] = &_rbBox;
        contact.rigidbody[1] = &_rbSphere;
        contact.transform[0] = &_transformBox;
        contact.transform[1] = &_transformSphere;
        contact.normal      = (closestPointWorld - centerSphere).Normalized();
        contact.penetration = _sphere.mRadius - Fixed::Sqrt(sqrDistance);
        contact.position    = closestPointWorld;
        contact.restitution = _physicsWorld.mRestitution;
        Vector3 tangent1, tangent2;
        Vector3::MakeOrthonormalBasis( contact.normal, tangent1, tangent2 );
        contact.contactToWorld = Matrix3( contact.normal, tangent1, tangent2 );
        contact.relativeContactPosition[0] = contact.position - _transformBox.mPosition;
        contact.relativeContactPosition[1] = contact.position - _transformSphere.mPosition;

        _physicsWorld.mCollisionDetection.mContacts.push_back( contact );
    }
}