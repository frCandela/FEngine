#pragma  once

#include "core/ecs/fanEcsComponent.hpp"
#include "network/fanPacket.hpp"

namespace fan
{
    struct Time;
	//========================================================================================================
	// [Server]
	//========================================================================================================
	struct HostConnection: public EcsComponent
	{
		ECS_COMPONENT( HostConnection )
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static FrameIndex CalculatePerfectSpawnTiming( const HostConnection& _connection, const Time& _time );

		enum State
		{
			Disconnected,		// Requires a hello packet from the client to start connection process
			NeedingApprouval,	// Client hello is received, a Login packet must be sent back
			PendingApprouval,	// Login packet was sent, waiting for ack
			Connected			// Login packet was received, client is connected
		};

		IpAddress   mIp;
		Port        mPort;
		std::string mName;
		State       mState;
		double      mLastResponseTime;	// last time the client answered back
		double      mLastPingTime;		// last time the client was sent a ping
		double      mLastDisconnectTime;	// last time the client was sent a disconnect packet
		float       mRtt;
		float       mBandwidth;			// Ko/s send to the client
		float       mPingDelay;			// send a ping to clients every X seconds
		float       mDisconnectDelay;	// send a disconnect packet to clients every X seconds
		float       mTimeoutDelay;		// disconnects clients after X seconds without a response

		// client frame index synchronization
		bool               mSynced;				// true if the client has been synced
		double             mLastSync;			// client frame index value is correct
		std::array<int, 3> mFramesDelta;		// server-client frame index delta in the N previous frames
		int                mTargetBufferSize;	// the size of the input buffer we target for the client specific rtt
		int                mNextDeltaIndex;		// next delta to update in the array

		void Write( EcsWorld& _world, EcsEntity _entity, Packet& _packet );
		void ProcessPacket	( const PacketHello& _packetHello );
		void ProcessPacket	( const PacketPing&  _packetPing, const FrameIndex _frameIndex, const float _logicDelta );
		void OnSyncSuccess	();
		void OnLoginFail	( const PacketTag );
		void OnLoginSuccess	( const PacketTag );
	};
}