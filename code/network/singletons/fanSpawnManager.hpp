#pragma once

#include <unordered_map>
#include "ecs/fanEcsSingleton.hpp"
#include "network/fanPacket.hpp"
#include "bullet/LinearMath/btVector3.h"
#include "network/components/fanClientRPC.hpp"
#include "network/fanPacket.hpp"

namespace fan
{
	struct Rigidbody;
	class EcsWorld;

	//================================================================
	// Append this to SpawnManager::spawn to trigger a spawn at a specific frame
	// For creating custom spawning method, look at game/spawn
	//================================================================
	struct SpawnInfo
	{
		FrameIndexNet spawnFrameIndex = 0;
		sf::Packet    data;
	};

	using SpawnIdType = sf::Uint32;

	//================================================================================================================================
	//================================================================================================================================	
	struct SpawnManager : public EcsSingleton
	{
		ECS_SINGLETON( SpawnManager )
	public:
		static void SetInfo( EcsSingletonInfo& _info );
		static void Init( EcsWorld& _world, EcsSingleton& _component );
		static void OnGui( EcsWorld&, EcsSingleton& _component );
		static void RegisterSpawnMethods( SpawnManager& _spawnManager );

		static void Spawn( EcsWorld& _world );			
		void OnSpawn( const FrameIndexNet _frameIndex, sf::Packet _data );

		std::vector< SpawnInfo > spawns;
		std::unordered_map< SpawnIdType, void ( * )( EcsWorld & _world, sf::Packet _data ) > spawnMethods;
	};
}