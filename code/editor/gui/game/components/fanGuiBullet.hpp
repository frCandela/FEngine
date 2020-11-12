#pragma once

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void Bullet::SetInfo( EcsComponentInfo& _info )
	{
		_info.mIcon       = ImGui::IconType::Joystick16;
		_info.mGroup      = EngineGroups::Game;
		_info.onGui       = &Bullet::OnGui;
		_info.mEditorPath = "game/";
		_info.mName       = "bullet";
	}

    //========================================================================================================
    //========================================================================================================
    void Bullet::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
    {
        Bullet& bullet = static_cast<Bullet&>( _component );

        ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
        {
            ImGui::FanPrefab( "explosion prefab", bullet.mExplosionPrefab );
        } ImGui::PopItemWidth();
    }
}