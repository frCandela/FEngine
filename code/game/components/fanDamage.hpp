#pragma once

#include "game/fanGamePrecompiled.hpp"

#include "ecs/fanComponent.hpp"

namespace fan
{
	//================================================================================================================================
	// A damage component is used to remove health points
	//================================================================================================================================
	struct Damage : public Component
	{
		DECLARE_COMPONENT( struct Damage )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( EcsWorld& _world, Component& _component );
		static void OnGui( EcsWorld& _world, EntityID _entityID, Component& _component );
		static void Save( const Component& _component, Json& _json );
		static void Load( Component& _component, const Json& _json );

		float damage;
	};
	static constexpr size_t sizeof_damage= sizeof( Damage );
}