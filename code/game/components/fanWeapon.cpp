#include "game/components/fanWeapon.hpp"

// #include "game/components/fanWithEnergy.hpp"
// #include "game/components/fanPlayerInput.hpp"
// #include "core/input/fanInput.hpp"
// #include "core/input/fanInputManager.hpp"
// #include "core/time/fanProfiler.hpp"
// #include "core/input/fanMouse.hpp"
// #include "core/time/fanTime.hpp"
#include "game/fanGameSerializable.hpp"

namespace fan
{
	REGISTER_COMPONENT( Weapon, "weapon" );

	//================================================================================================================================
	//================================================================================================================================
	void Weapon::SetInfo( ComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::JOYSTICK16;
		_info.onGui = &Weapon::OnGui;
		_info.init = &Weapon::Init;
		_info.load = &Weapon::Load;
		_info.save = &Weapon::Save;
		_info.editorPath = "game/";
	}

	//================================================================================================================================
	//================================================================================================================================
	void Weapon::Init( Component& _component )
	{
		Weapon& weapon = static_cast<Weapon&>( _component );

		// bullets
		weapon.bulletLifeTime = 1.f;
		weapon.scale = 0.2f;
		weapon.bulletSpeed = 0.2f;
		weapon.bulletsPerSecond = 10.f;
		weapon.bulletEnergyCost = 1.f;
		weapon.bulletDamage = 30.f;
		weapon.originOffset = btVector3::Zero();

		// explosion parameters
		weapon.explosionTime = 0.15f;
		weapon.exposionSpeed = 2.f;
		weapon.particlesPerExplosion = 3;

		// time accumulator
		weapon.bulletsAccumulator = 0.f;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Weapon::OnGui( Component& _component )
	{
		Weapon& weapon = static_cast<Weapon&>( _component );

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			//ImGui::FanPrefab( "bullet prefab", weapon.bulletPrefab );

			ImGui::DragFloat( "scale ##weapon", &weapon.scale, 0.05f, 0.f, 1.f );
			ImGui::DragFloat( "speed ##weapon", &weapon.bulletSpeed, 0.1f, 0.f, 100.f );
			ImGui::DragFloat3( "offset ##weapon", &weapon.originOffset[0] );
			ImGui::DragFloat( "bullets life time ##weapon", &weapon.bulletLifeTime, 1.f, 0.f, 100.f );
			ImGui::DragFloat( "bullets per second", &weapon.bulletsPerSecond, 1.f, 0.f, 1000.f );
			ImGui::DragFloat( "bullet energy cost", &weapon.bulletEnergyCost, 0.05f, 0.f, 10.f );
			ImGui::DragFloat( "bullet damage", &weapon.bulletDamage, 1.f, 0.f, 100.f );
			ImGui::Spacing();
			ImGui::DragFloat( "explosion time ##weapon", &weapon.explosionTime, 0.05f, 0.f, 10.f );
			ImGui::DragFloat( "explosion speed ##weapon", &weapon.exposionSpeed, 0.5f, 0.f, 100.f );
			ImGui::DragInt( "particles per explosion ##weapon", &weapon.particlesPerExplosion );

		} ImGui::PopItemWidth();
	}
	
	//================================================================================================================================
	//================================================================================================================================
	void Weapon::Save( const Component& _component, Json& _json )
	{
		const Weapon& weapon = static_cast<const Weapon&>( _component );

		//Serializable::SavePrefabPtr( _json, "bulletPrefab", weapon.bulletPrefab );
		Serializable::SaveFloat( _json, "scale", weapon.scale );
		Serializable::SaveFloat( _json, "speed", weapon.bulletSpeed );
		Serializable::SaveVec3( _json, "offset", weapon.originOffset );
		Serializable::SaveFloat( _json, "lifeTime", weapon.bulletLifeTime );
		Serializable::SaveFloat( _json, "bullets_per_second", weapon.bulletsPerSecond );
		Serializable::SaveFloat( _json, "bullet_energy_cost", weapon.bulletEnergyCost );
		Serializable::SaveFloat( _json, "bullet_damage", weapon.bulletDamage );
		Serializable::SaveFloat( _json, "exposion_speed", weapon.exposionSpeed );
		Serializable::SaveFloat( _json, "explosion_time", weapon.explosionTime );
		Serializable::SaveInt( _json, "particlesPerExplosion", weapon.particlesPerExplosion );
	}
	
	//================================================================================================================================
	//================================================================================================================================
	void Weapon::Load( Component& _component, const Json& _json )
	{
		Weapon& weapon = static_cast<Weapon&>( _component );

		//Serializable::SavePrefabPtr( _json, "bulletPrefab", weapon.bulletPrefab );
		Serializable::LoadFloat( _json, "scale", weapon.scale );
		Serializable::LoadFloat( _json, "speed", weapon.bulletSpeed );
		Serializable::LoadVec3( _json, "offset", weapon.originOffset );
		Serializable::LoadFloat( _json, "lifeTime", weapon.bulletLifeTime );
		Serializable::LoadFloat( _json, "bullets_per_second", weapon.bulletsPerSecond );
		Serializable::LoadFloat( _json, "bullet_energy_cost", weapon.bulletEnergyCost );
		Serializable::LoadFloat( _json, "bullet_damage", weapon.bulletDamage );
		Serializable::LoadFloat( _json, "exposion_speed", weapon.exposionSpeed );
		Serializable::LoadFloat( _json, "explosion_time", weapon.explosionTime );
		Serializable::LoadInt( _json, "particlesPerExplosion", weapon.particlesPerExplosion );
	}

// 	//================================================================================================================================
// 	//================================================================================================================================
// 	void Weapon::OnBulletContact( Rigidbody* _other, btPersistentManifold* const& _manifold )
// 	{
// 		Rigidbody* rb0 = static_cast< Rigidbody* > ( _manifold->getBody0()->getUserPointer() );
// 		Rigidbody* rb1 = static_cast< Rigidbody* > ( _manifold->getBody1()->getUserPointer() );
// 		Rigidbody* bulletRb = _other == rb0 ? rb1 : rb0;
// 
// 		m_gameobject->GetScene().DeleteGameobject( &bulletRb->GetGameobject() );
// 		if ( GetScene().IsServer() == false )
// 		{
// 			CreateExplosion( bulletRb->GetGameobject().GetTransform().GetPosition() );
// 		}
//	}
// 
// 	//================================================================================================================================
// 	//================================================================================================================================
// 	void  Weapon::CreateExplosion( const btVector3 _point )
// 	{
// 		for ( int particleIndex = 0; particleIndex < m_particlesPerExplosion; particleIndex++ )
// 		{
// 			EcsManager& ecs = m_gameobject->GetScene().GetEcsManager();
// 			ecsEntity entity = ecs.CreateEntity();
// 			ecsPosition& position = ecs.AddComponent<ecsPosition>( entity );
// 			ecs.AddComponent<ecsRotation>( entity ).Init();
// 			ecsMovement& movement = ecs.AddComponent<ecsMovement>( entity );
// 			ecsParticle& particle = ecs.AddComponent<ecsParticle>( entity );
// 
// 			movement.speed = btVector3( m_distribution( m_generator ), 0.f, m_distribution( m_generator ) ) - btVector3( 0.5f, 0.0f, 0.5f );
// 			movement.speed.normalize();
// 			movement.speed *= m_distribution( m_generator ) * m_exposionSpeed;
// 			position.position = _point;
// 			particle.durationLeft = m_explosionTime;
// 		}
// 	}
}