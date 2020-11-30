#include "core/ecs/fanEcsSystem.hpp"
#include "engine/components/fanTransform.hpp"
#include "engine/components/fanRigidbody.hpp"
#include "network/components/fanHostPersistentHandle.hpp"
#include "network/systems/fanHostReplication.hpp"
#include "network/singletons/fanTime.hpp"
#include "project_spaceships/components/fanPlayerInput.hpp"
#include "project_spaceships/components/fanBattery.hpp"
#include "project_spaceships/components/fanWeapon.hpp"
#include "project_spaceships/spawn/fanSpawnBullet.hpp"

namespace fan
{
	//========================================================================================================
	// fires the weapons :
	// creates bullets at the proper position & speed depending on player input
	//========================================================================================================
	struct SFireWeapons : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return
				_world.GetSignature<Transform>()	|
				_world.GetSignature<PlayerInput>()	|
				_world.GetSignature<Battery>()		|
				_world.GetSignature<Rigidbody>()	|
				_world.GetSignature<Weapon>();
		}
		static void Run( EcsWorld& _world, const EcsView& _view, const float _delta )
		{
			if( _delta == 0.f ) { return; }

			const Time& time = _world.GetSingleton<Time>();
			const Game& game = _world.GetSingleton<Game>();
			const LinkingContext& linkingContext = _world.GetSingleton<LinkingContext>();

			auto transformIt = _view.begin<Transform>();
			auto rigidbodyIt = _view.begin<Rigidbody>();
			auto inputIt = _view.begin<PlayerInput>();
			auto weaponIt = _view.begin<Weapon>();
			auto batteryIt = _view.begin<Battery>();
			for( ; transformIt != _view.end<Transform>();
                   ++transformIt, ++rigidbodyIt, ++inputIt, ++weaponIt, ++batteryIt )
			{
				const Transform& transform = *transformIt;
				const Rigidbody& rigidbody = *rigidbodyIt;
				const PlayerInput& input = *inputIt;
				Weapon& weapon = *weaponIt;
				Battery& battery = *batteryIt;

				weapon.bBulletsTimeAccumulator += _delta * weapon.mBulletsPerSecond;
				if( weapon.bBulletsTimeAccumulator > 1.f ) { weapon.bBulletsTimeAccumulator = 1.f; }

                if( input.mFire > 0 &&
                    weapon.bBulletsTimeAccumulator >= 1.f &&
                    battery.mCurrentEnergy >= weapon.mBulletEnergyCost )
				{
					--weapon.bBulletsTimeAccumulator;
					battery.mCurrentEnergy -= weapon.mBulletEnergyCost;

					const EcsEntity spaceshipEntity = transformIt.GetEntity();
					const EcsHandle ownerHandle = _world.GetHandle( spaceshipEntity );
                    const NetID     ownerID        = linkingContext.mEcsHandleToNetID.at( ownerHandle );
                    const btVector3 bulletPosition = transform.GetPosition() +
                                                     transform.TransformDirection( weapon.mOriginOffset );
                    const btVector3 bulletVelocity = rigidbody.GetVelocity() +
                                                     weapon.mBulletSpeed * transform.Forward();
					spawn::SpawnBullet::Instanciate( _world, ownerID, bulletPosition, bulletVelocity );

					// Adds bullet to the spawn manager for spawning on hosts
					if( game.mIsServer )
					{
						// spawn on all hosts
						const EcsHandle hostHandle = _world.GetComponent<HostPersistentHandle>( spaceshipEntity ).mHandle;
                        const SpawnInfo info = spawn::SpawnBullet::GenerateInfo( time.mFrameIndex,
                                                                                 ownerID,
                                                                                 bulletPosition,
                                                                                 bulletVelocity );
                        _world.Run<SReplicateOnAllHosts>( ClientRPC::RPCSpawn( info ),
                                                          HostReplication::ResendUntilReplicated,
                                                          hostHandle );
					}
				}
			}
		}
	};
}