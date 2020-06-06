#include "game/singletonComponents/fanSpawnManager.hpp"

#include "core/fanDebug.hpp"
#include "game/singletonComponents/fanCollisionManager.hpp"
#include "network/singletonComponents/fanLinkingContext.hpp"
#include "scene/singletonComponents/fanPhysicsWorld.hpp"
#include "scene/singletonComponents/fanScene.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanRigidbody.hpp"
#include "scene/components/fanMotionState.hpp"
#include "scene/components/fanSphereShape.hpp"
#include "game/components/fanWeapon.hpp"
#include "game/singletonComponents/fanGame.hpp"
#include "scene/components/fanSceneNode.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void SpawnManager::SetInfo( EcsSingletonInfo& _info )
	{
		_info.icon = ImGui::NONE;
		_info.name = "spawn manager";
		_info.onGui = &SpawnManager::OnGui;
	}

	//================================================================================================================================
	//================================================================================================================================
	void SpawnManager::Init( EcsWorld& /*_world*/, EcsSingleton& _component )
	{
		SpawnManager& spawnManager = static_cast<SpawnManager&>( _component );
		spawnManager.bullets.clear();
	}

	//================================================================================================================================
	//================================================================================================================================
	void SpawnManager::Spawn( EcsWorld& _world )
	{
		const Game& game = _world.GetSingleton<Game>();
		SpawnManager& spawnManager = _world.GetSingleton<SpawnManager>();
		Scene& scene = _world.GetSingleton<Scene>();
		PhysicsWorld& physicsWorld = _world.GetSingleton<PhysicsWorld>();
		CollisionManager& collisionManager = _world.GetSingleton<CollisionManager>();
		const LinkingContext& linkingContext = _world.GetSingleton<LinkingContext>();

		// spawn bullets
		for (int bulletIndex = int(spawnManager.bullets.size()) - 1; bulletIndex >= 0 ; bulletIndex--)
		{
			const ClientRPC::BulletSpawnInfo& spawnInfo = spawnManager.bullets[bulletIndex];
			if( game.frameIndex < spawnInfo.spawnFrameIndex )
			{
				continue; // spawn will happen later
			}
			else if( game.frameIndex == spawnInfo.spawnFrameIndex )
			{
				// spawn the bullet now
				const EcsHandle ownerHandle = linkingContext.netIDToEcsHandle.at( spawnInfo.owner );
				const EcsEntity ownerEntity = _world.GetEntity( ownerHandle );
				const Weapon& ownerWeapon = _world.GetComponent<Weapon>( ownerEntity );
				const Rigidbody& ownerRigidbody = _world.GetComponent<Rigidbody>( ownerEntity );

				// creates the bullet
				if( *ownerWeapon.bulletPrefab != nullptr )
				{
					SceneNode& node = *ownerWeapon.bulletPrefab->Instanciate( scene.GetRootNode() );
					EcsEntity bulletID = _world.GetEntity( node.handle );

					Transform& bulletTransform = _world.GetComponent<Transform>( bulletID );
					bulletTransform.SetPosition( spawnInfo.position );

					Rigidbody& bulletRigidbody = _world.GetComponent<Rigidbody>( bulletID );
					bulletRigidbody.onContactStarted.Connect( &CollisionManager::OnBulletContact, &collisionManager );
					bulletRigidbody.SetIgnoreCollisionCheck( ownerRigidbody, true );
					bulletRigidbody.SetVelocity( spawnInfo.velocity );
					bulletRigidbody.SetMotionState( _world.GetComponent<MotionState>( bulletID ).motionState );
					bulletRigidbody.SetCollisionShape( _world.GetComponent<SphereShape>( bulletID ).sphereShape );
					bulletRigidbody.SetTransform( bulletTransform.transform );
					physicsWorld.dynamicsWorld->addRigidBody( bulletRigidbody.rigidbody );
				}
			}
			else // game.frameIndex > spawnInfo.spawnFrameIndex
			{
				// we missed the spawn :'(
				// @todo rollback client to spawn the bullet correctly 
				Debug::Warning() << "missed bullet spawning for frame" << spawnInfo.spawnFrameIndex << Debug::Endl();
			}
			spawnManager.bullets.erase( spawnManager.bullets.begin() + bulletIndex );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void SpawnManager::OnGui( EcsWorld&, EcsSingleton& _component )
	{
		const SpawnManager& spawnManager = static_cast<const SpawnManager&>( _component );
		ImGui::Text( "pending bullets: %d", spawnManager.bullets.size() );
	}
}