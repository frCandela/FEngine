#include "game/systems/fanUpdateSpaceships.hpp"

#include "scene/components/fanTransform.hpp"
#include "scene/components/fanRigidbody.hpp"
#include "scene/fanSceneTags.hpp"
#include "game/components/fanSpaceShip.hpp"
#include "game/components/fanPlayerInput.hpp"
#include "game/components/fanBattery.hpp"
#include "ecs/fanEcsWorld.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Signature S_UpdateSpaceships::GetSignature( const EcsWorld& _world )
	{
		return
			_world.GetSignature<Transform>() |
			_world.GetSignature<Rigidbody>() |
			_world.GetSignature<Battery>() |
			_world.GetSignature<PlayerInput>() |
			_world.GetSignature<SpaceShip>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_UpdateSpaceships::Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta )
	{
		if( _delta == 0.f ) { return; }

		for( EntityID entityID : _entities )
		{
			Transform& transform = _world.GetComponent<Transform>( entityID );
			Rigidbody& rigidbody = _world.GetComponent<Rigidbody>( entityID );
			SpaceShip& spaceship = _world.GetComponent<SpaceShip>( entityID );
			Battery& battery = _world.GetComponent<Battery>( entityID );
			PlayerInput & playerInput = _world.GetComponent<PlayerInput>( entityID );

			// get player input
			const btVector3 direction = playerInput.inputData.direction;
			const bool  stop = playerInput.inputData.stop;
			const float leftForce = _delta * spaceship.lateralForce * playerInput.inputData.left;
			const float forwardAxis = stop ? 0.f : _delta * playerInput.inputData.forward;
			const float boost = playerInput.inputData.boost;

			// Orientation
			if( !direction.isZero() )
			{
				transform.LookAt( transform.GetPosition() + direction, btVector3::Up() );
			}

			// constrain position
			btVector3 pos = transform.GetPosition();
			pos.setY( 0.f );
			transform.SetPosition( pos );

			SpaceShip::SpeedMode speedMode;
			if( forwardAxis < 0 ) { speedMode = SpaceShip::REVERSE; }
			else if( boost > 0 )  { speedMode = SpaceShip::FAST; }
			else if( boost < 0 )  { speedMode = SpaceShip::SLOW; }
			else				  { speedMode = SpaceShip::NORMAL; }			

			// Consume energy
 			float totalConsumption = spaceship.energyConsumedPerUnitOfForce * ( std::abs( leftForce ) + std::abs( spaceship.forwardForces[speedMode] * forwardAxis ) );
			if( battery.currentEnergy < totalConsumption )
			{
				battery.currentEnergy = 0.f;
				if( speedMode != SpaceShip::REVERSE )
				{
					speedMode = SpaceShip::SLOW;
				}
			}
			else
			{
				battery.currentEnergy -= totalConsumption;
			}

// 			// Enable particle systems
// 			spaceship.fastForwardParticles->SetEnabled( false );
// 			spaceship.slowForwardParticles->SetEnabled( false );
// 			spaceship.reverseParticles->SetEnabled( false );
// 			spaceship.leftParticles->SetEnabled( false );
// 			spaceship.rightParticles->SetEnabled( false );

// 			if( GetScene().IsServer() == false )
// 			{
// 				if( forwardAxis != 0.f )
// 				{
// 					if( speedMode == SpeedMode::SLOW || speedMode == SpeedMode::NORMAL ) { m_slowForwardParticles->SetEnabled( true ); }
// 					else if( speedMode == SpeedMode::FAST ) { m_fastForwardParticles->SetEnabled( true ); }
// 					else if( speedMode == SpeedMode::REVERSE ) { m_reverseParticles->SetEnabled( true ); }
// 				}
// 				if( leftForce > 0.f ) { m_leftParticles->SetEnabled( true ); }
// 				else if( leftForce < 0.f ) { m_rightParticles->SetEnabled( true ); }
// 			}

			// Forces application		
			rigidbody.ApplyCentralForce( leftForce * transform.Left() );
			rigidbody.ApplyCentralForce( spaceship.forwardForces[speedMode] * forwardAxis * transform.Forward() );
			rigidbody.rigidbody.setAngularVelocity( btVector3::Zero() );

			// Drag
			btVector3 newVelocity = ( totalConsumption > 0.f ? spaceship.activeDrag : spaceship.passiveDrag ) * rigidbody.GetVelocity();
			newVelocity.setY( 0.f );
			rigidbody.SetVelocity( newVelocity );

			_world.AddTag<tag_boundsOutdated>( entityID );
		}
	}
}