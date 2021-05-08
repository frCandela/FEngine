#pragma  once

#include <queue>
#include "core/ecs/fanEcsComponent.hpp"
#include "network/fanPacket.hpp"

namespace fan
{
	//========================================================================================================
	// [Server] Uniquely identify and tag each packet send out
	// Send out an acknowledgment for each validated packet 
	// Process incoming acknowledgments and notify modules about which packets were received or dropped
	// Ensure packets are never processed out of order. Old packets arriving after newer packets are dropped.
	//========================================================================================================
	struct ReliabilityLayer: public EcsComponent
	{
		ECS_COMPONENT( ReliabilityLayer )
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );

		//================================================================
		// Packet that was send & waiting for a delivery status (received/dropped)
		//================================================================
		struct InFlightPacket
		{
			PacketTag           mTag;
			double              mTimeDispatch;
			Signal< PacketTag > mOnFailure;
			Signal< PacketTag > mOnSuccess;
		};

		static const float           sTimeoutDuration;		// time after which a packet is considered dropped
		PacketTag                    mNextPacketTag;		// the tag of the next packet being send
		PacketTag                    mExpectedPacketTag;	// the expected tag of the next received packet
		std::vector<PacketTag>       mPendingAck;			// validated packets waiting for ack dispatch @todo change into a range to gain space
		std::queue< InFlightPacket > mInFlightPackets;		// list of packets pending success/drop status

		void		RegisterPacket( Packet& _packet );
		bool		ValidatePacket( Packet& _packet );
		PacketTag	GetNextPacketTag() { return mNextPacketTag++; }
		void		ProcessPacket( const PacketAck& _packetAck );
		void		Write( Packet& _packet );
	};
}