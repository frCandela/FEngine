#include "network/singletons/fanSpawnManager.hpp"

#include "core/fanDebug.hpp"
#include "game/singletons/fanGame.hpp"
#include "game/spawn/fanSpawnBullet.hpp"
#include "game/spawn/fanSpawnShip.hpp"

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
		spawnManager.spawns.clear();
		RegisterSpawnMethods( spawnManager );
	}

	//================================================================================================================================
	// @todo put this in game lib to prevent linking issues
	//================================================================================================================================
	void SpawnManager::RegisterSpawnMethods( SpawnManager& _spawnManager )
	{
		_spawnManager.spawnMethods.clear();
		_spawnManager.spawnMethods[ spawn::SpawnBullet::s_id ] = &spawn::SpawnBullet::Spawn;
		_spawnManager.spawnMethods[ spawn::SpawnShip::s_id   ] = &spawn::SpawnShip::Spawn;
	}

	//================================================================================================================================
	// Iterates over the spawns list to find trigger spawns with the correct timing
	//================================================================================================================================
	void SpawnManager::Spawn( EcsWorld& _world )
	{
		const Game& game = _world.GetSingleton<Game>();
		SpawnManager& spawnManager = _world.GetSingleton<SpawnManager>();

		// spawn
		for (int spawnIndex = int(spawnManager.spawns.size()) - 1; spawnIndex >= 0 ; spawnIndex--)
		{
			SpawnInfo& spawnInfo = spawnManager.spawns[spawnIndex];
			if( game.frameIndex < spawnInfo.spawnFrameIndex )
			{
				continue; // spawn will happen later
			}
			else if( game.frameIndex == spawnInfo.spawnFrameIndex )
			{
				SpawnIdType spawnId;
				spawnInfo.data >> spawnId;
				spawnManager.spawnMethods.at(spawnId)( _world, spawnInfo.data );
				spawnManager.spawns.erase( spawnManager.spawns.begin() + spawnIndex );
			}
			else // game.frameIndex > spawnInfo.spawnFrameIndex
			{
				// we missed the spawn :'(
				// @todo rollback client to spawn the bullet correctly 
				Debug::Warning() << "missed bullet spawning for frame" << spawnInfo.spawnFrameIndex << Debug::Endl();
 				spawnManager.spawns.erase( spawnManager.spawns.begin() + spawnIndex );
			}
 		}
	}

	//================================================================================================================================
	// Called by the rpc manager when a spawn is received
	//================================================================================================================================
	void SpawnManager::OnSpawn( const FrameIndexNet _frameIndex, sf::Packet _data )
	{
		spawns.push_back( { _frameIndex , _data } );
	}

	//================================================================================================================================
	//================================================================================================================================
	void SpawnManager::OnGui( EcsWorld&, EcsSingleton& _component )
	{
		const SpawnManager& spawnManager = static_cast<const SpawnManager&>( _component );
		ImGui::Text( "pending bullets: %d", spawnManager.spawns.size() );
	}
}