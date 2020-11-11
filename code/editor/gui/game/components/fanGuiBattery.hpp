#prama once

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void Battery::SetInfo( EcsComponentInfo& _info )
	{
		_info.mIcon       = ImGui::IconType::Energy16;
		_info.mGroup      = EngineGroups::Game;
		_info.onGui       = &Battery::OnGui;
		_info.mName       = "battery";
		_info.mEditorPath = "game/";
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