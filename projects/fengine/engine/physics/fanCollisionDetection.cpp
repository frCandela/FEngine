#include "engine/physics/fanCollisionDetection.hpp"

#include "engine/physics/fanFxPhysicsWorld.hpp"
#include "engine/physics/fanFxRigidbody.hpp"
#include "engine/components/fanFxTransform.hpp"
#include "engine/physics/fanFxSphereCollider.hpp"
#include "core/math/fanMathUtils.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool CollisionDetection::SphereWithSphere( FxRigidbody& _rb0, FxSphereCollider& _sphere0, FxTransform& _transform0,
                                               FxRigidbody& _rb1, FxSphereCollider& _sphere1, FxTransform& _transform1, FxPhysicsWorld& _physicsWorld )
    {
        Vector3 p0 = _transform0.mPosition + _transform0.TransformDirection( _sphere0.mOffset );
        Vector3 p1 = _transform1.mPosition + _transform1.TransformDirection( _sphere1.mOffset );

        Vector3 midline  = p0 - p1;
        Fixed   distance = midline.Magnitude();
        if( Fixed::IsFuzzyZero( distance ) || distance > _sphere0.mRadius + _sphere1.mRadius ){ return false; }

        Contact contact;
        contact.rigidbody[0] = &_rb0;
        contact.rigidbody[1] = &_rb1;
        contact.transform[0]  = &_transform0;
        contact.transform[1]  = &_transform1;
        contact.normal      = midline / distance;
        contact.penetration = ( _sphere0.mRadius + _sphere1.mRadius ) - distance;
        contact.position    = p1 + ( _sphere1.mRadius - FIXED( 0.5 ) * contact.penetration ) * contact.normal;
        contact.restitution = _physicsWorld.mRestitution;
        Vector3 tangent1, tangent2;
        Vector3::MakeOrthonormalBasis( contact.normal, tangent1, tangent2 );
        contact.contactToWorld           = Matrix3( contact.normal, tangent1, tangent2 );
        contact.relativeContactPosition[0] = contact.position - _transform0.mPosition;
        contact.relativeContactPosition[1] = contact.position - _transform1.mPosition;

        _physicsWorld.mCollisionDetection.mContacts.push_back( contact );
        return true;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool CollisionDetection::SphereWithPlane( FxRigidbody& _rb, FxSphereCollider& _sphere, FxTransform& _transform,
                                              Vector3 _normal, Fixed _offset, FxPhysicsWorld& _physicsWorld )
    {
        Vector3 position    = _transform.mPosition + _transform.TransformDirection( _sphere.mOffset );
        Fixed   penetration = Vector3::Dot( position, _normal ) - _offset - _sphere.mRadius;
        if( penetration >= 0 ){ return false; }

        Contact contact;
        contact.rigidbody[0] = &_rb;
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
        return true;
    }
}