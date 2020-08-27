#pragma  once

#include "ecs/fanEcsComponent.hpp"
#include "render/fanRenderResourcePtr.hpp"
#include "core/math/fanVector2.hpp"

namespace fan
{
	struct Transform;
	struct Ray;

	//========================================================================================================
	// orthographic / perspective camera
	//========================================================================================================
	struct Camera : public EcsComponent
	{
		ECS_COMPONENT( Camera )
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );

		enum Type { Perspective, Orthogonal };
		float mFov;
		float mOrthoSize;
		float mAspectRatio;
		float mNearDistance;
		float mFarDistance;
		Type  mType;

		glm::mat4 GetView( const Transform& _cameraTransform ) const;
		glm::mat4 GetProjection() const;
        Ray ScreenPosToRay( const Transform& _cameraTransform, const btVector2& _screenSpacePosition ) const;
		btVector2 WorldPosToScreen( const Transform& _cameraTransform, const btVector3& worldPosition ) const;
	};
}