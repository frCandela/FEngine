#include "game/systems/fanUpdateSpaceships.hpp"

#include "core/fanDebug.hpp"
#include "scene/fanSceneTags.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanRigidbody.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/singletonComponents/fanScene.hpp"
#include "game/components/fanSpaceShip.hpp"
#include "game/components/fanSolarPanel.hpp"
#include "game/components/fanHealth.hpp"
#include "game/components/fanPlayerInput.hpp"
#include "game/components/fanBattery.hpp"
#include "game/singletonComponents/fanSolarEruption.hpp"
#include "ecs/fanEcsWorld.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Signature S_MoveSpaceships::GetSignature( const EcsWorld& _world )
	{
		return _world.GetSignature<Transform>() |
			   _world.GetSignature<Rigidbody>() |
			   _world.GetSignature<Battery>() |
			   _world.GetSignature<PlayerInput>() |
			   _world.GetSignature<SpaceShip>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_MoveSpaceships::Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta )
	{
		if( _delta == 0.f ) { return; }

		for( EntityID entityID : _entities )
		{
			Transform& transform = _world.GetComponent<Transform>( entityID );
			Rigidbody& rb = _world.GetComponent<Rigidbody>( entityID );
			SpaceShip& spaceship = _world.GetComponent<SpaceShip>( entityID );
			Battery& battery = _world.GetComponent<Battery>( entityID );
			PlayerInput & playerInput = _world.GetComponent<PlayerInput>( entityID );

			// get player input
			const btVector3 orientation = btVector3( playerInput.orientation.x(), 0.f, playerInput.orientation.z() );
			const float leftForce = _delta * spaceship.lateralForce * playerInput.left;
			const float forwardAxis = _delta * playerInput.forward;
			const float boost = playerInput.boost;

			// Orientation
			if( !orientation.isZero() )
			{
				transform.LookAt( transform.GetPosition() + orientation, btVector3::Up() );
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
			rb.rigidbody.applyCentralForce( leftForce * transform.Left() );
			rb.rigidbody.applyCentralForce( spaceship.forwardForces[speedMode] * forwardAxis * transform.Forward() );
			rb.rigidbody.setAngularVelocity( btVector3::Zero() );

			// Drag
			btVector3 newVelocity = ( totalConsumption > 0.f ? spaceship.activeDrag : spaceship.passiveDrag ) * rb.GetVelocity();
			newVelocity.setY( 0.f );
			rb.SetVelocity( newVelocity );

			_world.AddTag<tag_boundsOutdated>( entityID );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	Signature S_EruptionDamage::GetSignature( const EcsWorld& _world )
	{
		return  _world.GetSignature<SolarPanel>() |
				_world.GetSignature<Transform>() |
				_world.GetSignature<Health>()	 |
				_world.GetSignature<SpaceShip>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_EruptionDamage::Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta )
	{
		if( _delta == 0.f ) { return; }

		const SolarEruption& eruption = _world.GetSingletonComponent<SolarEruption>();
		for( EntityID entityID : _entities )
		{
			const Transform& transform = _world.GetComponent<Transform>( entityID );
			Health& health = _world.GetComponent<Health>( entityID );
			SolarPanel& solarPanel = _world.GetComponent<SolarPanel>( entityID );

			if( solarPanel.isInSunlight && eruption.state == SolarEruption::EXPODING )
			{
				const float damage = _delta * eruption.damagePerSecond;
				if( health.currentHealth < damage )
				{
					health.currentHealth = 0.f;
				}
				else
				{
					health.currentHealth -= damage;
				}
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	Signature S_PlayerDeath::GetSignature( const EcsWorld& _world )
	{
		return	_world.GetSignature<Health>() |
				_world.GetSignature<Transform>() |
				_world.GetSignature<SpaceShip>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_PlayerDeath::Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta )
	{
		if( _delta == 0.f ) { return; }

		const SolarEruption& eruption = _world.GetSingletonComponent<SolarEruption>();
		for( EntityID entityID : _entities )
		{
			Health& health = _world.GetComponent<Health>( entityID );

			if( health.currentHealth == 0.f )
			{
				Transform& transform = _world.GetComponent<Transform>( entityID );
				SpaceShip& spaceShip = _world.GetComponent<SpaceShip>( entityID );
				Scene& scene = _world.GetSingletonComponent<Scene>();
				
				if( spaceShip.deathFx == nullptr )
				{
					Debug::Error() << "S_PlayerDeath: SpaceShip has no death fx" << Debug::Endl();
				}
				else
				{
					SceneNode& fxNode = *spaceShip.deathFx->Instanciate( *scene.root );
					EntityID fxId = _world.GetEntityID( fxNode.handle );
					Transform& fxTransform = _world.GetComponent<Transform>( fxId );
					fxTransform.SetPosition( transform.GetPosition() );
				}

				_world.KillEntity( entityID );
			}
		}
	}
}