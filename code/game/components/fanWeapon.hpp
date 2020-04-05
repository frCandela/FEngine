#pragma once

#include "game/fanGamePrecompiled.hpp"

#include "ecs/fanComponent.hpp"
#include "scene/fanSceneResourcePtr.hpp"

namespace fan
{
	//================================================================================================================================
	// fires bullets pew pew
	// is placed on the spaceship on the player 
	//================================================================================================================================
	struct Weapon : public Component
	{
		DECLARE_COMPONENT( Weapon )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( EcsWorld& _world, Component& _component );
		static void OnGui( EcsWorld& _world, EntityID _entityID, Component& _component );
		static void Save( const Component& _component, Json& _json );
		static void Load( Component& _component, const Json& _json );

		// Bullets parameters
		PrefabPtr bulletPrefab;
		btVector3 originOffset;
		float bulletSpeed;
		float bulletsPerSecond;
		float bulletEnergyCost;

		// time accumulator
		float bulletsAccumulator;
	};
	static constexpr size_t sizeof_weapon = sizeof( Weapon );
}