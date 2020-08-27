#include "game/components/fanSolarPanel.hpp"

#include "scene/fanSceneSerializable.hpp"
#include "editor/fanModals.hpp"

namespace fan
{	
	//========================================================================================================
	//========================================================================================================
	void SolarPanel::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::JOYSTICK16;
		_info.group = EngineGroups::Game;
		_info.onGui = &SolarPanel::OnGui;
		_info.load = &SolarPanel::Load;
		_info.save = &SolarPanel::Save;
		_info.editorPath = "game/";
		_info.name = "solar panel";
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

    //========================================================================================================
    //========================================================================================================
    void SolarPanel::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
    {
        SolarPanel& solarPanel = static_cast<SolarPanel&>( _component );

        ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
        {
            ImGui::DragFloat( "min charging rate", &solarPanel.mMinChargingRate, 0.5f, 0.f, 100.f );
            ImGui::DragFloat( "max charging rate", &solarPanel.mMaxChargingRate, 0.5f, 0.f, 100.f );
            ImGui::DragFloat( "low range		", &solarPanel.mLowRange, 0.5f, 0.f, 100.f );
            ImGui::DragFloat( "high range 		", &solarPanel.mHighRange, 0.5f, 0.f, 100.f );
            ImGui::Spacing();
            ImGui::PushReadOnly();
            ImGui::Checkbox( "is inside sunlight", &solarPanel.mIsInSunlight );
            ImGui::DragFloat( "charging rate", &solarPanel.mCurrentChargingRate, 0.5f, 0.f, 100.f );
            ImGui::PopReadOnly();
        } ImGui::PopItemWidth();
    }
}