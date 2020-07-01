#include "ecs/fanEcsSystem.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanRigidbody.hpp"
#include "network/components/fanHostPersistentHandle.hpp"
#include "network/systems/fanHostReplication.hpp"
#include "network/singletons/fanTime.hpp"
#include "game/components/fanPlayerInput.hpp"
#include "game/components/fanBattery.hpp"
#include "game/components/fanWeapon.hpp"
#include "game/spawn/fanSpawnBullet.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// fires the weapons :
	// creates bullets at the proper position & speed depending on player input
	//==============================================================================================================================================================
	struct S_FireWeapons : EcsSystem
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
			for( ; transformIt != _view.end<Transform>(); ++transformIt, ++rigidbodyIt, ++inputIt, ++weaponIt, ++batteryIt )
			{
				const Transform& transform = *transformIt;
				const Rigidbody& rigidbody = *rigidbodyIt;
				const PlayerInput& input = *inputIt;
				Weapon& weapon = *weaponIt;
				Battery& battery = *batteryIt;

				weapon.bulletsAccumulator += _delta * weapon.bulletsPerSecond;
				if( weapon.bulletsAccumulator > 1.f ) { weapon.bulletsAccumulator = 1.f; }

				if( input.fire > 0 && weapon.bulletsAccumulator >= 1.f && battery.currentEnergy >= weapon.bulletEnergyCost )
				{
					--weapon.bulletsAccumulator;
					battery.currentEnergy -= weapon.bulletEnergyCost;

					const EcsEntity spaceshipEntity = transformIt.GetEntity();
					const EcsHandle ownerHandle = _world.GetHandle( spaceshipEntity );
					const NetID ownerID = linkingContext.ecsHandleToNetID.at( ownerHandle );
					const btVector3 bulletPosition = transform.GetPosition() + transform.TransformDirection( weapon.originOffset );
					const btVector3 bulletVelocity = rigidbody.GetVelocity() + weapon.bulletSpeed * transform.Forward();
					spawn::SpawnBullet::Instanciate( _world, ownerID, bulletPosition, bulletVelocity );

					// Adds bullet to the spawn manager for spawning on hosts
					if( game.IsServer() )
					{
						// spawn on all hosts
						const EcsHandle hostHandle = _world.GetComponent<HostPersistentHandle>( spaceshipEntity ).handle;
						const SpawnInfo info = spawn::SpawnBullet::GenerateInfo( time.frameIndex, ownerID, bulletPosition, bulletVelocity );
						_world.Run<S_ReplicateOnAllHosts>( ClientRPC::RPCSpawn( info ), HostReplication::ResendUntilReplicated, hostHandle );
					}
				}
			}
		}
	};
}