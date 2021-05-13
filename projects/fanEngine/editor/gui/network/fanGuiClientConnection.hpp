#pragma once

#include "network/components/fanClientConnection.hpp"
#include "editor/fanGuiInfos.hpp"
#include "editor/gui/fanGuiNetworkCommon.hpp"

namespace fan
{
    struct GuiEntityClientConnection
    {
        //====================================================================================================
        //====================================================================================================
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::Socket16;
            info.mGroup      = EngineGroups::Network;
            info.onGui       = &GuiEntityClientConnection::OnGui;
            info.mEditorName = "client connection";
            return info;
        }

        //====================================================================================================
        // Editor gui helper
        //====================================================================================================
        static std::string GetStateName( const ClientConnection::ClientState _clientState )
        {
            switch( _clientState )
            {
                case fan::ClientConnection::ClientState::Disconnected:
                    return "Disconnected";
                    break;
                case fan::ClientConnection::ClientState::Stopping:
                    return "Stopping";
                    break;
                case fan::ClientConnection::ClientState::PendingConnection:
                    return "PendingConnection";
                    break;
                case fan::ClientConnection::ClientState::Connected:
                    return "Connected";
                    break;
                default:
                    fanAssert( false );
                    return "Error";
                    break;
            }
        }

        //==================================================================================================================================================================================================
        // returns a color corresponding to a rtt time in seconds
        //==================================================================================================================================================================================================
        static ImVec4 GetStateColor( const ClientConnection::ClientState _clientState )
        {
            switch( _clientState )
            {
                case fan::ClientConnection::ClientState::Disconnected:
                    return Color::sRed.ToImGui();
                case fan::ClientConnection::ClientState::Stopping:
                    return Color::sPurple.ToImGui();
                case fan::ClientConnection::ClientState::PendingConnection:
                    return Color::sYellow.ToImGui();
                case fan::ClientConnection::ClientState::Connected:
                    return Color::sGreen.ToImGui();
                default:
                    fanAssert( false );
                    return Color::sPurple.ToImGui();
            }
        }

        //====================================================================================================
        //====================================================================================================
        static void
        OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
        {
            ClientConnection& connection = static_cast<ClientConnection&>( _component );

            ImGui::DragFloat( "timeout time", &connection.mTimeoutDelay, 0.1f, 0.f, 10.f );
            ImGui::Text( "state:               " );
            ImGui::SameLine();
            ImGui::TextColored( GetStateColor( connection.mState ),
                                "%s",
                                GetStateName( connection.mState ).c_str() );
            ImGui::Text( "client port           %u", connection.mClientPort );
            ImGui::Text( "server adress         %s::%u", connection.mServerIP.toString().c_str(),
                         connection.mServerPort );
            ImGui::Text( "rtt                  " );
            ImGui::SameLine();
            ImGui::TextColored( GetRttColor( connection.mRtt ), "%.1f", 1000.f * connection.mRtt );
            ImGui::Text( "bandwidth:            %.1f Ko/s", connection.mBandwidth );
            ImGui::Text( "server last response: %.1f",
                         Time::ElapsedSinceStartup() - connection.mServerLastResponse );
        }
    };
}