#pragma once

#include "network/components/fanHostReplication.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    struct GuiHostReplication
    {
        //====================================================================================================
        //====================================================================================================
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::Network16;
            info.mGroup      = EngineGroups::Network;
            info.onGui       = &GuiHostReplication::OnGui;
            info.mEditorName = "host replication";
            return info;
        }

        //========================================================================================================
        //========================================================================================================
        static void OnGui( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
        {
            HostReplication& hostReplication = static_cast<HostReplication&>( _component );
            ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 );
            {
                ImGui::Text( "next replication:    %d", hostReplication.mNextReplication.size() );
                ImGui::Text( "pending replication: %d", hostReplication.mPendingReplication.size() );
            }
            ImGui::PopItemWidth();
        }
    };
}