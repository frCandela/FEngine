#pragma once

#include "ecs/fanComponent.hpp"

namespace fan
{
	//================================================================================================================================
	// A battery is an energy container
	// a battery allows fast moving of ships & firing weapons
	//================================================================================================================================
	struct Battery : public Component
	{
		DECLARE_COMPONENT( Battery )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( EcsWorld& _world, Component& _component );
		static void OnGui( EcsWorld& _world, EntityID _entityID, Component& _component );
		static void Save( const Component& _component, Json& _json );
		static void Load( Component& _component, const Json& _json );

		float currentEnergy;
		float maxEnergy;
	};
	static constexpr size_t sizeof_battery = sizeof( Battery );
}