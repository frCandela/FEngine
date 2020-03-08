#pragma  once

#include "scene/fanScenePrecompiled.hpp"

#include "scene/ecs/fanEcComponent.hpp"
#include "render/fanRenderResourcePtr.hpp"

namespace fan
{
	struct ComponentInfo;
	struct Transform2;
	struct Ray;

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct Camera2 : public ecComponent
	{
		DECLARE_COMPONENT( Camera2 )
	public:
		enum Type { PERSPECTIVE, ORTHOGONAL };

		static void SetInfo( ComponentInfo& _info );
		static void Clear( ecComponent& _camera );
		static void OnGui( ecComponent& _camera );
		static void Save( const ecComponent& _camera, Json& _json );
		static void Load( ecComponent& _camera, const Json& _json );

		float fov;
		float orthoSize;
		float aspectRatio;
		float nearDistance;
		float farDistance;
		Type  type;

		glm::mat4 GetView( const Transform2& _cameraTransform ) const;
		glm::mat4 GetProjection() const;
		Ray ScreenPosToRay( const Transform2& _cameraTransform, const btVector2& _screenSpacePosition );
		btVector2 WorldPosToScreen( const Transform2& _cameraTransform, const btVector3& worldPosition );
	};
	static constexpr size_t sizeof_camera = sizeof( Camera2 );
}