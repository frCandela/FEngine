#pragma once

#include <vector>
#include <unordered_set>
#include "engine/physics/fanContact.hpp"

namespace fan
{
    struct Rigidbody;
    struct SphereCollider;
    struct BoxCollider;
    struct RenderDebug;

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct CollisionDetection
    {
        static RenderDebug* tmpRd;
        std::vector<Contact> mContacts;

        static void SphereWithSphere( Rigidbody& _rb0, SphereCollider& _sphere0, Rigidbody& _rb1, SphereCollider& _sphere1, std::vector<Contact>& _outContacts );
        static void BoxWithBox( Rigidbody& _rb0, BoxCollider& _box0, Rigidbody& _rb1, BoxCollider& _box1, std::vector<Contact>& _outContacts );
        static void SphereWithBox( Rigidbody& _rbSphere, SphereCollider& _sphere, Rigidbody& _rbBox, BoxCollider& _box, std::vector<Contact>& _outContacts );
        static void SphereWithPlane( Rigidbody& _rigidbody, SphereCollider& _sphere, const Vector3& _normal, const Fixed _offset, std::vector<Contact>& _outContacts );
        static void BoxWithPlane( Rigidbody& _rigidbody, BoxCollider& _box, const Vector3& _normal, const Fixed _offset, std::vector<Contact>& _outContacts );
    };
}