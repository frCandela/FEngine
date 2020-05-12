#pragma once

#include "ecs/fanSingletonComponent.hpp"
#include "network/fanPacket.hpp"

namespace fan
{
	class EcsWorld;
	struct SceneNode;

	//================================================================================================================================
	//================================================================================================================================
	struct ClientNetworkManager : public SingletonComponent
	{
		DECLARE_SINGLETON_COMPONENT()
	public:
		static void SetInfo( SingletonComponentInfo& _info );
		static void Init( EcsWorld& _world, SingletonComponent& _component );
		static void OnGui( EcsWorld&, SingletonComponent& _component );

		void Start( EcsWorld& _world );
		void Stop( EcsWorld& _world );			

		SceneNode* playerPersistent;
		std::vector< std::pair<NetID, FrameIndex> > shipsToSpawn;

		static void SpawnShips( EcsWorld& _world );

		// callbacks
		void OnSpawnShip( NetID _spaceshipID, FrameIndex _frameIndex );
	};
}
