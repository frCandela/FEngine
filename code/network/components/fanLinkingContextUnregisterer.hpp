#pragma  once

#include "core/ecs/fanEcsComponent.hpp"
#include "network/fanNetConfig.hpp"

namespace fan
{
	//========================================================================================================
	// Automagicaly removes its net id from the linking context
	//========================================================================================================
	struct LinkingContextUnregisterer : public EcsComponent
	{
		ECS_COMPONENT( LinkingContextUnregisterer )
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );
		static void Destroy( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
	};
}