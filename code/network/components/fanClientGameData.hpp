#pragma  once

#include <queue>
#include "ecs/fanComponent.hpp"
#include "network/fanPacket.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// All game info for a remote player
	//==============================================================================================================================================================
	struct ClientGameData : public Component
	{
		DECLARE_COMPONENT( ClientGameData )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( EcsWorld& _world, Component& _component );
		static void OnGui( EcsWorld& _world, EntityID _entityID, Component& _component );
		
		FrameIndex							spaceshipSpawnFrameIndex;	// the frame index on which the spaceship is spawned
		NetID								spaceshipNetID;
		EntityHandle						spaceshipHandle;
		std::queue< PacketInput >			inputs;
		std::queue< PacketPlayerGameState > previousStates;
		bool								synced;

		void ProcessPacket( const PacketPlayerGameState& _packet );
		// RPC callbacks
		void OnShiftFrameIndex( const int _framesDelta );
		void OnSpawnShip( NetID _spaceshipID, FrameIndex _frameIndex );
	};
	static constexpr size_t sizeof_clientGameData = sizeof( ClientGameData );
}