#include "game/components/fanSolarPanel.hpp"

#include "scene/fanSceneSerializable.hpp"
#include "editor/fanModals.hpp"

namespace fan
{	
	//========================================================================================================
	//========================================================================================================
	void SolarPanel::SetInfo( EcsComponentInfo& _info )
	{
		_info.load        = &SolarPanel::Load;
		_info.save        = &SolarPanel::Save;
	}

	//========================================================================================================
	//========================================================================================================
	void SolarPanel::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		SolarPanel& solarPanel = static_cast<SolarPanel&>( _component );
		solarPanel.mIsInSunlight        = false;
		solarPanel.mCurrentChargingRate = 0.f;
		solarPanel.mMinChargingRate     = 0.f;
		solarPanel.mMaxChargingRate     = 10.f;
		solarPanel.mLowRange            = 2.f;
		solarPanel.mHighRange           = 30.f;
	}

	//========================================================================================================
	//========================================================================================================
	void SolarPanel::Save( const EcsComponent& _component, Json& _json )
	{
		const SolarPanel& solarPanel = static_cast<const SolarPanel&>( _component );

		Serializable::SaveFloat( _json, "min_charging_rate", solarPanel.mMinChargingRate );
		Serializable::SaveFloat( _json, "max_charging_rate", solarPanel.mMaxChargingRate );
		Serializable::SaveFloat( _json, "low_range",		 solarPanel.mLowRange );
		Serializable::SaveFloat( _json, "high_range",		 solarPanel.mHighRange );
	}

	//========================================================================================================
	//========================================================================================================
	void SolarPanel::Load( EcsComponent& _component, const Json& _json )
	{
		SolarPanel& solarPanel = static_cast<SolarPanel&>( _component );

		Serializable::LoadFloat( _json, "min_charging_rate", solarPanel.mMinChargingRate );
		Serializable::LoadFloat( _json, "max_charging_rate", solarPanel.mMaxChargingRate );
		Serializable::LoadFloat( _json, "low_range", solarPanel.mLowRange );
		Serializable::LoadFloat( _json, "high_range", solarPanel.mHighRange );
	}
}