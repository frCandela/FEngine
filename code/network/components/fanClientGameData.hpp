#pragma  once

#include <deque>
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
		
		//===============================================================================
		// for registering inputs sent to the server		
		//===============================================================================
		struct InputSent
		{
			PacketTag  tag;					// tag of the packet
			FrameIndex mostRecentFrame;		// most recent frame index when the inputs were sent
		};

		FrameIndex							spaceshipSpawnFrameIndex;	// the frame index on which the spaceship is spawned
		NetID								spaceshipNetID;
		EntityHandle						spaceshipHandle;
		std::deque< PacketInput::InputData >previousInputs;
		std::deque< PacketInput::InputData >previousInputsSinceLastGameState;
		std::deque< InputSent>				inputsSent;
		std::queue< PacketPlayerGameState > previousStates;
		bool								frameSynced;
		bool								spaceshipSynced;
		int									maxInputSent;
		PacketPlayerGameState				lastServerState;

		void Write( Packet& _packet );
		void ProcessPacket( const PacketPlayerGameState& _packet );
		
		// callbacks
		void OnInputReceived( PacketTag _tag );
		void OnShiftFrameIndex( const int _framesDelta );
		void OnSpawnShip( NetID _spaceshipID, FrameIndex _frameIndex );
	};
	static constexpr size_t sizeof_clientGameData = sizeof( ClientGameData );
}