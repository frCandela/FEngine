#include "game/systems/fanUpdateWeapons.hpp"

#include "ecs/fanEcsWorld.hpp"
#include "scene/components/fanTransform.hpp"
#include "game/components/fanPlayerInput.hpp"
#include "game/components/fanBattery.hpp"
#include "scene/components/fanRigidbody.hpp"
#include "game/components/fanWeapon.hpp"
#include "game/singletonComponents/fanSpawnManager.hpp"
#include "game/singletonComponents/fanGame.hpp"
#include "network/singletonComponents/fanLinkingContext.hpp"
#include "network/systems/fanHostReplication.hpp"
#include "network/components/fanClientRPC.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	EcsSignature S_FireWeapons::GetSignature( const EcsWorld& _world )
	{
		return
			_world.GetSignature<Transform>()   |
			_world.GetSignature<PlayerInput>() |
			_world.GetSignature<Battery>() |
			_world.GetSignature<Rigidbody>() |
			_world.GetSignature<Weapon>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_FireWeapons::Run( EcsWorld& _world, const EcsView& _view, const float _delta )
	{
		if( _delta == 0.f ) { return; }		

		const Game& game = _world.GetSingleton<Game>();
		SpawnManager& spawnManager = _world.GetSingleton<SpawnManager>();
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
				
				// Adds bullet to the spawn manager for spawning on hosts
				if( game.IsServer() )
				{
					const EcsHandle ownerHandle = _world.GetHandle( transformIt.Entity() );
					const NetID ownerID = linkingContext.EcsHandleToNetID.at( ownerHandle );
					const btVector3 bulletPosition = transform.GetPosition() + transform.TransformDirection( weapon.originOffset );
					const btVector3 bulletVelocity = rigidbody.GetVelocity() + weapon.bulletSpeed * transform.Forward();
					const ClientRPC::BulletSpawnInfo info = { ownerID,bulletPosition,bulletVelocity, game.frameIndex + 5 };
					spawnManager.bullets.push_back( info );

					S_ReplicateOnAllHosts::Run( _world, _world.Match( S_ReplicateOnAllHosts::GetSignature( _world ) ),
						ClientRPC::RPCSpawnBullet( info ), HostReplication::ResendUntilReplicated );

				}			
			}
		}
	}
}