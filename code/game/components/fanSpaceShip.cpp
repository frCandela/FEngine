#include "game/components/fanSpaceShip.hpp"

#include "scene/fanSceneSerializable.hpp"

namespace fan
{
	REGISTER_COMPONENT( SpaceShip, "spaceship" );

	//================================================================================================================================
	//================================================================================================================================
	void SpaceShip::SetInfo( ComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::SPACE_SHIP16;
		_info.onGui = &SpaceShip::OnGui;
		_info.init = &SpaceShip::Init;
		_info.load = &SpaceShip::Load;
		_info.save = &SpaceShip::Save;
		_info.editorPath = "game/player/";
	}

	//================================================================================================================================
	//================================================================================================================================
	void SpaceShip::Init( EcsWorld& _world, Component& _component )
	{
		SpaceShip& spaceship					= static_cast<SpaceShip&>( _component );

		spaceship.forwardForces					= btVector4( 1000.f, 1000.f, 2000.f, 3500.f );
		spaceship.lateralForce					= 2000.f;
		spaceship.activeDrag					= 0.930f;
		spaceship.passiveDrag					= 0.950f;
		spaceship.energyConsumedPerUnitOfForce	= 0.001f;
		spaceship.remainingChargeEnergy			= 0.f;
		spaceship.planetDamage					= 5.f;
		spaceship.collisionRepulsionForce		= 500.f;
	}

	//================================================================================================================================
	//================================================================================================================================
	void SpaceShip::OnGui( EcsWorld& _world, EntityID _entityID, Component& _component )
	{
		SpaceShip& spaceship = static_cast<SpaceShip&>( _component );

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			ImGui::DragFloat( "reverse force", &spaceship.forwardForces[ SpeedMode::REVERSE ], 1.f, 0.f, 100000.f );
			ImGui::DragFloat( "slow forward force", &spaceship.forwardForces[ SpeedMode::SLOW ], 1.f, 0.f, 100000.f );
			ImGui::DragFloat( "normal forward force", &spaceship.forwardForces[ SpeedMode::NORMAL ], 1.f, 0.f, 100000.f );
			ImGui::DragFloat( "fast forward force", &spaceship.forwardForces[ SpeedMode::FAST ], 1.f, 0.f, 100000.f );
			ImGui::DragFloat( "lateral force", &spaceship.lateralForce, 1.f, 0.f, 100000.f );
			ImGui::DragFloat( "active drag", &spaceship.activeDrag, 0.001f, 0.f, 1.f );
			ImGui::DragFloat( "passive drag", &spaceship.passiveDrag, 0.001f, 0.f, 1.f );
			ImGui::DragFloat( "energyConsumedPerUnitOfForce", &spaceship.energyConsumedPerUnitOfForce, 0.0001f, 0.f, 1.f );
			ImGui::DragFloat( "planet damage", &spaceship.planetDamage );
			ImGui::DragFloat( "collision repulsion force", &spaceship.collisionRepulsionForce );

// 			ImGui::FanComponent( "fast forward particles", spaceship.fastForwardParticles );
// 			ImGui::FanComponent( "slow forward particles", spaceship.slowForwardParticles );
// 			ImGui::FanComponent( "reverse particles", spaceship.reverseParticles );
// 			ImGui::FanComponent( "left particles", spaceship.leftParticles );
// 			ImGui::FanComponent( "right particles", spaceship.rightParticles );
		} ImGui::PopItemWidth();	
	}

	//================================================================================================================================
	//================================================================================================================================
	void SpaceShip::Save( const Component& _component, Json& _json )
	{
		const SpaceShip& spaceship = static_cast<const SpaceShip&>( _component );

		Serializable::SaveVec4( _json, "forward_forces", spaceship.forwardForces );
		Serializable::SaveFloat( _json, "lateral_force", spaceship.lateralForce );
		Serializable::SaveFloat( _json, "active_drag", spaceship.activeDrag );
		Serializable::SaveFloat( _json, "passive_drag", spaceship.passiveDrag );
		Serializable::SaveFloat( _json, "energy_consumed_per_unit_of_force", spaceship.energyConsumedPerUnitOfForce );
		Serializable::SaveFloat( _json, "planet_damage", spaceship.planetDamage );
		Serializable::SaveFloat( _json, "collision_repulsion_force", spaceship.collisionRepulsionForce );
// 		Serializable::SaveComponentPtr( _json, "fast_forward_particles", m_fastForwardParticles );
// 		Serializable::SaveComponentPtr( _json, "slow_forward_particles", m_slowForwardParticles );
// 		Serializable::SaveComponentPtr( _json, "reverse_particles", m_reverseParticles );
// 		Serializable::SaveComponentPtr( _json, "left_particles", m_leftParticles );
// 		Serializable::SaveComponentPtr( _json, "right_particles", m_rightParticles );
	}

	//================================================================================================================================
	//================================================================================================================================
	void SpaceShip::Load( Component& _component, const Json& _json )
	{
		SpaceShip& spaceship = static_cast<SpaceShip&>( _component );

		Serializable::LoadVec4( _json, "forward_forces", spaceship.forwardForces );
		Serializable::LoadFloat( _json, "lateral_force", spaceship.lateralForce );
		Serializable::LoadFloat( _json, "active_drag", spaceship.activeDrag );
		Serializable::LoadFloat( _json, "passive_drag", spaceship.passiveDrag );
		Serializable::LoadFloat( _json, "energy_consumed_per_unit_of_force", spaceship.energyConsumedPerUnitOfForce );
		Serializable::LoadFloat( _json, "planet_damage", spaceship.planetDamage );
		Serializable::LoadFloat( _json, "collision_repulsion_force", spaceship.collisionRepulsionForce );
// 		Serializable::LoadComponentPtr( _json, m_gameobject->GetScene(), "fast_forward_particles", spaceship.fastForwardParticles );
// 		Serializable::LoadComponentPtr( _json, m_gameobject->GetScene(), "slow_forward_particles", spaceship.slowForwardParticles );
// 		Serializable::LoadComponentPtr( _json, m_gameobject->GetScene(), "reverse_particles", spaceship.reverseParticles );
// 		Serializable::LoadComponentPtr( _json, m_gameobject->GetScene(), "left_particles", spaceship.leftParticles );
// 		Serializable::LoadComponentPtr( _json, m_gameobject->GetScene(), "right_particles", spaceship.rightParticles );
	}
}

