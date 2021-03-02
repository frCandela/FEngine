#pragma  once

#include "core/ecs/fanEcsComponent.hpp"

namespace fan
{
	//========================================================================================================
	// make an entity die after some time
	//========================================================================================================
	struct ExpirationTime : public EcsComponent
	{
		ECS_COMPONENT( ExpirationTime )
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );

		float mDuration;
	};
}