#pragma once

#include "network/singletons/fanServerConnection.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiServerConnection
    {
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mIcon       = ImGui::Connection16;
            info.mGroup      = EngineGroups::Network;
            info.onGui       = &GuiServerConnection::OnGui;
            info.mEditorName = "server connection";
            return info;
        }

        static void OnGui( EcsWorld&, EcsSingleton& _component )
        {
            ServerConnection& connection = static_cast<ServerConnection&>( _component );
            ImGui::Text( "Server" );
            ImGui::Spacing();
            ImGui::Text( "port: %u", connection.mServerPort );
        }
    };
}