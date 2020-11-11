#prama once

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void Weapon::SetInfo( EcsComponentInfo& _info )
	{
		_info.mIcon       = ImGui::IconType::Joystick16;
		_info.mGroup      = EngineGroups::Game;
		_info.onGui       = &Weapon::OnGui;
		_info.mEditorPath = "game/";
		_info.mName       = "weapon";
	}

    //========================================================================================================
    //========================================================================================================
    void Weapon::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
    {
        Weapon& weapon = static_cast<Weapon&>( _component );

        ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
        {
            ImGui::FanPrefab( "bullet prefab", weapon.mBulletPrefab );
            ImGui::FanToolTip("must have a transform, rigidbody, sphere shape & motion state");
            ImGui::DragFloat( "speed ##weapon", &weapon.mBulletSpeed, 0.1f, 0.f, 100.f );
            ImGui::DragFloat3( "offset ##weapon", &weapon.mOriginOffset[0] );
            ImGui::DragFloat( "bullets per second", &weapon.mBulletsPerSecond, 1.f, 0.f, 1000.f );
            ImGui::DragFloat( "bullet energy cost", &weapon.mBulletEnergyCost, 0.05f, 0.f, 10.f );
        } ImGui::PopItemWidth();
    }
}