#pragma once

#include "project_spaceships/game/singletons/fanClientNetworkManager.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    struct GuiClientNetworkManager
    {
        //====================================================================================================
        //====================================================================================================
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mIcon  = ImGui::ClientNet16;
            info.mGroup = EngineGroups::GameNetwork;
            info.onGui  = &GuiClientNetworkManager::OnGui;
            info.mEditorName  = "client network manager";
            return info;
        }

        //========================================================================================================
        //========================================================================================================
        static void OnGui( EcsWorld& /*_world*/, EcsSingleton& _component )
        {
            ClientNetworkManager& netManager = static_cast<ClientNetworkManager&>( _component );
            ImGui::Text( "persistent handle : %d", netManager.mPersistentHandle );
        }
    };
}