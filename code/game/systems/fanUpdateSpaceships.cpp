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

			// Enable particle systems
			if( spaceship.fastForwardParticlesR != nullptr &&
				spaceship.fastForwardParticlesL != nullptr &&
				spaceship.slowForwardParticlesR != nullptr &&
				spaceship.slowForwardParticlesL != nullptr &&
 				spaceship.reverseParticles		!= nullptr &&
 				spaceship.leftParticles			!= nullptr &&
 				spaceship.rightParticles		!= nullptr 
			)
			{
				spaceship.fastForwardParticlesR->enabled = false;
				spaceship.fastForwardParticlesL->enabled = false;
				spaceship.slowForwardParticlesR->enabled = false;
				spaceship.slowForwardParticlesL->enabled = false;
 				spaceship.reverseParticles->enabled = false;
 				spaceship.leftParticles->enabled = false;
 				spaceship.rightParticles->enabled = false;


				if( forwardAxis != 0.f )
				{
					if( speedMode == SpaceShip::SLOW || speedMode == SpaceShip::NORMAL )
					{ 
						spaceship.slowForwardParticlesL->enabled = true;
						spaceship.slowForwardParticlesR->enabled = true;
					}
					else if( speedMode == SpaceShip::FAST ) { 
						spaceship.fastForwardParticlesL->enabled = true;
						spaceship.fastForwardParticlesR->enabled = true;
					}
					else if( speedMode == SpaceShip::REVERSE ) 
					{ 
						spaceship.reverseParticles->enabled = true; 
					}
				}
				if( leftForce < 0.f ) 
				{ 
					spaceship.leftParticles->enabled = true; 
				}
				else if( leftForce > 0.f )
				{ 
					spaceship.rightParticles->enabled = true; 
				}
			}

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