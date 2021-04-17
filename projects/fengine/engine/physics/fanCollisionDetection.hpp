#pragma once

#include <vector>
#include <unordered_set>
#include "engine/physics/fanContact.hpp"

namespace fan
{
    struct FxPhysicsWorld;
    struct FxTransform;
    struct FxRigidbody;
    struct FxSphereCollider;
    struct FxBoxCollider;

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct CollisionDetection
    {
        std::vector<Contact> mContacts;

        static void SphereWithSphere( FxRigidbody& _rb0, FxSphereCollider& _sphere0, FxTransform& _transform0,
                                      FxRigidbody& _rb1, FxSphereCollider& _sphere1, FxTransform& _transform1, FxPhysicsWorld& _physicsWorld );
        static void SphereWithBox( FxRigidbody& _rbSphere, FxSphereCollider& _sphere, FxTransform& _transformSphere,
                                   FxRigidbody& _rbBox, FxBoxCollider& _box, FxTransform& _transformBox, FxPhysicsWorld& _physicsWorld );
        static void SphereWithPlane( FxRigidbody& _rigidbody, FxSphereCollider& _sphere, FxTransform& _transform, const Vector3& _normal, const Fixed _offset, FxPhysicsWorld& _physicsWorld );
        static void BoxWithPlane( FxRigidbody& _rigidbody, FxBoxCollider& _box, FxTransform& _transform, const Vector3& _normal, const Fixed _offset, FxPhysicsWorld& _physicsWorld );
    };
}