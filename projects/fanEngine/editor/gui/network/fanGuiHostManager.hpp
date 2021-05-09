#pragma once

#include "network/singletons/fanHostManager.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiHostManager
    {
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mIcon       = ImGui::Network16;
            info.mGroup      = EngineGroups::Network;
            info.onGui       = &GuiHostManager::OnGui;
            info.mEditorName = "host manager";
            return info;
        }

        static void OnGui( EcsWorld&, EcsSingleton& _component )
        {
            HostManager& hostManager = static_cast<HostManager&>( _component );

            ImGui::Columns( 3 );

            ImGui::Text( "name" );
            ImGui::NextColumn();
            ImGui::Text( "ip" );
            ImGui::NextColumn();
            ImGui::Text( "port" );
            ImGui::NextColumn();
            for( const std::pair<HostManager::IPPort, EcsHandle>& pair : hostManager.mHostHandles )
            {
                ImGui::Text( "host%d", pair.second );
                ImGui::NextColumn();
                ImGui::Text( "%s", pair.first.mAdress.toString().c_str() );
                ImGui::NextColumn();
                ImGui::Text( "%d", pair.first.mPort );
                ImGui::NextColumn();
            }
            ImGui::Columns( 1 );
        }
    };
}