#pragma once

#include "network/components/fanClientRPC.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    struct GuiClientRPC
    {
        //====================================================================================================
        //====================================================================================================
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::Network16;
            info.mGroup      = EngineGroups::Network;
            info.onGui       = &GuiClientRPC::OnGui;
            info.mEditorName = "Client RPC";
            return info;
        }

        //========================================================================================================
        //========================================================================================================
        static void OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
        {
            ClientRPC& rpc = static_cast<ClientRPC&>( _component );
            ImGui::Text( "rpc list: " );
            ImGui::Indent();
            for( std::pair<ClientRPC::RpcID, ClientRPC::RpcUnwrapMethod> pair : rpc.mNameToRPCTable )
            {
                ImGui::Text( "%d", pair.first );
            }
            ImGui::Unindent();
        }
    };
}