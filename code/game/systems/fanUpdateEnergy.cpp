#include "game/systems/fanUpdateEnergy.hpp"

#include "ecs/fanEcsWorld.hpp"
#include "scene/components/fanTransform.hpp"
#include "game/components/fanSolarPanel.hpp"
#include "game/components/fanBattery.hpp"
#include "game/singletonComponents/fanSunLight.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	EcsSignature S_UpdateSolarPannels::GetSignature( const EcsWorld& _world )
	{
		return
			_world.GetSignature<Transform>() |
			_world.GetSignature<SolarPanel>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_UpdateSolarPannels::Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities, const float _delta )
	{
		if( _delta == 0.f ) { return; }

		const SunLight& sunLight = _world.GetSingleton<SunLight>();

		for( EcsEntity entity : _entities )
		{
			const Transform& transform = _world.GetComponent<Transform>( entity );
			SolarPanel& solarPanel = _world.GetComponent<SolarPanel>( entity );

			// sunlight mesh raycast
			const btVector3 rayOrigin = transform.GetPosition() + btVector3::Up();
			btVector3 outIntersection;
			solarPanel.isInSunlight = sunLight.mesh.RayCast( rayOrigin, -btVector3::Up(), outIntersection );
			
			// Charging rate
			if ( solarPanel.isInSunlight )
			{
				const btVector3 position = transform.GetPosition();
				const float distance = position.norm();
				const float slope = ( solarPanel.maxChargingRate - solarPanel.minChargingRate ) / ( solarPanel.highRange - solarPanel.lowRange );
				const float unclampedRate = solarPanel.maxChargingRate - slope * ( distance - solarPanel.lowRange );
				solarPanel.currentChargingRate = std::clamp( unclampedRate, solarPanel.minChargingRate, solarPanel.maxChargingRate );
			}
			else
			{
				solarPanel.currentChargingRate = 0.f;
			}				
		}
	}

	//================================================================================================================================
//================================================================================================================================
	EcsSignature S_RechargeBatteries::GetSignature( const EcsWorld& _world )
	{
		return
			_world.GetSignature<Battery>() |
			_world.GetSignature<SolarPanel>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_RechargeBatteries::Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities, const float _delta )
	{
		if( _delta == 0.f ) { return; }

		for( EcsEntity entity : _entities )
		{
			const SolarPanel& solarPanel = _world.GetComponent<SolarPanel>( entity );
			Battery& battery = _world.GetComponent<Battery>( entity );

			const float energyAdded = _delta * solarPanel.currentChargingRate;
			battery.currentEnergy = std::min( battery.currentEnergy + energyAdded, battery.maxEnergy );
		}
	}	 
}