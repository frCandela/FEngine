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
        contact.rb0         = &_rb0;
        contact.rb1         = &_rb1;
        contact.transform0  = &_transform0;
        contact.transform1  = &_transform1;
        contact.normal      = midline / distance;
        contact.penetration = ( _sphere0.mRadius + _sphere1.mRadius ) - distance;
        contact.position    = p1 + ( _sphere1.mRadius - FIXED( 0.5 ) * contact.penetration ) * contact.normal;
        contact.restitution = _physicsWorld.mRestitution;
        Vector3::MakeOrthonormalBasis( contact.normal, contact.tangents[0], contact.tangents[1] );
        contact.contactToWorld           = Matrix3( contact.normal, contact.tangents[0], contact.tangents[1] );
        contact.relativeContactPosition0 = contact.position - _transform0.mPosition;
        contact.relativeContactPosition1 = contact.position - _transform1.mPosition;

        _physicsWorld.mCollisionDetection.mContacts.push_back( contact );
        return true;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool CollisionDetection::SphereWithPlane( FxRigidbody& _rb, FxSphereCollider& _sphere, FxTransform& _transform,
                                              Vector3 _normal, Fixed _offset, FxPhysicsWorld& _physicsWorld )
    {
        Vector3 position = _transform.mPosition + _transform.TransformDirection( _sphere.mOffset );
        Fixed   penetration = Vector3::Dot( position, _normal ) - _offset - _sphere.mRadius;
        if( penetration >= 0 ) { return false; }

        Contact contact;
        contact.rb0         = &_rb;
        contact.transform0  = &_transform;
        contact.normal      = _normal;
        contact.penetration = -penetration;
        contact.position    = position - _sphere.mRadius  * contact.normal;
        contact.restitution = _physicsWorld.mRestitution;
        Vector3::MakeOrthonormalBasis( contact.normal, contact.tangents[0], contact.tangents[1] );
        contact.contactToWorld           = Matrix3( contact.normal, contact.tangents[0], contact.tangents[1] );
        contact.relativeContactPosition0 = contact.position - _transform.mPosition;

        float pen = contact.penetration.ToFloat(); (void)pen;
        glm::vec3 niorm = Math::ToGLM(contact.normal ); (void)niorm;
        glm::vec3 ppos =  Math::ToGLM(position ); (void)ppos;

        _physicsWorld.mCollisionDetection.mContacts.push_back( contact );
        return true;
    }
}