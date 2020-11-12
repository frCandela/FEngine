#pragma once

#include "network/components/fanReliabilityLayer.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void ReliabilityLayer::SetInfo( EcsComponentInfo& _info )
	{
		_info.mIcon  = ImGui::IconType::ReliabilityLayer16;
		_info.mGroup = EngineGroups::Network;
		_info.onGui  = &ReliabilityLayer::OnGui;
		_info.mName  = "reliability layer";
	}

	//========================================================================================================
	//========================================================================================================
	void ReliabilityLayer::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
	{
		ReliabilityLayer& deliveryNotification = static_cast<ReliabilityLayer&>( _component );
		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 );
		{
			ImGui::Text( "next packet tag:       %d", deliveryNotification.mNextPacketTag );
			ImGui::Text( "expected packet tag:   %d", deliveryNotification.mExpectedPacketTag );
			ImGui::Text( "num pending ack:       %d", deliveryNotification.mPendingAck.size() );
			ImGui::Text( "num in flight packets: %d", deliveryNotification.mInFlightPackets.size() );
		}
		ImGui::PopItemWidth();
	}
}