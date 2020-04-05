#pragma once

#include "game/fanGamePrecompiled.hpp"

#include "ecs/fanComponent.hpp"
#include "scene/fanSceneResourcePtr.hpp"

namespace fan
{
	//================================================================================================================================
	// A projectile that deals damage 
	// @todo merge this with the damage component
	//================================================================================================================================
	struct Bullet : public Component
	{
		DECLARE_COMPONENT( Bullet )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( EcsWorld& _world, Component& _component );
		static void OnGui( EcsWorld& _world, EntityID _entityID, Component& _component );
		static void Save( const Component& _component, Json& _json );
		static void Load( Component& _component, const Json& _json );

		float	  damage;
		PrefabPtr explosionPrefab;
	};
	static constexpr size_t sizeof_bullet = sizeof( Bullet );
}