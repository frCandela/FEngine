#include "game/components/fanBullet.hpp"

#include "scene/fanSceneSerializable.hpp"

namespace fan
{

	//================================================================================================================================
	//================================================================================================================================
	void Bullet::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::JOYSTICK16;
		_info.onGui = &Bullet::OnGui;
		_info.load = &Bullet::Load;
		_info.save = &Bullet::Save;
		_info.editorPath = "game/";
		_info.name = "bullet";
	}

	//================================================================================================================================
	//================================================================================================================================
	void Bullet::Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component )
	{
		Bullet& bullet = static_cast<Bullet&>( _component );

		// bullets
		bullet.explosionPrefab.Set( nullptr );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Bullet::OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component )
	{
		Bullet& bullet = static_cast<Bullet&>( _component );

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			ImGui::FanPrefab( "explosion prefab", bullet.explosionPrefab );
		} ImGui::PopItemWidth();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Bullet::Save( const EcsComponent& _component, Json& _json )
	{
		const Bullet& bullet = static_cast<const Bullet&>( _component );
		Serializable::SavePrefabPtr( _json, "explosion_prefab", bullet.explosionPrefab );	
	}

	//================================================================================================================================
	//================================================================================================================================
	void Bullet::Load( EcsComponent& _component, const Json& _json )
	{
		Bullet& bullet = static_cast<Bullet&>( _component );
		Serializable::LoadPrefabPtr( _json, "explosion_prefab", bullet.explosionPrefab );
	}
}