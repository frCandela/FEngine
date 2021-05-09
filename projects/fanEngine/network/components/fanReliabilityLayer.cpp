#include "network/components/fanReliabilityLayer.hpp"
#include "network/singletons/fanTime.hpp"

namespace fan
{
    const float ReliabilityLayer::sTimeoutDuration = 2.f;

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ReliabilityLayer::SetInfo( EcsComponentInfo& /*_info*/ )
    {
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ReliabilityLayer::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
    {
        ReliabilityLayer& reliabilityLayer = static_cast<ReliabilityLayer&>( _component );
        reliabilityLayer.mNextPacketTag     = 0;
        reliabilityLayer.mExpectedPacketTag = 0;
    }

    //==================================================================================================================================================================================================
    // Ensure packets are never processed out of order.Old packets arriving after newer packets are dropped.
    // Return true if the packet is valid and ready to be processed, false otherwise
    //==================================================================================================================================================================================================
    bool ReliabilityLayer::ValidatePacket( Packet& _packet )
    {
        if( _packet.mOnlyContainsAck ){ return true; }

        if( _packet.mTag == mExpectedPacketTag )    // packet is perfect \o/
        {
            mExpectedPacketTag++;
            mPendingAck.push_back( _packet.mTag );
            return true;
        }
        else if( _packet.mTag < mExpectedPacketTag ) // silently drop old packet.
        {
            return false;
        }
        else //we missed some packets
        {
            fanAssert( _packet.mTag > mExpectedPacketTag );
            mExpectedPacketTag = _packet.mTag + 1;
            mPendingAck.push_back( _packet.mTag );
            return true;
        }
    }

    //==================================================================================================================================================================================================
    // Registers the packet as an inFlightPacket.
    // allows timeout and delivery notification to be issued
    //==================================================================================================================================================================================================
    void ReliabilityLayer::RegisterPacket( Packet& _packet )
    {
        if( _packet.mOnlyContainsAck )
        {
            mNextPacketTag--;
            return;
        }

        InFlightPacket inFlightPacket;
        inFlightPacket.mTag          = _packet.mTag;
        inFlightPacket.mOnFailure    = _packet.mOnFail;
        inFlightPacket.mOnSuccess    = _packet.mOnSuccess;
        inFlightPacket.mTimeDispatch = Time::ElapsedSinceStartup();
        mInFlightPackets.push( inFlightPacket );
    }

    //==================================================================================================================================================================================================
    // Process incoming acknowledgments and notify modules about which packets were received or dropped
    //==================================================================================================================================================================================================
    void ReliabilityLayer::ProcessPacket( const PacketAck& _packetAck )
    {
        int ackIndex = 0;
        while( ackIndex < (int)_packetAck.mTags.size() && !mInFlightPackets.empty() )
        {
            const PacketTag ackPacketTag = _packetAck.mTags[ackIndex];
            /*const*/ InFlightPacket& inFlightPacket = mInFlightPackets.front();
            if( inFlightPacket.mTag == ackPacketTag ) // packet was received ! \o/
            {
                inFlightPacket.mOnSuccess.Emmit( inFlightPacket.mTag );
                mInFlightPackets.pop();
                ackIndex++;
            }
            else if( inFlightPacket.mTag < ackPacketTag ) // packet was dropped or too old
            {
                inFlightPacket.mOnFailure.Emmit( inFlightPacket.mTag );
                mInFlightPackets.pop();
            }
            else // inFlightPacket was already removed (maybe from timeout)
            {
                fanAssert( inFlightPacket.mTag > ackPacketTag );
                ackIndex++;
            }
        }
    }

    //==================================================================================================================================================================================================
    // Send out an acknowledgment for each validated packet
    //==================================================================================================================================================================================================
    void ReliabilityLayer::Write( Packet& _packet )
    {
        if( !mPendingAck.empty() )
        {
            PacketAck packetAck;
            packetAck.mTags = mPendingAck;
            packetAck.Write( _packet );
            mPendingAck.clear();
        }
    }
}