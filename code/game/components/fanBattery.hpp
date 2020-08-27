#pragma once

#include "ecs/fanEcsComponent.hpp"

namespace fan
{
	//========================================================================================================
	// A battery is an energy container
	// a battery allows fast moving of ships & firing weapons
	//========================================================================================================
	struct Battery : public EcsComponent
	{
		ECS_COMPONENT( Battery )
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );

		float mCurrentEnergy;
		float mMaxEnergy;
	};
}