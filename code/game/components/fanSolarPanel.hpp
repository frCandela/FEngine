#pragma once

#include "game/fanGamePrecompiled.hpp"

#include "ecs/fanComponent.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	class SolarPanel : public Component
	{
		DECLARE_COMPONENT( SolarPanel )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( EcsWorld& _world, Component& _component );
		static void OnGui( EcsWorld& _world, EntityID _entityID, Component& _component );
		static void Save( const Component& _component, Json& _json );
		static void Load( Component& _component, const Json& _json );

		bool  isInSunlight;
		float currentChargingRate;  // Energy/s
		float minChargingRate;		// Energy/s
		float maxChargingRate;		// Energy/s
		float lowRange;				// Distance at which the solar panel reaches maximum charging rate
		float highRange;			// Distance at which the solar panel reaches minimum charging rate
	};
	static constexpr size_t sizeof_solarPanel = sizeof( SolarPanel );
}