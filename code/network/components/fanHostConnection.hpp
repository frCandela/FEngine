#pragma  once

#include "ecs/fanEcsComponent.hpp"
#include "network/fanPacket.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// [Server]
	//==============================================================================================================================================================
	struct HostConnection: public EcsComponent
	{
		ECS_COMPONENT( HostConnection )
	public:
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );

		enum State
		{
			Disconnected,		// Requires a hello packet from the client to start connection process
			NeedingApprouval,	// Client hello is received, a Login packet must be sent back
			PendingApprouval,	// Login packet was sent, waiting for ack
			Connected			// Login packet was received, client is connected
		};

		IpAddress	ip;
		Port		port;
		std::string	name;
		State		state;
		double		lastResponseTime;	// last time the client answered back
		double		lastPingTime;		// last time the client was sent a ping
		double		lastDisconnectTime;	// last time the client was sent a disconnect packet
		float		rtt;
		float		bandwidth;			// Ko/s send to the client
		float		pingDelay;			// send a ping to clients every X seconds
		float		disconnectDelay;	// send a disconnect packet to clients every X seconds
		float		timeoutDelay;		// disconnects clients after X seconds without a response

		// client frame index synchronization
		bool				synced;				// true if the client has been synced 
		double				lastSync;			// client frame index value is correct
		std::array<int, 3>  framesDelta;		// server-client frame index delta in the N previous frames
		int					targetBufferSize;	// the size of the input buffer we target for the client specific rtt
		int					nextDeltaIndex;		// next delta to update in the array

		void Write( EcsWorld& _world, Packet& _packet );
		void ProcessPacket	( const PacketHello& _packetHello );
		void ProcessPacket	( const PacketPing&  _packetPing, const FrameIndex _frameIndex, const float _logicDelta );
		void OnSyncSuccess	();
		void OnLoginFail	( const PacketTag );
		void OnLoginSuccess	( const PacketTag );
	};
	static constexpr size_t sizeof_hostConnection = sizeof( HostConnection );
}