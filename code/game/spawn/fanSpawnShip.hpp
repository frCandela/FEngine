#pragma once

#include "network/singletons/fanSpawnManager.hpp"
#include "network/components/fanClientGameData.hpp"
#include "network/components/fanHostGameData.hpp"
#include "game/singletons/fanGame.hpp"
#include "game/singletons/fanClientNetworkManager.hpp"

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
					if( game.IsServer() )
					{
						HostGameData& hostData = _world.GetComponent<HostGameData>( _world.GetEntity( playerID ) );
						hostData.spaceshipHandle = Game::SpawnSpaceship( _world, true, false );
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
								gameData.spaceshipHandle = Game::SpawnSpaceship( _world, true, true );
								linkingContext.AddEntity( gameData.spaceshipHandle, spaceshipID );
							}
						}
						else
						{
							// spawn remote player ship
							const EcsHandle handle = Game::SpawnSpaceship( _world, false, false );
							linkingContext.AddEntity( handle, spaceshipID );
						}
					}
				}
			}
		};
	}
}