#pragma once

#include "network/components/fanHostGameData.hpp"
#include "editor/fanGuiInfos.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiHostGameData
    {
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::GameData16;
            info.mGroup      = EngineGroups::Network;
            info.onGui       = &GuiHostGameData::OnGui;
            info.mEditorName = "host game data";
            return info;
        }

        static void OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
        {
            HostGameData& hostGameData = static_cast<HostGameData&>( _component );
            ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 );
            {
                ImGui::Text( "spaceshipID :      %u", hostGameData.mSpaceshipID );
                ImGui::Text( "spaceship handle : %u", hostGameData.mSpaceshipHandle );
                ImGui::Text( "inputs size :      %u", (int)hostGameData.mInputs.size() );

                if( ImGui::CollapsingHeader( "inputs" ) )
                {
                    std::queue<PacketInput::InputData> inputsCpy = hostGameData.mInputs;
                    while( !inputsCpy.empty() )
                    {
                        ImGui::Text( "%d", inputsCpy.front().mFrameIndex );
                        inputsCpy.pop();
                    }
                }
            }
            ImGui::PopItemWidth();
        }
    };
}