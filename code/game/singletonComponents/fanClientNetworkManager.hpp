#pragma once

#include "ecs/fanEcsSingleton.hpp"
#include "network/fanPacket.hpp"

namespace fan
{
	class EcsWorld;
	struct SceneNode;

	//================================================================================================================================
	//================================================================================================================================
	struct ClientNetworkManager : public EcsSingleton
	{
		ECS_SINGLETON( ClientNetworkManager )
	public:
		static void SetInfo( EcsSingletonInfo& _info );
		static void Init( EcsWorld& _world, EcsSingleton& _component );
		static void OnGui( EcsWorld&, EcsSingleton& _component );

		void Start( EcsWorld& _world );
		void Stop( EcsWorld& _world );			

		SceneNode* playerPersistent;
		std::vector< std::pair<NetID, FrameIndex> > shipsToSpawn;

		static void SpawnShips( EcsWorld& _world );

		// callbacks
		void OnSpawnShip( NetID _spaceshipID, FrameIndex _frameIndex );
	};
}
