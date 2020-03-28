#pragma once

#include "game/fanGamePrecompiled.hpp"

#include "ecs/fanComponent.hpp"
#include "scene/fanSceneResourcePtr.hpp"

namespace fan
{
	struct ComponentInfo;
	struct Weapon;
	

	//================================================================================================================================
	//================================================================================================================================
	struct Bullet : public Component
	{
		DECLARE_COMPONENT( Bullet )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( EcsWorld& _world, Component& _component );
		static void OnGui( Component& _component );
		static void Save( const Component& _component, Json& _json );
		static void Load( Component& _component, const Json& _json );

		float	  damage;
		PrefabPtr explosionPrefab;
	};
	static constexpr size_t sizeof_bullet = sizeof( Bullet );
}