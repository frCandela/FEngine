#pragma  once

#include "ecs/fanEcsComponent.hpp"
#include "scene/fanSceneResourcePtr.hpp"
#include "scene/components/ui/fanTransformUI.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// makes a ui transform follow another ui transform
	//==============================================================================================================================================================
	struct FollowTransformUI : public EcsComponent
	{
		ECS_COMPONENT( FollowTransformUI )
	public:
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );

		ComponentPtr<TransformUI> targetTransform;
		glm::vec2 offset;
		bool locked;

		static void UpdateOffset( EcsWorld& _world, EcsEntity _entityID );
	};
	static constexpr size_t sizeof_followTransformUUI = sizeof( FollowTransformUI );
}