#include "network/singletons/fanSpawnManager.hpp"

#include "game/singletons/fanCollisionManager.hpp"
#include "network/singletons/fanLinkingContext.hpp"
#include "scene/singletons/fanPhysicsWorld.hpp"
#include "scene/singletons/fanScene.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanRigidbody.hpp"
#include "scene/components/fanMotionState.hpp"
#include "scene/components/fanSphereShape.hpp"
#include "game/components/fanWeapon.hpp"
#include "scene/components/fanSceneNode.hpp"

namespace fan
{
	namespace spawn
	{

		//================================================================================================================================
		//================================================================================================================================
		struct SpawnBullet
		{
			static const SpawnIdType  id = 'BLET';

			//================================================================
			//================================================================
			static void WriteInfo( sf::Packet& _data, const NetID _owner, const  btVector3 _position, const btVector3 _velocity )
			{
				// Write data to packet
				_data << id;
				_data << _position[0] << _position[2];
				_data << _velocity[0] << _velocity[2];
				_data << _owner;
			}

			//================================================================
			//================================================================
			static void Spawn( EcsWorld& _world, sf::Packet _data )
			{
				// Get data from packet
				NetID			owner;
				btVector3		position( 0.f, 0.f, 0.f );
				btVector3		velocity( 0.f, 0.f, 0.f );
				_data >> position[0] >> position[2];
				_data >> velocity[0] >> velocity[2];
				_data >> owner;

				Scene& scene = _world.GetSingleton<Scene>();
				PhysicsWorld& physicsWorld = _world.GetSingleton<PhysicsWorld>();
				CollisionManager& collisionManager = _world.GetSingleton<CollisionManager>();
				const LinkingContext& linkingContext = _world.GetSingleton<LinkingContext>();

				// spawn the bullet now
				const EcsHandle ownerHandle = linkingContext.netIDToEcsHandle.at( owner );
				const EcsEntity ownerEntity = _world.GetEntity( ownerHandle );
				const Weapon& ownerWeapon = _world.GetComponent<Weapon>( ownerEntity );
				const Rigidbody& ownerRigidbody = _world.GetComponent<Rigidbody>( ownerEntity );

				// creates the bullet
				if( *ownerWeapon.bulletPrefab != nullptr )
				{
					SceneNode& node = *ownerWeapon.bulletPrefab->Instanciate( scene.GetRootNode() );
					EcsEntity bulletID = _world.GetEntity( node.handle );

					Transform& bulletTransform = _world.GetComponent<Transform>( bulletID );
					bulletTransform.SetPosition( position );

					Rigidbody& bulletRigidbody = _world.GetComponent<Rigidbody>( bulletID );
					bulletRigidbody.onContactStarted.Connect( &CollisionManager::OnBulletContact, &collisionManager );
					bulletRigidbody.SetIgnoreCollisionCheck( ownerRigidbody, true );
					bulletRigidbody.SetVelocity( velocity );
					bulletRigidbody.SetMotionState( _world.GetComponent<MotionState>( bulletID ).motionState );
					bulletRigidbody.SetCollisionShape( _world.GetComponent<SphereShape>( bulletID ).sphereShape );
					bulletRigidbody.SetTransform( bulletTransform.transform );
					physicsWorld.dynamicsWorld->addRigidBody( bulletRigidbody.rigidbody );
				}
			}
		};
	}
}