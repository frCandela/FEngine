#pragma once

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void Damage::SetInfo( EcsComponentInfo& _info )
	{
		_info.mIcon       = ImGui::IconType::Joystick16;
		_info.mGroup      = EngineGroups::Game;
		_info.onGui       = &Damage::OnGui;
		_info.mEditorPath = "game/";
		_info.mName       = "damage";
	}

    //========================================================================================================
    //========================================================================================================
    void Damage::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
    {
        Damage& damage = static_cast<Damage&>( _component );

        ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
        {
            ImGui::DragFloat( "damage ##Damage", &damage.mDamage );
        } ImGui::PopItemWidth();
    }
}