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

		PlayerID							playerID;			 // unique id of the player, corresponds to the server side player persistent handle
		EcsHandle							spaceshipHandle;	 // handle of the player spaceship
		std::deque< PacketInput::InputData >previousInputs;		 // inputs that need to be sent/acknowledged by the server
		std::deque< InputSent>				inputsSent;			 // inputs sent to server waiting ack
		int									maxInputSent;		 // the maximum number of inputs to send in one packet
		std::queue< PacketPlayerGameState > previousLocalStates; // local state of the client to compare with server state
		PacketPlayerGameState				lastServerState;	 // the last know game state received from server ( useful for rollback )
		bool								frameSynced;		 // false if frame index is not synchronized with the server
		bool								spaceshipSynced;	 // if false, causes a rollback/resimulate to restore synchronization		

		void Write( EcsWorld& _world, EcsEntity _entity, Packet& _packet );
		void ProcessPacket( const PacketPlayerGameState& _packet );
		
		// callbacks
		void OnInputReceived( PacketTag _tag );
		void OnShiftFrameIndex( const int _framesDelta );
		void OnLoginSuccess( PlayerID _playerID ) { playerID = _playerID; }
	};
	static constexpr size_t sizeof_clientGameData = sizeof( ClientGameData );
}