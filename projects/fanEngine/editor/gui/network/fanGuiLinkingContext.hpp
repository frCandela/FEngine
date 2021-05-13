#pragma once

#include "network/singletons/fanLinkingContext.hpp"
#include "editor/fanGuiInfos.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiLinkingContext
    {
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mIcon       = ImGui::Link16;
            info.mGroup      = EngineGroups::Network;
            info.onGui       = &GuiLinkingContext::OnGui;
            info.mEditorName = "linking context";
            return info;
        }

        static void OnGui( EcsWorld&, EcsSingleton& _component )
        {
            LinkingContext& linkingContext = static_cast<LinkingContext&>( _component );
            ImGui::Columns( 2 );

            ImGui::Text( "net ID" );
            ImGui::NextColumn();
            ImGui::Text( "entity handle" );
            ImGui::NextColumn();
            for( std::pair<NetID, EcsHandle> pair : linkingContext.mNetIDToEcsHandle )
            {
                ImGui::Text( "%d", pair.first );
                ImGui::NextColumn();
                ImGui::Text( "%d", pair.second );
                ImGui::NextColumn();
            }
            ImGui::Columns( 1 );
        }
    };
}