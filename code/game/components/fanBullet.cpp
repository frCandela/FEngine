#include "game/components/fanBullet.hpp"

#include "game/fanGameSerializable.hpp"

namespace fan
{
	REGISTER_COMPONENT( Bullet, "bullet" );

	//================================================================================================================================
	//================================================================================================================================
	void Bullet::SetInfo( ComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::JOYSTICK16;
		_info.onGui = &Bullet::OnGui;
		_info.init = &Bullet::Init;
		_info.load = &Bullet::Load;
		_info.save = &Bullet::Save;
		_info.editorPath = "game/";
	}

	//================================================================================================================================
	//================================================================================================================================
	void Bullet::Init( Component& _component )
	{
		Bullet& bullet = static_cast<Bullet&>( _component );

		// bullets
		bullet.damage = 5.f;
		bullet.explosionPrefab = nullptr;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Bullet::OnGui( Component& _component )
	{
		Bullet& bullet = static_cast<Bullet&>( _component );

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			ImGui::DragFloat( "damage ##bullet", &bullet.damage, 0.1f, 0.f, 100.f );
			ImGui::FanPrefab( "explosion prefab", bullet.explosionPrefab );
		} ImGui::PopItemWidth();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Bullet::Save( const Component& _component, Json& _json )
	{
		const Bullet& bullet = static_cast<const Bullet&>( _component );
		Serializable::SaveFloat( _json, "damage", bullet.damage );
		Serializable::SavePrefabPtr( _json, "explosion_prefab", bullet.explosionPrefab );	
	}

	//================================================================================================================================
	//================================================================================================================================
	void Bullet::Load( Component& _component, const Json& _json )
	{
		Bullet& bullet = static_cast<Bullet&>( _component );
		Serializable::LoadFloat( _json, "damage", bullet.damage );
		Serializable::LoadPrefabPtr( _json, "explosion_prefab", bullet.explosionPrefab );
	}
}