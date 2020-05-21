#pragma  once

#include <queue>
#include "ecs/fanEcsComponent.hpp"
#include "network/fanPacket.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// [Server] Uniquely identify and tag each packet send out
	// Send out an acknowledgment for each validated packet 
	// Process incoming acknowledgments and notify connected modules about which packets were received or dropped
	// Ensure packets are never processed out of order. Old packets arriving after newer packets are dropped.
	//==============================================================================================================================================================
	struct ReliabilityLayer: public EcsComponent
	{
		ECS_COMPONENT( ReliabilityLayer )
	public:
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );

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

		void		RegisterPacket( Packet& _packet );
		bool		ValidatePacket( Packet& _packet );
		PacketTag	GetNextPacketTag() { return nextPacketTag++; }
		void		ProcessPacket( const PacketAck& _packetAck );
		void		Write( Packet& _packet );
	};
	static constexpr size_t sizeof_reliabilityLayer = sizeof( ReliabilityLayer );
}