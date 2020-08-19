#pragma once

#include "network/singletons/fanSpawnManager.hpp"
#include "network/components/fanClientGameData.hpp"
#include "network/components/fanHostGameData.hpp"
#include "network/components/fanClientRollback.hpp"
#include "network/components/fanHostPersistentHandle.hpp"
#include "network/components/fanEntityReplication.hpp"
#include "network/components/fanLinkingContextUnregisterer.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanRigidbody.hpp"
#include "scene/components/fanMotionState.hpp"
#include "scene/components/fanBoxShape.hpp"
#include "game/singletons/fanGame.hpp"
#include "game/singletons/fanClientNetworkManager.hpp"
#include "game/components/fanPlayerInput.hpp"
#include "game/components/fanPlayerController.hpp"
#include "game/singletons/fanCollisionManager.hpp"

namespace fan
{
	namespace spawn
	{
		//================================================================================================================================
		//================================================================================================================================
		struct SpawnShip
		{
			static const SpawnID  s_id = SSID( "SpawnShip" );

			//================================================================
			//================================================================
			static SpawnInfo GenerateInfo( const PlayerID _owner, const FrameIndex _spawnFrameIndex, const NetID _spaceshipID, const  btVector3 _position )
			{
				assert( _spaceshipID != 0 );

				SpawnInfo info;
				info.spawnFrameIndex = _spawnFrameIndex;
				info.spawnID = s_id;

				// Write data to packet
				info.data << _owner;
				info.data << _spaceshipID;
				info.data << _position[0] << _position[2];

				return info;
			}

			//================================================================
			// @todo use position
			//================================================================
			static void Spawn( EcsWorld& _world, sf::Packet _data )
			{
				// Get data from packet
				PlayerID	playerID;
				NetID		spaceshipID;
				btVector3	position( 0.f, 0.f, 0.f );
				_data >> playerID;
				_data >> spaceshipID;
				_data >> position[0] >> position[2];

				LinkingContext& linkingContext			= _world.GetSingleton<LinkingContext>();
				const Game& game						= _world.GetSingleton<Game>();

				// do not spawn twice
				if( linkingContext.netIDToEcsHandle.find( spaceshipID ) == linkingContext.netIDToEcsHandle.end() )
				{
					if( game.mIsServer )
					{
						HostGameData& hostData = _world.GetComponent<HostGameData>( _world.GetEntity( playerID ) );
						hostData.spaceshipHandle = SpawnShip::SpawnSpaceship( _world, true, false, playerID );
						linkingContext.AddEntity( hostData.spaceshipHandle, spaceshipID );

					}
					else
					{
						const ClientNetworkManager& netManager = _world.GetSingleton<ClientNetworkManager>();
						const EcsEntity				persistentEntity = _world.GetEntity( netManager.persistentHandle );
						ClientGameData& gameData = _world.GetComponent<ClientGameData>( persistentEntity );

						if( gameData.playerID == playerID )
						{
							// spawn this client ship
							if( gameData.spaceshipHandle == 0 )
							{
								gameData.spaceshipHandle = SpawnShip::SpawnSpaceship( _world, true, true );
								if( gameData.spaceshipHandle != 0 )
                                {
                                    linkingContext.AddEntity( gameData.spaceshipHandle, spaceshipID );
                                }
							}
						}
						else
						{
							// spawn remote player ship
							const EcsHandle handle = SpawnShip::SpawnSpaceship( _world, false, false );
							linkingContext.AddEntity( handle, spaceshipID );
						}
					}
				}
			}

			//================================================================================================================================
			// Generates the spaceship entity from the game prefab
			// PlayerInput component causes the ship to be driven by inputs ( forward, left, right, boost etc. )
			// PlayerController automatically updates the PlayerInput with local inputs from mouse & keyboard
			//================================================================================================================================
			static EcsHandle SpawnSpaceship( EcsWorld& _world, const bool _hasPlayerInput, const bool _hasPlayerController, const EcsHandle _persistentHandle = 0 )
			{
				// spawn the spaceship	
				Game& game = _world.GetSingleton< Game >();
				if( game.mSpaceshipPrefab != nullptr )
				{
					Scene& scene = _world.GetSingleton<Scene>();
					SceneNode& spaceshipNode = *game.mSpaceshipPrefab->Instantiate( scene.GetRootNode() );
					EcsEntity spaceshipID = _world.GetEntity( spaceshipNode.handle );

					_world.AddComponent<LinkingContextUnregisterer>( spaceshipID );

					// set name
					if( _hasPlayerController )
					{
						spaceshipNode.name = "spaceship_player";
					}
					else if( _persistentHandle )
					{
						spaceshipNode.name = "spaceship" + std::to_string( _persistentHandle );
					}

					if( _persistentHandle != 0 ) // server only
					{						
						_world.AddComponent<HostPersistentHandle>( spaceshipID ).handle = _persistentHandle;
						EntityReplication& entityReplication = _world.AddComponent<EntityReplication>( spaceshipID );
						entityReplication.componentTypes = { Transform::Info::s_type, Rigidbody::Info::s_type };
						entityReplication.exclude = _persistentHandle;
					}

					if( _hasPlayerInput )
					{
						_world.AddComponent<PlayerInput>( spaceshipID );
					}

					if( _hasPlayerController )
					{
						_world.AddComponent<PlayerController>( spaceshipID );
						_world.AddComponent<ClientRollback>( spaceshipID );
					}


                    fanAssert( _world.HasComponent<Transform>( spaceshipID ));
                    fanAssert( _world.HasComponent<Rigidbody>( spaceshipID ));
                    fanAssert( _world.HasComponent<MotionState>( spaceshipID ));
					fanAssert( _world.HasComponent<BoxShape>( spaceshipID ));

					if( _world.HasComponent<Transform>( spaceshipID )
						&& _world.HasComponent<Rigidbody>( spaceshipID )
						&& _world.HasComponent<MotionState>( spaceshipID )
						&& _world.HasComponent<BoxShape>( spaceshipID ) )
					{
						// set initial position
						Transform& transform = _world.GetComponent<Transform>( spaceshipID );
						transform.SetPosition( btVector3( 0, 0, 4.f ) );

						// add rigidbody to the physics world
						PhysicsWorld& physicsWorld = _world.GetSingleton<PhysicsWorld>();
						Rigidbody& rigidbody = _world.GetComponent<Rigidbody>( spaceshipID );
						MotionState& motionState = _world.GetComponent<MotionState>( spaceshipID );
						BoxShape& boxShape = _world.GetComponent<BoxShape>( spaceshipID );
						rigidbody.SetCollisionShape( boxShape.boxShape );
						rigidbody.SetMotionState( motionState.motionState );
						rigidbody.rigidbody->setWorldTransform( transform.transform );
						physicsWorld.dynamicsWorld->addRigidBody( rigidbody.rigidbody );

						// registers physics callbacks
						CollisionManager& collisionManager = _world.GetSingleton<CollisionManager>();
						rigidbody.onContactStarted.Connect( &CollisionManager::OnSpaceShipContact, &collisionManager );

						return spaceshipNode.handle;
					}
					else
					{
						Debug::Error()
							<< "Game: spaceship prefab is missing a component" << "\n"
							<< "component needed: Transform, Rigidbody, MotionState, BoxShape" << Debug::Endl();
						return 0;
					}
				}
				else
				{
					Debug::Error() << "spaceship prefab is null" << Debug::Endl();
					return 0;
				}
			}
		};
	}
}