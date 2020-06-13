#pragma once

#include "network/singletons/fanSpawnManager.hpp"
#include "game/singletons/fanGame.hpp"
#include "game/singletons/fanClientNetworkManager.hpp"
#include "network/components/fanClientGameData.hpp"

namespace fan
{
	namespace spawn
	{
		//================================================================================================================================
		// @todo merge SpawnShip & SpawnClientShip into one spawn
		//================================================================================================================================
		struct SpawnShip
		{
			static const SpawnIdType  s_id = 'SHIP';

			//================================================================
			//================================================================
			static SpawnInfo GenerateInfo( const FrameIndexNet _spawnFrameIndex, const NetID _spaceshipID, const  btVector3 _position )
			{
				SpawnInfo info;

				info.spawnFrameIndex = _spawnFrameIndex;

				// Write data to packet
				info.data << s_id;
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
				NetID			spaceshipID;
				btVector3		position( 0.f, 0.f, 0.f );
				_data >> spaceshipID;
				_data >> position[0] >> position[2];

				LinkingContext& linkingContext = _world.GetSingleton<LinkingContext>();
				// do not spawn twice
				if( linkingContext.netIDToEcsHandle.find( spaceshipID ) == linkingContext.netIDToEcsHandle.end() )
				{
					// spawn
					const EcsHandle handle = Game::SpawnSpaceship( _world, false, false );
					linkingContext.AddEntity( handle, spaceshipID );
				}
			}
		};

		//================================================================================================================================
		//================================================================================================================================
		struct SpawnClientShip
		{
			static const SpawnIdType  s_id = 'CSHP';

			//================================================================
			//================================================================
			static SpawnInfo GenerateInfo( const FrameIndexNet _spawnFrameIndex, const NetID _spaceshipID, const  btVector3 _position )
			{
				SpawnInfo info;
				info.spawnFrameIndex = _spawnFrameIndex;

				// Write data to packet
				info.data << s_id;
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
				NetID			spaceshipID;
				btVector3		position( 0.f, 0.f, 0.f );
				_data >> spaceshipID;
				_data >> position[0] >> position[2];

				const ClientNetworkManager& netManager = _world.GetSingleton<ClientNetworkManager>();
				const EcsEntity persistentID = _world.GetEntity( netManager.persistentHandle );
				LinkingContext& linkingContext = _world.GetSingleton<LinkingContext>();
				ClientGameData& gameData = _world.GetComponent<ClientGameData>( persistentID );

				// spawns spaceship
				if( gameData.spaceshipHandle == 0 )
				{
					gameData.spaceshipHandle = Game::SpawnSpaceship( _world, true, true );
					linkingContext.AddEntity( gameData.spaceshipHandle, spaceshipID );
				}
			}
		};
	}
}