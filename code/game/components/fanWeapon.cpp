#include "game/components/fanWeapon.hpp"

#include "editor/fanModals.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void Weapon::SetInfo( EcsComponentInfo& _info )
	{
		_info.mIcon       = ImGui::IconType::Joystick16;
		_info.mGroup      = EngineGroups::Game;
		_info.onGui       = &Weapon::OnGui;
		_info.load        = &Weapon::Load;
		_info.save        = &Weapon::Save;
		_info.mEditorPath = "game/";
		_info.mName       = "weapon";
	}

	//========================================================================================================
	//========================================================================================================
	void Weapon::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		Weapon& weapon = static_cast<Weapon&>( _component );

		// bullets
		weapon.mBulletSpeed      = 0.2f;
		weapon.mBulletsPerSecond = 10.f;
		weapon.mBulletEnergyCost = 1.f;
		weapon.mOriginOffset     = btVector3::Zero();
		weapon.bBulletsTimeAccumulator = 0.f;
		weapon.mBulletPrefab.Set( nullptr );
	}
	
	//========================================================================================================
	//========================================================================================================
	void Weapon::Save( const EcsComponent& _component, Json& _json )
	{
		const Weapon& weapon = static_cast<const Weapon&>( _component );

		Serializable::SavePrefabPtr( _json, "bullet_prefab", weapon.mBulletPrefab );
		Serializable::SaveFloat( _json, "speed", weapon.mBulletSpeed );
		Serializable::SaveVec3( _json, "offset", weapon.mOriginOffset );
		Serializable::SaveFloat( _json, "bullets_per_second", weapon.mBulletsPerSecond );
		Serializable::SaveFloat( _json, "bullet_energy_cost", weapon.mBulletEnergyCost );
	}
	
	//========================================================================================================
	//========================================================================================================
	void Weapon::Load( EcsComponent& _component, const Json& _json )
	{
		Weapon& weapon = static_cast<Weapon&>( _component );

		Serializable::LoadPrefabPtr( _json, "bullet_prefab", weapon.mBulletPrefab );
		Serializable::LoadFloat( _json, "speed", weapon.mBulletSpeed );
		Serializable::LoadVec3( _json, "offset", weapon.mOriginOffset );
		Serializable::LoadFloat( _json, "bullets_per_second", weapon.mBulletsPerSecond );
		Serializable::LoadFloat( _json, "bullet_energy_cost", weapon.mBulletEnergyCost );
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