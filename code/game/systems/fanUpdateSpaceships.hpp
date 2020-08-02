#include "ecs/fanEcsSystem.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanRigidbody.hpp"
#include "scene/components/fanSceneNode.hpp"

#include "game/components/fanBattery.hpp"
#include "game/components/fanPlayerInput.hpp"
#include "game/components/fanSpaceShip.hpp"
#include "game/components/fanSolarPanel.hpp"
#include "game/components/fanHealth.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// moves the spaceships
	// @todo split this in multiple sub systems for force application & energy management
	//==============================================================================================================================================================
	struct S_MoveSpaceships : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return 
				_world.GetSignature<SceneNode>() |
				_world.GetSignature<Transform>() |
				_world.GetSignature<Rigidbody>() |
				_world.GetSignature<Battery>() |
				_world.GetSignature<PlayerInput>() |
				_world.GetSignature<SpaceShip>();
		}

		static void Run( EcsWorld& /*_world*/, const EcsView& _view, const float _delta )
		{
			if( _delta == 0.f ) { return; }

			auto rbIt = _view.begin<Rigidbody>();
			auto transformIt = _view.begin<Transform>();
			auto spaceshipIt = _view.begin<SpaceShip>();
			auto batteryIt = _view.begin<Battery>();
			auto playerInputIt = _view.begin<PlayerInput>();
			auto sceneNodeIt = _view.begin<SceneNode>();
			for( ; rbIt != _view.end<Rigidbody>(); ++rbIt, ++transformIt, ++spaceshipIt, ++batteryIt, ++playerInputIt, ++sceneNodeIt )
			{
				const SpaceShip& spaceship = *spaceshipIt;
				const PlayerInput& playerInput = *playerInputIt;
				Rigidbody& rb = *rbIt;
				Transform& transform = *transformIt;
				Battery& battery = *batteryIt;

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
				else if( boost > 0 ) { speedMode = SpaceShip::FAST; }
				else if( boost < 0 ) { speedMode = SpaceShip::SLOW; }
				else { speedMode = SpaceShip::NORMAL; }

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
					spaceship.reverseParticles != nullptr &&
					spaceship.leftParticles != nullptr &&
					spaceship.rightParticles != nullptr
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
						else if( speedMode == SpaceShip::FAST )
						{
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
				rb.rigidbody->applyCentralForce( leftForce * transform.Left() );
				rb.rigidbody->applyCentralForce( spaceship.forwardForces[speedMode] * forwardAxis * transform.Forward() );
				rb.rigidbody->setAngularVelocity( btVector3::Zero() );

				// Drag
				btVector3 newVelocity = ( totalConsumption > 0.f ? spaceship.activeDrag : spaceship.passiveDrag )* rb.GetVelocity();
				newVelocity.setY( 0.f );
				rb.SetVelocity( newVelocity );

				SceneNode& sceneNode = *sceneNodeIt;
				sceneNode.AddFlag( SceneNode::BoundsOutdated );
			}
		}
	};

	//==============================================================================================================================================================
	// Deals damage to the health entities that are in the sunlight when the sun is bursting
	//==============================================================================================================================================================
	struct S_EruptionDamage : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return  _world.GetSignature<SolarPanel>() |
				_world.GetSignature<Health>() |
				_world.GetSignature<SpaceShip>();
		}

		static void Run( EcsWorld& _world, const EcsView& _view, const float _delta )
		{
			if( _delta == 0.f ) { return; }

			const SolarEruption& eruption = _world.GetSingleton<SolarEruption>();

			auto healthIt = _view.begin<Health>();
			auto solarPanelIt = _view.begin<SolarPanel>();
			for( ; healthIt != _view.end<Health>(); ++healthIt, ++solarPanelIt )
			{
				Health& health = *healthIt;
				const SolarPanel& solarPanel = *solarPanelIt;
				if( !health.invincible && solarPanel.isInSunlight && eruption.state == SolarEruption::EXPODING )
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
	};

	//==============================================================================================================================================================
	// checks if any player is death and makes an explosion
	//==============================================================================================================================================================
	struct S_PlayerDeath : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return	_world.GetSignature<Health>() |
				_world.GetSignature<Transform>() |
				_world.GetSignature<SpaceShip>();
		}

		static void Run( EcsWorld& _world, const EcsView& _view, const float _delta )
		{
			if( _delta == 0.f ) { return; }

			auto healthIt = _view.begin<Health>();
			auto transformIt = _view.begin<Transform>();
			auto spaceShipIt = _view.begin<SpaceShip>();
			for( ; healthIt != _view.end<Health>(); ++healthIt, ++transformIt, ++spaceShipIt )
			{
				const Health& health = *healthIt;
				const Transform& transform = *transformIt;
				const SpaceShip& spaceShip = *spaceShipIt;
				const EcsEntity entity = healthIt.GetEntity();

				if( health.currentHealth == 0.f && !health.invincible )
				{
					Scene& scene = _world.GetSingleton<Scene>();

					if( spaceShip.deathFx == nullptr )
					{
						Debug::Error() << "S_PlayerDeath: SpaceShip has no death fx" << Debug::Endl();
					}
					else
					{
						SceneNode& fxNode = *spaceShip.deathFx->Instantiate( scene.GetRootNode() );
						EcsEntity fxId = _world.GetEntity( fxNode.handle );
						Transform& fxTransform = _world.GetComponent<Transform>( fxId );
						fxTransform.SetPosition( transform.GetPosition() );
					}

					_world.Kill( entity );
				}
			}
		}

	};
}