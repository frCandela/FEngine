#pragma once

#include <vector>
#include <unordered_set>
#include "engine/physics/fanContact.hpp"

namespace fan
{
    struct CollisionPair
    {
        int index0;
        int index1;
        constexpr bool operator==( const CollisionPair& _value ) const { return index0 == _value.index0 && index1 == _value.index1; }
    };
}

namespace std
{
    template<> struct hash<fan::CollisionPair>
    {
        size_t operator()( fan::CollisionPair const& _pair ) const
        {
            return size_t( _pair.index0 ) | size_t( _pair.index1 ) << 32;
        }
    };
}

namespace fan
{
    struct FxPhysicsWorld;
    struct FxTransform;
    struct FxRigidbody;
    struct FxSphereCollider;

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct CollisionDetection
    {
        std::vector<Contact>              mContacts;

        static bool SphereWithSphere( FxRigidbody&_rb0, FxSphereCollider& _sphere0, FxTransform& _transform0,
                                      FxRigidbody& _rb1, FxSphereCollider& _sphere1, FxTransform& _transform1, FxPhysicsWorld& _physicsWorld );
        static bool SphereWithPlane( FxRigidbody&_rb0, FxSphereCollider& _sphere0, FxTransform& _transform0,
                                     Vector3 _normal, Fixed _offset, FxPhysicsWorld& _physicsWorld );
    };
}