#pragma once

#include "network/singletons/fanSpawnManager.hpp"
#include "project_spaceships/singletons/fanSolarEruption.hpp"
#include "project_spaceships/singletons/fanGame.hpp"

namespace fan
{
	namespace spawn
	{
		//====================================================================================================
		//====================================================================================================
		struct SpawnSolarEruption
		{
			static const SpawnID sID = SSID( "SpawnEruption" );

			//================================================================
			//================================================================
			static SpawnInfo GenerateInfo( const FrameIndex _spawnFrameIndex )
			{
				SpawnInfo info;
				info.spawnFrameIndex = _spawnFrameIndex;
				info.spawnID = sID;

				// Write data to packet
				//info.data << s_id;

				return info;
			}

			//================================================================
			//================================================================
			static void Spawn( EcsWorld& _world, sf::Packet /*_data*/ )
			{
				SolarEruption& eruption = _world.GetSingleton<SolarEruption>();
				eruption.SpawnEruptionNow();

				Game& game = _world.GetSingleton<Game>();
				if( game.mIsServer )
				{
					eruption.ScheduleNextEruption( _world );
				}
			}
		};
	}
}