#pragma once

#include "game/singletons/fanServerNetworkManager.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    struct GuiServerNetworkManager
    {
        //====================================================================================================
        //====================================================================================================
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mIcon  = ImGui::Network16;
            info.mGroup = EngineGroups::GameNetwork;
            info.onGui  = &GuiServerNetworkManager::OnGui;
            info.mEditorName  = "server network manager";
            return info;
        }

        //========================================================================================================
        //========================================================================================================
        static void OnGui( EcsWorld& /*_world*/, EcsSingleton& /*_component*/ )
        {
            //ServerNetworkManager& netManager = static_cast<ServerNetworkManager&>( _component );
            ImGui::Text( "Stop looking at me plz" );
        }
    };
}