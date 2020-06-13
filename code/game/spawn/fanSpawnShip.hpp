#pragma once

#include "network/singletons/fanSpawnManager.hpp"

// #include "game/components/fanPlayerController.hpp"
// #include "game/components/fanPlayerInput.hpp"
#include "game/singletons/fanGame.hpp"

namespace fan
{
	namespace spawn
	{
		//================================================================================================================================
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
	}
}