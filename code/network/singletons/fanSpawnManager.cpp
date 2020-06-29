#include "network/singletons/fanSpawnManager.hpp"

#include "core/fanDebug.hpp"
#include "network/singletons/fanTime.hpp"
#include "game/spawn/fanSpawnBullet.hpp"
#include "game/spawn/fanSpawnShip.hpp"
#include "game/spawn/fanSpawnSolarEruption.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void SpawnManager::SetInfo( EcsSingletonInfo& _info )
	{
		_info.icon = ImGui::SPAWN16;
		_info.name = "spawn manager";
		_info.onGui = &SpawnManager::OnGui;
	}

	//================================================================================================================================
	//================================================================================================================================
	void SpawnManager::Init( EcsWorld& /*_world*/, EcsSingleton& _component )
	{
		SpawnManager& spawnManager = static_cast<SpawnManager&>( _component );
		spawnManager.RegisterSpawnMethods();
	}

	//================================================================================================================================
	// register one spawn method, checks that there is no spawnID collision
	//================================================================================================================================
	void SpawnManager::RegisterSpawnMethod( const SpawnID _spawnID, const SpawnMethod _spawnMethod )
	{
		assert( spawnMethods.find( _spawnID ) == spawnMethods.end() );
		spawnMethods[_spawnID] = _spawnMethod;
	}

	//================================================================================================================================
	// @todo put this in game lib to prevent linking issues
	//================================================================================================================================
	void SpawnManager::RegisterSpawnMethods()
	{
		spawnMethods.clear();		
		RegisterSpawnMethod( spawn::SpawnBullet::s_id, &spawn::SpawnBullet::Spawn );
		RegisterSpawnMethod( spawn::SpawnShip::s_id, &spawn::SpawnShip::Spawn );
		RegisterSpawnMethod( spawn::SpawnSolarEruption::s_id, &spawn::SpawnSolarEruption::Spawn );
	}

	//================================================================================================================================
	// Iterates over the spawns list to find trigger spawns with the correct timing
	//================================================================================================================================
	void SpawnManager::Spawn( EcsWorld& _world )
	{
		const Time& time = _world.GetSingleton<Time>();
		SpawnManager& spawnManager = _world.GetSingleton<SpawnManager>();

		// spawn
		for (int spawnIndex = int(spawnManager.spawns.size()) - 1; spawnIndex >= 0 ; spawnIndex--)
		{
			SpawnInfo& spawnInfo = spawnManager.spawns[spawnIndex];
			if( time.frameIndex >= spawnInfo.spawnFrameIndex )
			{
				if( time.frameIndex > spawnInfo.spawnFrameIndex )
				{

					Debug::Warning() << "missed spawning for frame" << spawnInfo.spawnFrameIndex;
					Debug::Get() << " ,delta is " << time.frameIndex - spawnInfo.spawnFrameIndex << Debug::Endl();
				}

				spawnManager.spawnMethods.at( spawnInfo.spawnID )( _world, spawnInfo.data );
				spawnManager.spawns.erase( spawnManager.spawns.begin() + spawnIndex );
			}
 		}
	}

	//================================================================================================================================
	// Called by the rpc manager when a spawn is received
	//================================================================================================================================
	void SpawnManager::OnSpawn( const sf::Uint32 _spawnID, const FrameIndex _frameIndex, sf::Packet _data )
	{
		spawns.push_back( { _spawnID, _frameIndex , _data } );
	}

	//================================================================================================================================
	//================================================================================================================================
	void SpawnManager::OnGui( EcsWorld&, EcsSingleton& _component )
	{
		const SpawnManager& spawnManager = static_cast<const SpawnManager&>( _component );
		ImGui::Indent(); ImGui::Indent();
		{
			ImGui::Text( "pending spawns: %d", spawnManager.spawns.size() );
		}
		ImGui::Unindent(); ImGui::Unindent();		
	}
}