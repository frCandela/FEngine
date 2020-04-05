#pragma once

#include "game/fanGamePrecompiled.hpp"

#include "ecs/fanComponent.hpp"

namespace fan
{
	//================================================================================================================================
	// The health of en entity
	//================================================================================================================================
	class Health : public Component
	{
		DECLARE_COMPONENT( Health )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( EcsWorld& _world, Component& _component );
		static void OnGui( EcsWorld& _world, EntityID _entityID, Component& _component );
		static void Save( const Component& _component, Json& _json );
		static void Load( Component& _component, const Json& _json );

		bool  invincible = false;
		float currentHealth = 0.f;
		float maxHealth = 100.f;
	};
	static constexpr size_t sizeof_health = sizeof( Health );
}