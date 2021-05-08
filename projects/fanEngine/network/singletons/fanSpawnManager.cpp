#include "network/singletons/fanSpawnManager.hpp"

#include "network/singletons/fanTime.hpp"
#include "network/singletons/fanLinkingContext.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void SpawnManager::SetInfo( EcsSingletonInfo& /*_info*/ )
	{
	}

	//========================================================================================================
	//========================================================================================================
	void SpawnManager::Init( EcsWorld& /*_world*/, EcsSingleton& _component )
	{
		SpawnManager& spawnManager = static_cast<SpawnManager&>( _component );
        (void)spawnManager;
    }

	//========================================================================================================
	// register one spawn method, checks that there is no spawnID collision
	//========================================================================================================
	void SpawnManager::RegisterSpawnMethod( const SpawnID _spawnID, const SpawnMethod _spawnMethod )
	{
        fanAssert( spawnMethods.find( _spawnID ) == spawnMethods.end() );
		spawnMethods[_spawnID] = _spawnMethod;
	}

	//========================================================================================================
	// Iterates over the spawns list to find trigger spawns with the correct timing
	//========================================================================================================
	void SpawnManager::Update( EcsWorld& _world )
	{
		const Time& time = _world.GetSingleton<Time>();
		SpawnManager& spawnManager = _world.GetSingleton<SpawnManager>();

		// spawn
		for (int spawnIndex = int(spawnManager.spawns.size()) - 1; spawnIndex >= 0 ; spawnIndex--)
		{
			SpawnInfo& spawnInfo = spawnManager.spawns[spawnIndex];
			if( time.mFrameIndex >= spawnInfo.spawnFrameIndex )
			{
				if( time.mFrameIndex > spawnInfo.spawnFrameIndex )
				{
					Debug::Warning() << "missed spawning for frame" << spawnInfo.spawnFrameIndex
					                 << " ,delta is " << time.mFrameIndex - spawnInfo.spawnFrameIndex
					                 << Debug::Endl();
				}

				spawnManager.spawnMethods.at( spawnInfo.spawnID )( _world, spawnInfo.data );
				spawnManager.spawns.erase( spawnManager.spawns.begin() + spawnIndex );
			}
 		}

		// despawn
		LinkingContext& linkingContext = _world.GetSingleton<LinkingContext>();
		for ( const NetID netID : spawnManager.despawns )
		{
			auto it = linkingContext.mNetIDToEcsHandle.find( netID );
			if( it != linkingContext.mNetIDToEcsHandle.end() )
			{
				const EcsHandle handle = it->second;
				linkingContext.RemoveEntity( handle );
				_world.Kill( _world.GetEntity( handle ) );
			}
			else
			{
				Debug::Warning() << "spawn manager despawn failed for net ID " << netID << Debug::Endl();
			}			
		}
		spawnManager.despawns.clear();
	}

	//========================================================================================================
	// Called by the rpc manager when a spawn is received
	//========================================================================================================
	void SpawnManager::OnSpawn( const sf::Uint32 _spawnID, const FrameIndex _frameIndex, sf::Packet _data )
	{
		spawns.push_back( { _spawnID, _frameIndex , _data } );
	}

	//========================================================================================================
	//========================================================================================================
	void SpawnManager::OnDespawn( const NetID _netID )
	{
		despawns.push_back( _netID );
	}
}