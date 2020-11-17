#pragma once

#include "network/components/fanHostConnection.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"
#include "editor/gui/fanGuiNetworkCommon.hpp"

namespace fan
{
    struct GuiHostConnection
    {
        //====================================================================================================
        //====================================================================================================
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::Socket16;
            info.mGroup      = EngineGroups::Network;
            info.onGui       = &GuiHostConnection::OnGui;
            info.mEditorName = "host connection";
            return info;
        }

        //========================================================================================================
        // Editor gui helper
        //========================================================================================================
        static std::string GetStateName( const HostConnection::State _clientState )
        {
            switch( _clientState )
            {
                case HostConnection::Disconnected:
                    return "Disconnected";
                    break;
                case HostConnection::NeedingApprouval:
                    return "NeedingApprouval";
                    break;
                case HostConnection::PendingApprouval:
                    return "PendingApprouval";
                    break;
                case HostConnection::Connected:
                    return "Connected";
                    break;
                default:
                    fanAssert( false );
                    return "error";
                    break;
            }
        }

        //========================================================================================================
        // returns a color corresponding to a rtt time in seconds
        //========================================================================================================
        static ImVec4 GetStateColor( const HostConnection::State _clientState )
        {
            switch( _clientState )
            {
                case HostConnection::Disconnected:
                    return Color::sOrange.ToImGui();
                    break;
                case HostConnection::NeedingApprouval:
                    return Color::sYellow.ToImGui();
                    break;
                case HostConnection::PendingApprouval:
                    return Color::sYellow.ToImGui();
                    break;
                case HostConnection::Connected:
                    return Color::sGreen.ToImGui();
                    break;
                default:
                    fanAssert( false );
                    return Color::sPurple.ToImGui();
                    break;
            }
        }

        //====================================================================================================
        //====================================================================================================
        static void OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
        {
            HostConnection& hostConnection = static_cast<HostConnection&>( _component );
            const double currentTime = Time::ElapsedSinceStartup();
            ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 );
            {
                ImGui::Text( "name:               %s", hostConnection.mName.c_str() );
                ImGui::TextColored( GetStateColor( hostConnection.mState ),
                                    "state:              %s",
                                    GetStateName( hostConnection.mState ).c_str() );
                ImGui::TextColored( GetRttColor( hostConnection.mRtt ),
                                    "rtt:                %.1f",
                                    1000.f * hostConnection.mRtt );
                ImGui::Text( "bandwidth :         %.1f Ko/s", hostConnection.mBandwidth );
                ImGui::Text( "last response:      %.1f", currentTime - hostConnection.mLastResponseTime );
                ImGui::Text( "adress:             %s::%u",
                             hostConnection.mIp.toString().c_str(),
                             hostConnection.mPort );
                ImGui::Text( "target buffer size: %d", hostConnection.mTargetBufferSize );
                ImGui::Text( "frame delta:        %d %d %d",
                             hostConnection.mFramesDelta[0],
                             hostConnection.mFramesDelta[1],
                             hostConnection.mFramesDelta[2] );
                ImGui::DragFloat( "ping delay", &hostConnection.mPingDelay, 0.1f, 0.f, 10.f );
                ImGui::DragFloat( "timeout time", &hostConnection.mTimeoutDelay, 0.1f, 0.f, 10.f );
            }
            ImGui::PopItemWidth();
        }
    };
}