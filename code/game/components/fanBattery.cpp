#include "game/components/fanBattery.hpp"

#include "scene/fanSceneSerializable.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void Battery::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::ENERGY16;
		_info.group = EngineGroups::Game;
		_info.onGui = &Battery::OnGui;
		_info.load = &Battery::Load;
		_info.save = &Battery::Save;
		_info.name = "battery";
		_info.editorPath = "game/";
	}

	//========================================================================================================
	//========================================================================================================
	void Battery::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		Battery& battery = static_cast<Battery&>( _component );
		battery.mMaxEnergy     = 100.f;
		battery.mCurrentEnergy = battery.mMaxEnergy;
	}

	//========================================================================================================
	//========================================================================================================
	void Battery::Save( const EcsComponent& _component, Json& _json )
	{
		const Battery& battery = static_cast<const Battery&>( _component );

		Serializable::SaveFloat( _json, "max_energy", battery.mMaxEnergy );
	}

	//========================================================================================================
	//========================================================================================================
	void Battery::Load( EcsComponent& _component, const Json& _json )
	{
		Battery& battery = static_cast<Battery&>( _component );

		Serializable::LoadFloat( _json, "max_energy", battery.mMaxEnergy );
	}

    //========================================================================================================
    //========================================================================================================
    void Battery::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
    {
        Battery& battery = static_cast<Battery&>( _component );

        ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
        {
            ImGui::DragFloat( "current energy ##Battery", &battery.mCurrentEnergy );
            ImGui::DragFloat( "max energy     ##Battery", &battery.mMaxEnergy );
        } ImGui::PopItemWidth();
    }
}