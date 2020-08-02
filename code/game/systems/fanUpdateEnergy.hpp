#include "ecs/fanEcsSystem.hpp"
#include "scene/components/fanTransform.hpp"
#include "game/components/fanSolarPanel.hpp"
#include "game/components/fanBattery.hpp"
#include "game/singletons/fanSunLight.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// updates solar panels charging rate depending on where they are in the sunlight
	//==============================================================================================================================================================
	struct S_UpdateSolarPannels : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return
				_world.GetSignature<Transform>() |
				_world.GetSignature<SolarPanel>();
		}
		static void Run( EcsWorld& _world, const EcsView& _view, const float _delta )
		{
			if( _delta == 0.f ) { return; }

			const SunLight& sunLight = _world.GetSingleton<SunLight>();

			auto transformIt = _view.begin<Transform>();
			auto solarPanelIt = _view.begin<SolarPanel>();
			for( ; transformIt != _view.end<Transform>(); ++transformIt, ++solarPanelIt )
			{
				const Transform& transform = *transformIt;
				SolarPanel& solarPanel = *solarPanelIt;

				// sunlight mesh raycast
				const btVector3 rayOrigin = transform.GetPosition() + btVector3::Up();
				btVector3 outIntersection;
				solarPanel.isInSunlight = sunLight.mesh->RayCast( rayOrigin, -btVector3::Up(), outIntersection );

				// Charging rate
				if( solarPanel.isInSunlight )
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
	};

	//==============================================================================================================================================================
	// recharges batteries from solar panels
	//==============================================================================================================================================================
	struct S_RechargeBatteries : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return
				_world.GetSignature<Battery>() |
				_world.GetSignature<SolarPanel>();
		}

		static void Run( EcsWorld& /*_world*/, const EcsView& _view, const float _delta )
		{
			if( _delta == 0.f ) { return; }

			auto batteryIt = _view.begin<Battery>();
			auto solarPanelIt = _view.begin<SolarPanel>();
			for( ; batteryIt != _view.end<Battery>(); ++batteryIt, ++solarPanelIt )
			{
				Battery& battery = *batteryIt;
				const SolarPanel& solarPanel = *solarPanelIt;

				const float energyAdded = _delta * solarPanel.currentChargingRate;
				battery.currentEnergy = std::min( battery.currentEnergy + energyAdded, battery.maxEnergy );
			}
		}
	};
}