#pragma once

#include "network/singletons/fanSpawnManager.hpp"

#include "project_spaceships/singletons/fanCollisionManager.hpp"
#include "network/singletons/fanLinkingContext.hpp"
#include "engine/singletons/fanPhysicsWorld.hpp"
#include "engine/singletons/fanScene.hpp"
#include "engine/components/fanTransform.hpp"
#include "engine/components/fanRigidbody.hpp"
#include "engine/components/fanMotionState.hpp"
#include "engine/components/fanSphereShape.hpp"
#include "project_spaceships/components/fanWeapon.hpp"
#include "engine/components/fanSceneNode.hpp"

namespace fan
{
	namespace spawn
	{
		//====================================================================================================
		//====================================================================================================
		struct SpawnBullet
		{
			static const SpawnID sID = SSID( "SpawnBullet" );

			//================================================================
			//================================================================
            static SpawnInfo GenerateInfo( const FrameIndex _spawnFrameIndex,
                                           const NetID _owner,
                                           const btVector3 _position,
                                           const btVector3 _velocity )
            {
				SpawnInfo info;
				info.spawnFrameIndex = _spawnFrameIndex;
				info.spawnID = sID;

				// Write data to packet
				info.data << _position[0] << _position[2];
				info.data << _velocity[0] << _velocity[2];
				info.data << _owner;

				return info;
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



				Instanciate( _world, owner, position, velocity );
			}

			//================================================================
			//================================================================
            static void Instanciate( EcsWorld& _world,
                                     const NetID _owner,
                                     const btVector3& _position,
                                     const btVector3& _velocity )
            {
				Scene& scene = _world.GetSingleton<Scene>();
				PhysicsWorld& physicsWorld = _world.GetSingleton<PhysicsWorld>();
				CollisionManager& collisionManager = _world.GetSingleton<CollisionManager>();
				const LinkingContext& linkingContext = _world.GetSingleton<LinkingContext>();

				// spawn the bullet now
				const EcsHandle ownerHandle = linkingContext.mNetIDToEcsHandle.at( _owner );
				const EcsEntity ownerEntity = _world.GetEntity( ownerHandle );
				const Weapon& ownerWeapon = _world.GetComponent<Weapon>( ownerEntity );
				const Rigidbody& ownerRigidbody = _world.GetComponent<Rigidbody>( ownerEntity );

				// creates the bullet
				if( *ownerWeapon.mBulletPrefab != nullptr )
				{
					SceneNode& node = *ownerWeapon.mBulletPrefab->Instantiate( scene.GetRootNode() );
					EcsEntity bulletID = _world.GetEntity( node.mHandle );

					Transform& bulletTransform = _world.GetComponent<Transform>( bulletID );
					bulletTransform.SetPosition( _position );

					Rigidbody& bulletRigidbody = _world.GetComponent<Rigidbody>( bulletID );
                    bulletRigidbody.mOnContactStarted.Connect( &CollisionManager::OnBulletContact,
                                                               &collisionManager );
					bulletRigidbody.SetIgnoreCollisionCheck( ownerRigidbody, true );
					bulletRigidbody.SetVelocity( _velocity );
					bulletRigidbody.SetMotionState( _world.GetComponent<MotionState>( bulletID ).mMotionState );
					bulletRigidbody.SetCollisionShape( _world.GetComponent<SphereShape>( bulletID ).mSphereShape );
					bulletRigidbody.SetTransform( bulletTransform.mTransform );
					physicsWorld.mDynamicsWorld->addRigidBody( bulletRigidbody.mRigidbody );
				}
			}
		};
	}
}