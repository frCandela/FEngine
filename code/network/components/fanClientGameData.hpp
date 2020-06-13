#pragma  once

#include <deque>
#include <queue>
#include "ecs/fanEcsComponent.hpp"
#include "network/fanPacket.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// All game info for a remote player
	//==============================================================================================================================================================
	struct ClientGameData : public EcsComponent
	{
		ECS_COMPONENT( ClientGameData )
	public:
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );
		
		//===============================================================================
		// for registering inputs sent to the server		
		//===============================================================================
		struct InputSent
		{
			PacketTag  tag;					// tag of the packet
			FrameIndex mostRecentFrame;		// most recent frame index when the inputs were sent
		};

		EcsHandle							spaceshipHandle;
		std::deque< PacketInput::InputData >previousInputs;
		std::deque< PacketInput::InputData >previousInputsSinceLastGameState;
		std::deque< InputSent>				inputsSent;
		std::queue< PacketPlayerGameState > previousStates;
		bool								frameSynced;
		static const int					maxFrameDeltaBeforeShift = 20; // if the server/client frame delta > this, shift frameIndex. Otherwise use timescale
		bool								spaceshipSynced;
		int									maxInputSent;
		PacketPlayerGameState				lastServerState;

		void Write( EcsWorld& _world, EcsEntity _entity, Packet& _packet );
		void ProcessPacket( const PacketPlayerGameState& _packet );
		
		// callbacks
		void OnInputReceived( PacketTag _tag );
		void OnShiftFrameIndex( const int _framesDelta );
	};
	static constexpr size_t sizeof_clientGameData = sizeof( ClientGameData );
}