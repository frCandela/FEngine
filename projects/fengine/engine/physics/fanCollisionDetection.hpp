#pragma once

#include <vector>
#include <unordered_set>
#include "engine/physics/fanContact.hpp"

namespace fan
{
    struct FxPhysicsWorld;
    struct FxRigidbody;
    struct FxSphereCollider;
    struct FxBoxCollider;
    struct RenderDebug;
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct CollisionDetection
    {
        std::vector<Contact> mContacts;

        static void SphereWithSphere( FxRigidbody& _rb0, FxSphereCollider& _sphere0, FxRigidbody& _rb1, FxSphereCollider& _sphere1, FxPhysicsWorld& _physicsWorld );
        static void BoxWithBox( FxRigidbody& _rb0, FxBoxCollider& _box0, FxRigidbody& _rb1, FxBoxCollider& _box1, FxPhysicsWorld& _physicsWorld );
        static void SphereWithBox( FxRigidbody& _rbSphere, FxSphereCollider& _sphere, FxRigidbody& _rbBox, FxBoxCollider& _box, FxPhysicsWorld& _physicsWorld );
        static void SphereWithPlane( FxRigidbody& _rigidbody, FxSphereCollider& _sphere, const Vector3& _normal, const Fixed _offset, FxPhysicsWorld& _physicsWorld );
        static void BoxWithPlane( FxRigidbody& _rigidbody, FxBoxCollider& _box, const Vector3& _normal, const Fixed _offset, FxPhysicsWorld& _physicsWorld );
    };
}