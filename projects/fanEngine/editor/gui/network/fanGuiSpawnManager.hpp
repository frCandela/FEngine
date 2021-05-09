#pragma once

#include "network/singletons/fanSpawnManager.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiSpawnManager
    {
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mIcon       = ImGui::Spawn16;
            info.mGroup      = EngineGroups::Network;
            info.mEditorName = "spawn manager";
            info.onGui       = &GuiSpawnManager::OnGui;
            return info;
        }

        static void OnGui( EcsWorld&, EcsSingleton& _component )
        {
            const SpawnManager& spawnManager = static_cast<const SpawnManager&>( _component );
            ImGui::Text( "pending spawns: %d", (int)spawnManager.spawns.size() );
        }
    };
}