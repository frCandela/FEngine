#pragma  once

#include "scene/fanScenePrecompiled.hpp"

#include "ecs/fanComponent.hpp"
#include "render/fanRenderResourcePtr.hpp"

namespace fan
{
	struct Transform;
	struct Ray;

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct Camera : public Component
	{
		DECLARE_COMPONENT( Camera )
	public:
		enum Type { PERSPECTIVE, ORTHOGONAL };

		static void SetInfo( ComponentInfo& _info );
		static void Init( EcsWorld& _world, Component& _component );
		static void OnGui( EcsWorld& _world, EntityID _entityID, Component& _component );
		static void Save( const Component& _component, Json& _json );
		static void Load( Component& _component, const Json& _json );

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