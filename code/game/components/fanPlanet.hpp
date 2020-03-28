#pragma once

#include "game/fanGamePrecompiled.hpp"
#include "ecs/fanComponent.hpp"

namespace fan
{
	struct ComponentInfo;

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct Planet : public Component
	{
		DECLARE_COMPONENT( Planet )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( EcsWorld& _world, Component& _component );
		static void OnGui( Component& _component );
		static void Save( const Component& _component, Json& _json );
		static void Load( Component& _component, const Json& _json );

		float timeAccumulator;
		float speed;
		float radius;
		float phase;
	};
	static constexpr size_t sizeof_planet = sizeof( Planet );
}