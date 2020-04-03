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

		spaceship.fastForwardParticlesL.Init( _world );
		spaceship.fastForwardParticlesR.Init( _world );
		spaceship.slowForwardParticlesL.Init( _world );
		spaceship.slowForwardParticlesR.Init( _world );
		spaceship.reverseParticles.Init( _world );
		spaceship.leftParticles.Init( _world );
		spaceship.rightParticles.Init( _world );
	}

	//================================================================================================================================
	//================================================================================================================================
	void SpaceShip::OnGui( EcsWorld& _world, EntityID _entityID, Component& _component )
	{
		SpaceShip& spaceship = static_cast<SpaceShip&>( _component );

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			ImGui::DragFloat( "reverse force",					&spaceship.forwardForces[ SpeedMode::REVERSE ], 1.f, 0.f, 100000.f );
			ImGui::DragFloat( "slow forward force",				&spaceship.forwardForces[ SpeedMode::SLOW ], 1.f, 0.f, 100000.f );
			ImGui::DragFloat( "normal forward force",			&spaceship.forwardForces[ SpeedMode::NORMAL ], 1.f, 0.f, 100000.f );
			ImGui::DragFloat( "fast forward force",				&spaceship.forwardForces[ SpeedMode::FAST ], 1.f, 0.f, 100000.f );
			ImGui::DragFloat( "lateral force",					&spaceship.lateralForce, 1.f, 0.f, 100000.f );
			ImGui::DragFloat( "active drag",					&spaceship.activeDrag, 0.001f, 0.f, 1.f );
			ImGui::DragFloat( "passive drag",					&spaceship.passiveDrag, 0.001f, 0.f, 1.f );
			ImGui::DragFloat( "energyConsumedPerUnitOfForce",	&spaceship.energyConsumedPerUnitOfForce, 0.0001f, 0.f, 1.f );
			ImGui::DragFloat( "planet damage",					&spaceship.planetDamage );
			ImGui::DragFloat( "collision repulsion force",		&spaceship.collisionRepulsionForce );

			ImGui::Spacing();
			 
			ImGui::FanComponent( "fast forward particles L", spaceship.fastForwardParticlesL );
			ImGui::FanComponent( "fast forward particles R", spaceship.fastForwardParticlesR );
			ImGui::FanComponent( "slow forward particles L", spaceship.slowForwardParticlesL );
			ImGui::FanComponent( "slow forward particles R", spaceship.slowForwardParticlesR );
 			ImGui::FanComponent( "reverse particles",		spaceship.reverseParticles );
 			ImGui::FanComponent( "left particles",			spaceship.leftParticles );
 			ImGui::FanComponent( "right particles",			spaceship.rightParticles );
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
		Serializable::SaveComponentPtr( _json, "fast_forward_particlesR", spaceship.fastForwardParticlesR );
		Serializable::SaveComponentPtr( _json, "fast_forward_particlesL", spaceship.fastForwardParticlesL );
		Serializable::SaveComponentPtr( _json, "slow_forward_particlesR", spaceship.slowForwardParticlesR );
		Serializable::SaveComponentPtr( _json, "slow_forward_particlesL", spaceship.slowForwardParticlesL );
 		Serializable::SaveComponentPtr( _json, "reverse_particles", spaceship.reverseParticles );
 		Serializable::SaveComponentPtr( _json, "left_particles", spaceship.leftParticles );
 		Serializable::SaveComponentPtr( _json, "right_particles", spaceship.rightParticles );
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
		Serializable::LoadComponentPtr( _json, "fast_forward_particlesL", spaceship.fastForwardParticlesL );
		Serializable::LoadComponentPtr( _json, "fast_forward_particlesR", spaceship.fastForwardParticlesR );
		Serializable::LoadComponentPtr( _json, "slow_forward_particlesL", spaceship.slowForwardParticlesL );
		Serializable::LoadComponentPtr( _json, "slow_forward_particlesR", spaceship.slowForwardParticlesR );
		Serializable::LoadComponentPtr( _json, "reverse_particles", spaceship.reverseParticles );
		Serializable::LoadComponentPtr( _json, "left_particles", spaceship.leftParticles );
 		Serializable::LoadComponentPtr( _json, "right_particles", spaceship.rightParticles );
	}
}

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
