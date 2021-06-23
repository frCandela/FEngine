#pragma  once

#include "ecs/fanEcsComponent.hpp"
#include "core/math/fanFixedPoint.hpp"
#include "fanGlm.hpp"

namespace fan
{
    struct Transform;
    struct Vector3;
    struct Ray;

    //==================================================================================================================================================================================================
    // orthographic / perspective camera
    //==================================================================================================================================================================================================
    struct Camera : public EcsComponent
    {
    ECS_COMPONENT( Camera )
        static void SetInfo( EcsComponentInfo& _info );
        static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
        static void Save( const EcsComponent& _component, Json& _json );
        static void Load( EcsComponent& _component, const Json& _json );

        enum Type
        {
            Perspective, Orthogonal
        };
        Fixed mFov;
        Fixed mOrthoSize;
        Fixed mAspectRatio;
        Fixed mNearDistance;
        Fixed mFarDistance;
        Type  mType;

        glm::mat4 GetView( const Transform& _cameraTransform ) const;
        glm::mat4 GetProjection() const;
        Ray ScreenPosToRay( const Transform& _cameraTransform, const glm::vec2& _screenSpacePosition ) const;
        glm::vec2 WorldPosToScreen( const Transform& _cameraTransform, const Vector3& _worldPosition ) const;
    };
}