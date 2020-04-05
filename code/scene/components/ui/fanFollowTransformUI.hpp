#pragma  once

#include "scene/fanScenePrecompiled.hpp"

#include "ecs/fanComponent.hpp"
#include "scene/fanSceneResourcePtr.hpp"
#include "scene/components/ui/fanTransformUI.hpp"
namespace fan
{
	//==============================================================================================================================================================
	// makes a ui transform follow another ui transform
	//==============================================================================================================================================================
	struct FollowTransformUI : public Component
	{
		DECLARE_COMPONENT( FollowTransformUI )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( EcsWorld& _world, Component& _component );
		static void OnGui( EcsWorld& _world, EntityID _entityID, Component& _component );
		static void Save( const Component& _component, Json& _json );
		static void Load( Component& _component, const Json& _json );

		ComponentPtr<TransformUI> targetTransform;
		glm::vec2 offset;
		bool locked;

		static void UpdateOffset( EcsWorld& _world, EntityID _entityID );
	};
	static constexpr size_t sizeof_followTransformUUI = sizeof( FollowTransformUI );
}