#pragma  once

#include <queue>
#include "ecs/fanComponent.hpp"
#include "network/fanPacket.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// [Server]
	// Uniquely identify and tag each packet send out
	// Send out an acknowledgment for each validated packet 
	// Process incoming acknowledgments and notify connected modules about which packets were received or dropped
	// Ensure packets are never processed out of order. Old packets arriving after newer packets are dropped.
	//
	// HostID is an unique ID used to identify a remote host and store its state
	// The server has multiple hosts, on for each client
	// The client has a single host, the server. Its id is always zero
	//==============================================================================================================================================================
	struct HostDeliveryNotification: public Component
	{
		DECLARE_COMPONENT( HostDeliveryNotification )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( EcsWorld& _world, Component& _component );
		static void OnGui( EcsWorld& _world, EntityID _entityID, Component& _component );

		//================================================================
		// Packet that was send & waiting for a delivery status (received/dropped)
		//================================================================
		struct InFlightPacket
		{
			PacketTag			tag;
			double				timeDispatch;
			Signal< PacketTag >	onFailure;
			Signal< PacketTag >	onSuccess;
		};

		static const float			 timeoutDuration;		// time after which a packet is considered dropped
		PacketTag					 nextPacketTag;			// the tag of the next packet being send
		PacketTag					 expectedPacketTag;		// the expected tag of the next received packet 
		std::vector<PacketTag>		 pendingAck;			// validated packets waiting for ack dispatch @todo change into a range to gain space
		std::queue< InFlightPacket > inFlightPackets;		// list of packets pending success/drop status

		void		RegisterPacket( Packet& _packet, const HostID  _hostID = 0 );
		bool		ValidatePacket( Packet& _packet, const HostID  _hostID = 0 );
		PacketTag	GetNextPacketTag( const HostID _hostID = 0 ) { return nextPacketTag++; }
		void		ProcessPacket( const PacketAck& _packetAck, const HostID _hostID = 0 );
		void		Write( Packet& _packet, const HostID _hostID = 0 );
	};
	static constexpr size_t sizeof_hostDeliveryNotification = sizeof( HostDeliveryNotification );
}