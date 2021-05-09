#pragma once

#include "editor/singletons/fanEditorGuiInfo.hpp"
#include "network/components/fanClientGameData.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiClientGameData
    {
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::GameData16;
            info.mGroup      = EngineGroups::Network;
            info.onGui       = &GuiClientGameData::OnGui;
            info.mEditorName = "client game data";
            return info;
        }

        static void OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
        {
            ClientGameData& gameData = static_cast<ClientGameData&>( _component );
            ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 );
            {
                ImGui::Text( "player ID           : %u", gameData.mPlayerId );
                ImGui::DragInt( "max input sent", &gameData.mMaxInputSent, 1.f, 0, 200 );
                ImGui::Text( "size previous states:  %d", (int)gameData.mPreviousLocalStates.size() );
                ImGui::Text( "%s", gameData.mFrameSynced ? "frame synced" : "frame not synced" );
                ImGui::Text( "size pending inputs:  %d", (int)gameData.mPreviousInputs.size() );
                ImGui::Text( "size inputs sent:      %d", (int)gameData.mInputsSent.size() );
            }
            ImGui::PopItemWidth();
        }
    };
}