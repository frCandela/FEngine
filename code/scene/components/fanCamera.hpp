#pragma  once

#include "ecs/fanEcsComponent.hpp"
#include "render/fanRenderResourcePtr.hpp"
#include "core/math/fanVector2.hpp"

namespace fan
{
	struct Transform;
	struct Ray;

	//==============================================================================================================================================================
	// orthographic / perspective camera
	//==============================================================================================================================================================
	struct Camera : public EcsComponent
	{
		ECS_COMPONENT( Camera )
	public:
		enum Type { PERSPECTIVE, ORTHOGONAL };

		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );

		float fov;
		float orthoSize;
		float aspectRatio;
		float nearDistance;
		float farDistance;
		Type  type;

		glm::mat4 GetView( const Transform& _cameraTransform ) const;
		glm::mat4 GetProjection() const;
		Ray		  ScreenPosToRay( const Transform& _cameraTransform, const btVector2& _screenSpacePosition ) const ;
		btVector2 WorldPosToScreen( const Transform& _cameraTransform, const btVector3& worldPosition ) const;
	};
	static constexpr size_t sizeof_camera = sizeof( Camera );
}