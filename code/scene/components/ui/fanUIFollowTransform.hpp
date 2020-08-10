#pragma  once

#include "ecs/fanEcsComponent.hpp"
#include "scene/fanSceneResourcePtr.hpp"
#include "scene/components/ui/fanUITransform.hpp"

namespace fan
{
	//========================================================================================================
	// makes a ui transform follow another ui transform
	//========================================================================================================
	struct UIFollowTransform : public EcsComponent
	{
		ECS_COMPONENT( UIFollowTransform )
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );

		ComponentPtr<UITransform> mTarget;
		glm::ivec2                mOffset;
		bool                      mlocked;

		static void UpdateOffset( EcsWorld& _world, EcsEntity _entityID );
	};
}