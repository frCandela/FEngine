#include "game/components/fanSolarPanel.hpp"

#include "scene/fanSceneSerializable.hpp"
#include "editor/fanModals.hpp"

namespace fan
{	
	REGISTER_COMPONENT( SolarPanel, "solar pannel" );

	//================================================================================================================================
	//================================================================================================================================
	void SolarPanel::SetInfo( ComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::JOYSTICK16;
		_info.onGui = &SolarPanel::OnGui;
		_info.init = &SolarPanel::Init;
		_info.load = &SolarPanel::Load;
		_info.save = &SolarPanel::Save;
		_info.editorPath = "game/";
	}

	//================================================================================================================================
	//================================================================================================================================
	void SolarPanel::Init( EcsWorld& _world, Component& _component )
	{
		SolarPanel& solarPanel = static_cast<SolarPanel&>( _component );
		solarPanel.isInSunlight = false;
		solarPanel.currentChargingRate = 0.f;
		solarPanel.minChargingRate = 0.f;
		solarPanel.maxChargingRate = 10.f;
		solarPanel.lowRange = 2.f;
		solarPanel.highRange = 30.f;
	}

	//================================================================================================================================
	//================================================================================================================================
	void SolarPanel::OnGui( EcsWorld& _world, EntityID _entityID, Component& _component )
	{
		SolarPanel& solarPanel = static_cast<SolarPanel&>( _component );

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			ImGui::DragFloat( "min charging rate", &solarPanel.minChargingRate, 0.5f, 0.f, 100.f );
			ImGui::DragFloat( "max charging rate", &solarPanel.maxChargingRate, 0.5f, 0.f, 100.f );
			ImGui::DragFloat( "low range		", &solarPanel.lowRange, 0.5f, 0.f, 100.f );
			ImGui::DragFloat( "high range 		", &solarPanel.highRange, 0.5f, 0.f, 100.f );
			ImGui::Spacing();
			ImGui::PushReadOnly();
			ImGui::Checkbox( "is inside sunlight", &solarPanel.isInSunlight );
			ImGui::DragFloat( "charging rate", &solarPanel.currentChargingRate, 0.5f, 0.f, 100.f );
			ImGui::PopReadOnly();
		} ImGui::PopItemWidth();
	}

	//================================================================================================================================
	//================================================================================================================================
	void SolarPanel::Save( const Component& _component, Json& _json )
	{
		const SolarPanel& solarPanel = static_cast<const SolarPanel&>( _component );

		Serializable::SaveFloat( _json, "min_charging_rate", solarPanel.minChargingRate );
		Serializable::SaveFloat( _json, "max_charging_rate", solarPanel.maxChargingRate );
		Serializable::SaveFloat( _json, "low_range",		 solarPanel.lowRange );
		Serializable::SaveFloat( _json, "high_range",		 solarPanel.highRange );
	}

	//================================================================================================================================
	//================================================================================================================================
	void SolarPanel::Load( Component& _component, const Json& _json )
	{
		SolarPanel& solarPanel = static_cast<SolarPanel&>( _component );

		Serializable::LoadFloat( _json, "min_charging_rate", solarPanel.minChargingRate );
		Serializable::LoadFloat( _json, "max_charging_rate", solarPanel.maxChargingRate );
		Serializable::LoadFloat( _json, "low_range", solarPanel.lowRange );
		Serializable::LoadFloat( _json, "high_range", solarPanel.highRange );
	}
}