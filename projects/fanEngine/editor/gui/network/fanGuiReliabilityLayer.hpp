#pragma once

#include "network/components/fanReliabilityLayer.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiReliabilityLayer
    {
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::ReliabilityLayer16;
            info.mGroup      = EngineGroups::Network;
            info.onGui       = &GuiReliabilityLayer::OnGui;
            info.mEditorName = "reliability layer";
            return info;
        }

        static void OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
        {
            ReliabilityLayer& deliveryNotification = static_cast<ReliabilityLayer&>( _component );
            ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 );
            {
                ImGui::Text( "next packet tag:       %d", deliveryNotification.mNextPacketTag );
                ImGui::Text( "expected packet tag:   %d", deliveryNotification.mExpectedPacketTag );
                ImGui::Text( "num pending ack:       %d", (int)deliveryNotification.mPendingAck.size() );
                ImGui::Text( "num in flight packets: %d", (int)deliveryNotification.mInFlightPackets.size() );
            }
            ImGui::PopItemWidth();
        }
    };
}