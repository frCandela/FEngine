#pragma once

#include "network/components/fanClientReplication.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    struct GuiClientReplication
    {
        //====================================================================================================
        //====================================================================================================
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::Network16;
            info.mGroup      = EngineGroups::Network;
            info.onGui       = &GuiClientReplication::OnGui;
            info.mEditorName = "client replication";
            return info;
        }

        //====================================================================================================
        //====================================================================================================
        static void OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
        {
            ClientReplication& replicationManager = static_cast<ClientReplication&>( _component );
            ImGui::Text( "singletons: %d", replicationManager.mReplicationListSingletons.size() );
            ImGui::Text( "rpc:        %d", replicationManager.mReplicationListRPC.size() );
        }
    };
}