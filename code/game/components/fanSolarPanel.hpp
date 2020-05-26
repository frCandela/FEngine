#pragma once

#include "ecs/fanEcsComponent.hpp"

namespace fan
{
	//================================================================================================================================
	// creates energy from the sun light and stores it in a battery
	//================================================================================================================================
	class SolarPanel : public EcsComponent
	{
		ECS_COMPONENT( SolarPanel )
	public:
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );

		bool  isInSunlight;
		float currentChargingRate;  // Energy/s  @todo remove this ?
		float minChargingRate;		// Energy/s
		float maxChargingRate;		// Energy/s
		float lowRange;				// Distance at which the solar panel reaches maximum charging rate
		float highRange;			// Distance at which the solar panel reaches minimum charging rate
	};
	static constexpr size_t sizeof_solarPanel = sizeof( SolarPanel );
}