#pragma  once

#include "ecs/fanComponent.hpp"
#include "network/fanPacket.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// [Server]
	//==============================================================================================================================================================
	struct HostConnection: public Component
	{
		DECLARE_COMPONENT( HostConnection )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( EcsWorld& _world, Component& _component );
		static void OnGui( EcsWorld& _world, EntityID _entityID, Component& _component );

		enum State
		{
			Null,				// empty client slot
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
		float		rtt;
		float		bandwidth;			// Ko/s send to the client
		float		pingDelay;			// send a ping to clients every X seconds
		float		timeoutTime;		// disconnects clients after X seconds without a response

		// client frame index synchronization
		bool				synced;			// true if the client has been synced 
		double				lastSync;		// client frame index value is correct
		std::array<int, 5>  framesDelta;	// server-client frame index delta in the N previous frames
		int					nextDeltaIndex;	// next delta to update in the array

		void Write( EcsWorld& _world, Packet& _packet );
		void ProcessPacket	( const PacketHello& _packetHello );
		void ProcessPacket	( const PacketPing&  _packetPing, const FrameIndex _frameIndex, const float _logicDelta );
		void OnSyncSuccess	();
		void OnLoginFail	( const PacketTag );
		void OnLoginSuccess	( const PacketTag );
	};
	static constexpr size_t sizeof_hostConnection = sizeof( HostConnection );
}