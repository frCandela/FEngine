#include "game/components/fanWeapon.hpp"
#include "game/components/fanWithEnergy.hpp"
#include "game/components/fanPlayerInput.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanMaterial.hpp"
#include "scene/components/fanRigidbody.hpp"
#include "scene/components/fanMeshRenderer.hpp"
#include "scene/components/fanSphereShape.hpp"
#include "core/input/fanInput.hpp"
#include "core/input/fanInputManager.hpp"
#include "core/time/fanProfiler.hpp"
#include "core/input/fanMouse.hpp"
#include "core/time/fanTime.hpp"

namespace fan
{
	REGISTER_TYPE_INFO( Weapon, TypeInfo::Flags::EDITOR_COMPONENT, "game/" )

	//================================================================================================================================
	//================================================================================================================================
	void Weapon::Start()
	{
		REQUIRE_COMPONENT( WithEnergy, m_energy );
		REQUIRE_COMPONENT( PlayerInput, m_input );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Weapon::OnAttach()
	{
		Actor::OnAttach();
		m_distribution = std::uniform_real_distribution<float> ( 0.f, 1.f );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Weapon::OnDetach()
	{
		Actor::OnDetach();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Weapon::LateUpdate( const float /*_delta*/ ){}

	//================================================================================================================================
	//================================================================================================================================
	void Weapon::Update( const float _delta )
	{
		SCOPED_PROFILE( weapon_update )

		m_bulletsAccumulator += _delta * m_bulletsPerSecond;
		if ( m_bulletsAccumulator > 1.f )
		{
			m_bulletsAccumulator = 1.f;
		}

		m_input->RefreshInput();
		const float fire = m_input->GetInputData().fire;
		if ( fire > 0 && m_bulletsAccumulator >= 1.f ) {
			-- m_bulletsAccumulator;
			if( m_energy->TryRemoveEnergy( m_bulletEnergyCost ) ) {
				FireBullet( );  
			}
			
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Weapon::OnBulletContact( Rigidbody* _other, btPersistentManifold* const& _manifold )
	{
		
		Rigidbody * rb0 = static_cast<Rigidbody*> ( _manifold->getBody0()->getUserPointer() );
 		Rigidbody * rb1 = static_cast<Rigidbody*> ( _manifold->getBody1()->getUserPointer() );
		Rigidbody * bulletRb = _other == rb0 ? rb1 : rb0;

		m_gameobject->GetScene().DeleteGameobject( &bulletRb->GetGameobject() );
		if ( GetScene().IsServer() == false )
		{
			CreateExplosion( bulletRb->GetGameobject().GetTransform().GetPosition());
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void  Weapon::CreateExplosion( const btVector3 _point )
	{
		for (int particleIndex = 0; particleIndex < m_particlesPerExplosion ; particleIndex++)
		{		
			EcsManager& ecs = m_gameobject->GetScene().GetEcsManager();
			ecsEntity entity = ecs.CreateEntity();
			ecsPosition & position = ecs.AddComponent<ecsPosition>( entity );
			ecs.AddComponent<ecsRotation>( entity ).Init();
			ecsMovement & movement = ecs.AddComponent<ecsMovement>( entity );
			ecsParticle & particle = ecs.AddComponent<ecsParticle>( entity );

			movement.speed = btVector3( m_distribution( m_generator ), 0.f, m_distribution( m_generator ) ) - btVector3( 0.5f, 0.0f, 0.5f );
			movement.speed.normalize();
			movement.speed *= m_distribution( m_generator ) * m_exposionSpeed;
			position.position = _point;
			particle.durationLeft = m_explosionTime;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Weapon::FireBullet()
	{
		if ( *m_bulletPrefab != nullptr )
		{
			Gameobject * bullet = m_gameobject->GetScene().CreateGameobject( **m_bulletPrefab, m_gameobject );
			bullet->AddEcsComponent<ecsBullet>()->Init( m_lifeTime, m_bulletDamage );			

			Rigidbody * rb = bullet->GetComponent<Rigidbody>();
			if ( rb != nullptr )
			{
				const Rigidbody * thisRb = m_gameobject->GetComponent<Rigidbody>();
				const Transform& thisTransform = m_gameobject->GetTransform();

				rb->onContactStarted.Connect( &Weapon::OnBulletContact, this );
				rb->SetIgnoreCollisionCheck( *thisRb, true );
				rb->SetVelocity( thisRb->GetVelocity() + m_speed * thisTransform.Forward() );

				Transform& transform = bullet->GetTransform();
				transform.SetPosition( thisTransform.GetPosition() + thisTransform.TransformDirection( m_offset ) );
			}
		} 		
	}

	//================================================================================================================================
	//================================================================================================================================
	void Weapon::OnGui()
	{
		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			ImGui::FanPrefab("bullet prefab", &m_bulletPrefab );
			ImGui::DragFloat( "scale ##wepoffset", &m_scale, 0.05f, 0.f, 1.f );
			ImGui::DragFloat( "speed ##wepspeed", &m_speed, 0.1f, 0.f, 100.f );
			ImGui::DragFloat3( "offset ##wepspeed", &m_offset[0] );
			ImGui::DragFloat( "life time ##wepspeed", &m_lifeTime, 1.f, 0.f, 100.f );
			ImGui::DragFloat( "bullets per second", &m_bulletsPerSecond, 1.f, 0.f, 1000.f );
			ImGui::DragFloat( "bullet energy cost", &m_bulletEnergyCost, 0.05f, 0.f, 10.f );
			ImGui::DragFloat( "bullet damage", &m_bulletDamage, 1.f, 0.f, 100.f );
			ImGui::Spacing();
			ImGui::DragFloat( "explosion time ##wep_exposionTime", &m_explosionTime, 0.05f, 0.f, 10.f );
			ImGui::DragFloat( "explosion speed ##wep_exposionSpeed", &m_exposionSpeed, 0.5f, 0.f, 100.f );
			ImGui::DragInt( "particles per explosion ##wep_particlesPerExplosion", &m_particlesPerExplosion );

		} ImGui::PopItemWidth();
	}
	 
	//================================================================================================================================
	//================================================================================================================================
	bool Weapon::Save( Json & _json ) const
	{
		Actor::Save(	_json );

		Serializable::SavePrefabPtr(	_json, "bulletPrefab", m_bulletPrefab );
		Serializable::SaveFloat(		_json, "scale", m_scale );  
		Serializable::SaveFloat(		_json, "speed", m_speed );
		Serializable::SaveVec3(		_json, "offset", m_offset );
		Serializable::SaveFloat(		_json, "lifeTime", m_lifeTime );
		Serializable::SaveFloat(		_json, "bullets_per_second", m_bulletsPerSecond );
		Serializable::SaveFloat(		_json, "bullet_energy_cost", m_bulletEnergyCost );
		Serializable::SaveFloat(		_json, "bullet_damage", m_bulletDamage );
		Serializable::SaveFloat(		_json, "exposion_speed", m_exposionSpeed );
		Serializable::SaveFloat(		_json, "explosion_time", m_explosionTime );
		Serializable::SaveInt(		_json, "particlesPerExplosion", m_particlesPerExplosion );
		return true;
	}
	 
	//================================================================================================================================
	//================================================================================================================================
	bool Weapon::Load( const Json & _json )
	{
		Actor::Load( _json );
		Serializable::LoadPrefabPtr(	_json, "bulletPrefab", m_bulletPrefab );
		Serializable::LoadFloat( _json,"scale", m_scale );
		Serializable::LoadFloat( _json,"speed", m_speed );
		Serializable::LoadVec3( _json, "offset", m_offset );
		Serializable::LoadFloat( _json, "lifeTime", m_lifeTime );
		Serializable::LoadFloat( _json, "bullets_per_second", m_bulletsPerSecond );
		Serializable::LoadFloat( _json, "bullet_energy_cost", m_bulletEnergyCost );
		Serializable::LoadFloat( _json, "bullet_damage", m_bulletDamage );

		Serializable::LoadFloat( _json, "exposion_speed", m_exposionSpeed );
		Serializable::LoadFloat( _json, "explosion_time", m_explosionTime );
		Serializable::LoadInt( _json, "particlesPerExplosion", m_particlesPerExplosion );
		return true;
	}


}