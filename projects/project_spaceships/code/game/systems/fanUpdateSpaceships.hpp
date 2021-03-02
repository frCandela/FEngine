#include "core/ecs/fanEcsSystem.hpp"
#include "engine/components/fanTransform.hpp"
#include "engine/components/fanRigidbody.hpp"
#include "engine/components/fanSceneNode.hpp"

#include "game/singletons/fanSolarEruption.hpp"
#include "game/components/fanBattery.hpp"
#include "game/components/fanPlayerInput.hpp"
#include "game/components/fanSpaceShip.hpp"
#include "game/components/fanSolarPanel.hpp"
#include "game/components/fanHealth.hpp"

namespace fan
{
	//========================================================================================================
	// moves the spaceships
	// @todo split this in multiple sub systems for force application & energy management
	//========================================================================================================
	struct SMoveSpaceships : EcsSystem
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
			for( ; rbIt != _view.end<Rigidbody>();
                   ++rbIt, ++transformIt, ++spaceshipIt, ++batteryIt, ++playerInputIt, ++sceneNodeIt )
			{
				const SpaceShip& spaceship = *spaceshipIt;
				const PlayerInput& playerInput = *playerInputIt;
				Rigidbody& rb = *rbIt;
				Transform& transform = *transformIt;
				Battery& battery = *batteryIt;

				// get player input
                const btVector3 orientation = btVector3( playerInput.mOrientation.x(),
                                                         0.f,
                                                         playerInput.mOrientation.z() );
				const float leftForce = _delta * spaceship.mLateralForce * playerInput.mLeft;
				const float forwardAxis = _delta * playerInput.mForward;
				const float boost = playerInput.mBoost;

				// Orientation
				if( !orientation.isZero() )
				{
					transform.LookAt( transform.GetPosition() + orientation, btVector3_Up );
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
                float totalConsumption = spaceship.mEnergyConsumedPerUnitOfForce *
                                         ( std::abs( leftForce ) +
                                           std::abs( spaceship.mForwardForces[speedMode] * forwardAxis ) );
				if( battery.mCurrentEnergy < totalConsumption )
				{
					battery.mCurrentEnergy = 0.f;
					if( speedMode != SpaceShip::REVERSE )
					{
						speedMode = SpaceShip::SLOW;
					}
				}
				else
				{
					battery.mCurrentEnergy -= totalConsumption;
				}

				// Enable particle systems
				if( spaceship.mFastForwardParticlesR != nullptr &&
                    spaceship.mFastForwardParticlesL != nullptr &&
                    spaceship.mSlowForwardParticlesR != nullptr &&
                    spaceship.mSlowForwardParticlesL != nullptr &&
                    spaceship.mReverseParticles != nullptr &&
                    spaceship.mLeftParticles != nullptr &&
                    spaceship.mRightParticles != nullptr
					)
				{
					spaceship.mFastForwardParticlesR->mEnabled = false;
					spaceship.mFastForwardParticlesL->mEnabled = false;
					spaceship.mSlowForwardParticlesR->mEnabled = false;
					spaceship.mSlowForwardParticlesL->mEnabled = false;
					spaceship.mReverseParticles->mEnabled      = false;
					spaceship.mLeftParticles->mEnabled         = false;
					spaceship.mRightParticles->mEnabled        = false;


					if( forwardAxis != 0.f )
					{
						if( speedMode == SpaceShip::SLOW || speedMode == SpaceShip::NORMAL )
						{
							spaceship.mSlowForwardParticlesL->mEnabled = true;
							spaceship.mSlowForwardParticlesR->mEnabled = true;
						}
						else if( speedMode == SpaceShip::FAST )
						{
							spaceship.mFastForwardParticlesL->mEnabled = true;
							spaceship.mFastForwardParticlesR->mEnabled = true;
						}
						else if( speedMode == SpaceShip::REVERSE )
						{
							spaceship.mReverseParticles->mEnabled = true;
						}
					}
					if( leftForce < 0.f )
					{
						spaceship.mLeftParticles->mEnabled = true;
					}
					else if( leftForce > 0.f )
					{
						spaceship.mRightParticles->mEnabled = true;
					}
				}

				// Forces application		
				rb.mRigidbody->applyCentralForce( leftForce * transform.Left() );
                rb.mRigidbody->applyCentralForce( spaceship.mForwardForces[speedMode] *
                                                  forwardAxis *
                                                  transform.Forward() );
				rb.mRigidbody->setAngularVelocity( btVector3_Zero );

				// Drag
                btVector3 newVelocity = ( totalConsumption > 0.f ?
                        spaceship.mActiveDrag :
                        spaceship.mPassiveDrag ) * rb.GetVelocity();
				newVelocity.setY( 0.f );
				rb.SetVelocity( newVelocity );

				SceneNode& sceneNode = *sceneNodeIt;
				sceneNode.AddFlag( SceneNode::BoundsOutdated );
			}
		}
	};

	//========================================================================================================
	// Deals damage to the health entities that are in the sunlight when the sun is bursting
	//========================================================================================================
	struct SEruptionDamage : EcsSystem
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
                if( !health.mInvincible &&
                    solarPanel.mIsInSunlight &&
                    eruption.mState == SolarEruption::EXPODING )
				{
					const float damage = _delta * eruption.mDamagePerSecond;
					if( health.mCurrentHealth < damage )
					{
						health.mCurrentHealth = 0.f;
					}
					else
					{
						health.mCurrentHealth -= damage;
					}
				}
			}
		}
	};

	//========================================================================================================
	// checks if any player is death and makes an explosion
	//========================================================================================================
	struct SPlayerDeath : EcsSystem
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

				if( health.mCurrentHealth == 0.f && !health.mInvincible )
				{
					Scene& scene = _world.GetSingleton<Scene>();

					if( spaceShip.mDeathFx == nullptr )
					{
						Debug::Error() << "SPlayerDeath: SpaceShip has no death fx" << Debug::Endl();
					}
					else
					{
						SceneNode& fxNode = *spaceShip.mDeathFx->Instantiate( scene.GetRootNode() );
						EcsEntity fxId = _world.GetEntity( fxNode.mHandle );
						Transform& fxTransform = _world.GetComponent<Transform>( fxId );
						fxTransform.SetPosition( transform.GetPosition() );
					}

					_world.Kill( entity );
				}
			}
		}
	};
}