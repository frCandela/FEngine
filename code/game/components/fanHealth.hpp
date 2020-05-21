#pragma once

#include "ecs/fanEcsComponent.hpp"

namespace fan
{
	//================================================================================================================================
	// The health of en entity
	//================================================================================================================================
	class Health : public EcsComponent
	{
		ECS_COMPONENT( Health )
	public:
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );

		bool  invincible = false;
		float currentHealth = 0.f;
		float maxHealth = 100.f;
	};
	static constexpr size_t sizeof_health = sizeof( Health );
}