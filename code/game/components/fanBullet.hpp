#pragma once

#include "game/fanGamePrecompiled.hpp"

#include "ecs/fanComponent.hpp"

namespace fan
{
	struct ComponentInfo;

	//================================================================================================================================
	//================================================================================================================================
	struct Bullet : public Component
	{
		DECLARE_COMPONENT( Bullet )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( Component& _component );
		static void OnGui( Component& _component );
		static void Save( const Component& _component, Json& _json );
		static void Load( Component& _component, const Json& _json );

		float durationLeft;
		float damage;
	};
	static constexpr size_t sizeof_bullet = sizeof( Bullet );
}