#pragma  once

#include <deque>
#include <queue>
#include "ecs/fanEcsComponent.hpp"
#include "network/fanPacket.hpp"

namespace fan
{
	//========================================================================================================
	// All game info for a remote player
	//========================================================================================================
	struct ClientGameData : public EcsComponent
	{
		ECS_COMPONENT( ClientGameData )
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );
		
		//===============================================================================
		// for registering inputs sent to the server		
		//===============================================================================
		struct InputSent
		{
			PacketTag  mTag;					// tag of the packet
			FrameIndex mMostRecentFrame;		// most recent frame index when the inputs were sent
		};

		PlayerID                             mPlayerId;			// unique id of the player, corresponds to the server side player persistent handle
		EcsHandle                            sSpaceshipHandle;	// handle of the player spaceship
		std::deque< PacketInput::InputData > mPreviousInputs;   // inputs that need to be sent/acknowledged by the server
		std::deque< InputSent>               mInputsSent;		// inputs sent to server waiting ack
		int                                  mMaxInputSent;		// the maximum number of inputs to send in one packet
		std::queue< PacketPlayerGameState >  mPreviousLocalStates; // local state of the client to compare with server state
		PacketPlayerGameState                mLastServerState;	    // the last know game state received from server ( useful for rollback )
		bool                                 mFrameSynced;		    // false if frame index is not synchronized with the server
		bool                                 mSpaceshipSynced;	    // if false, causes a rollback/resimulate to restore synchronization

		void Write( EcsWorld& _world, EcsEntity _entity, Packet& _packet );
		void ProcessPacket( const PacketPlayerGameState& _packet );
		
		// callbacks
		void OnInputReceived( PacketTag _tag );
		void OnShiftFrameIndex( const int _framesDelta );
		void OnLoginSuccess( PlayerID _playerID ) { mPlayerId = _playerID; }
	};
}