namespace fan
{

// 		//================================================================================================================================
// 		//================================================================================================================================
// 		void SpaceShip::Start()
// 	{
// 		REQUIRE_COMPONENT( WithEnergy, m_energy )
// 			REQUIRE_COMPONENT( Rigidbody, m_rigidbody )
// 			REQUIRE_COMPONENT( PlayerInput, m_input );
// 		REQUIRE_COMPONENT( Health, m_health );
//
// 		REQUIRE_TRUE( *m_fastForwardParticles != nullptr, "Spaceship: missing particles reference" );
// 		REQUIRE_TRUE( *m_slowForwardParticles != nullptr, "Spaceship: missing particles reference" );
// 		REQUIRE_TRUE( *m_reverseParticles != nullptr, "Spaceship: missing particles reference" );
// 		REQUIRE_TRUE( *m_leftParticles != nullptr, "Spaceship: missing particles reference" );
// 		REQUIRE_TRUE( *m_rightParticles != nullptr, "Spaceship: missing particles reference" );
//
// 		if ( *m_fastForwardParticles != nullptr ) { m_fastForwardParticles->SetEnabled( false ); }
// 		if ( *m_slowForwardParticles != nullptr ) { m_slowForwardParticles->SetEnabled( false ); }
// 		if ( *m_reverseParticles != nullptr ) { m_reverseParticles->SetEnabled( false ); }
// 		if ( *m_leftParticles != nullptr ) { m_leftParticles->SetEnabled( false ); }
// 		if ( *m_rightParticles != nullptr ) { m_rightParticles->SetEnabled( false ); }
// 
// 		if ( m_health )
// 		{
// 			m_health->AddHealth( m_health->GetMaxHealth() );
// 			m_health->onFallToZero.Connect( &SpaceShip::Die, this );
// 		}
// 		if ( m_energy ) { m_energy->AddEnergy( m_energy->GetMaxEnergy() ); }
// 		if ( m_rigidbody != nullptr ) { m_rigidbody->onContactStarted.Connect( &SpaceShip::OnContactStarted, this ); }
// 	}
// 
//
// 	//================================================================================================================================
// 	//================================================================================================================================
// 	void  SpaceShip::OnContactStarted( Rigidbody* _rb, btPersistentManifold* const& )
// 	{
// 		float damage = 0.f;
// 
// 		if ( ecsBullet* bullet = _rb->GetGameobject().GetEcsComponent<ecsBullet>() )
// 		{
// 			//Debug::Log("bullet");
// 			damage = bullet->damage;
// 		}
// 		else if ( _rb->GetGameobject().GetComponent<Planet>() )
// 		{
// 			btVector3 dir = m_gameobject->GetTransform().GetPosition() - _rb->GetGameobject().GetTransform().GetPosition();
// 			if ( !dir.fuzzyZero() )
// 			{
// 				m_rigidbody->ApplyCentralForce( m_collisionRepulsionForce * dir.normalized() );
// 			}
// 
// 			//Debug::Log("planet");
// 			damage = m_planetDamage;
// 		}
// 		else if ( _rb->GetGameobject().GetComponent<SolarSystem>() )
// 		{
// 			//Debug::Log( "sun" );
// 			damage = m_health->GetHealth() + 1.f;
// 		}
// 		else
// 		{
// 			//Debug::Log( "other" );
// 		}
// 
// 		// Death
// 		if ( m_health->GetHealth() > 0 && !m_health->TryRemoveHealth( damage ) )
// 
// 		{
// 			m_health->TryRemoveHealth( m_health->GetHealth() );
// 		}
//	}

// 	//================================================================================================================================
// 	//================================================================================================================================
// 	void SpaceShip::Die()
// 	{
// 		Debug::Log( "dead" );
// 
// 		if ( GetScene().IsServer() == false )
// 		{
// 			// Explosion
// 			std::default_random_engine			  m_generator;
// 			std::uniform_real_distribution<float> m_distribution( 0.f, 1.f );
// 			for ( int particleIndex = 0; particleIndex < 1000; particleIndex++ )
// 			{
// 				EcsManager& ecs = m_gameobject->GetScene().GetEcsManager();
// 				ecsEntity entity = ecs.CreateEntity();
// 				ecsPosition& position = ecs.AddComponent<ecsPosition>( entity );
// 				ecs.AddComponent<ecsRotation>( entity ).Init();
// 				ecsMovement& movement = ecs.AddComponent<ecsMovement>( entity );
// 				ecsParticle& particle = ecs.AddComponent<ecsParticle>( entity );
// 
// 				movement.speed = btVector3( m_distribution( m_generator ), m_distribution( m_generator ), m_distribution( m_generator ) ) - btVector3( 0.5f, 0.5f, 0.5f );
// 				movement.speed.normalize();
// 				movement.speed *= m_distribution( m_generator ) * 10.f + 10.f;
// 				position.position = m_gameobject->GetTransform().GetPosition();
// 				particle.durationLeft = 3.f;
// 			}
// 		}
// 
// 		onPlayerDie.Emmit( m_gameobject );
//	}
}