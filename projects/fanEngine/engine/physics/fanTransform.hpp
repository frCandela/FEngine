#pragma once

#include "ecs/fanEcsComponent.hpp"
#include "fanDisableWarnings.hpp"
#include "fanGlm.hpp"
#include "core/math/fanVector3.hpp"
#include "core/math/fanQuaternion.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    // position and rotation only
    // scale must be handled with a FxScale component
    //==================================================================================================================================================================================================
    struct Transform : public EcsComponent
    {
    ECS_COMPONENT( Transform )
        static void SetInfo( EcsComponentInfo& _info );
        static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
        static void Save( const EcsComponent& _component, Json& _json );
        static void Load( EcsComponent& _component, const Json& _json );

        static const Transform sIdentity;

        static Transform Make( const Quaternion& _rotation, const Vector3& _position );
        void LookAt( const Vector3& _point, const Vector3& _up = Vector3::sUp );

        glm::mat4 GetModelMatrix( const Vector3& _scale = Vector3::sOne ) const;
        glm::mat4 GetNormalMatrix( const Vector3& _scale = Vector3::sOne ) const;

        Vector3 Left() const { return mRotation * Vector3::sLeft; }
        Vector3 Right() const { return mRotation * Vector3::sRight; }
        Vector3 Forward() const { return mRotation * Vector3::sForward; }
        Vector3 Back() const { return mRotation * Vector3::sBack; }
        Vector3 Up() const { return mRotation * Vector3::sUp; }
        Vector3 Down() const { return mRotation * Vector3::sDown; }

        Transform Inverse() const;
        Vector3 TransformPoint( const Vector3 _point, const Vector3 _scale = Vector3::sOne ) const;
        Vector3 TransformDirection( const Vector3 _point, const Vector3 _scale = Vector3::sOne ) const;
        Vector3 InverseTransformPoint( const Vector3 _point, const Vector3 _scale = Vector3::sOne ) const;
        Vector3 InverseTransformDirection( const Vector3 _point, const Vector3 _scale = Vector3::sOne ) const;

        Quaternion mRotation;
        Vector3    mPosition;
    };

    inline Transform operator*( const Transform& _t1, const Transform& _t2 )
    {
        Transform transform;
        transform.mPosition = _t1.mRotation * _t2.mPosition + _t1.mPosition;
        transform.mRotation = _t1.mRotation * _t2.mRotation;
        return transform;
    }

    inline Vector3 operator*( const Transform& _transform, const Vector3& _vector )
    {
        return _transform.TransformPoint( _vector );
    }
}