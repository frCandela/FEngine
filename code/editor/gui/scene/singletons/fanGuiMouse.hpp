#pragma once

#include "engine/singletons/fanMouse.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
    struct GuiMouse
    {
        //====================================================================================================
        //====================================================================================================
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mIcon  = ImGui::IconType::Input16;
            info.mGroup = EngineGroups::Scene;
            info.mEditorName  = "input mouse";
            info.onGui  = &GuiMouse::OnGui;
            return info;
        }

        //========================================================================================================
        //========================================================================================================
        static void OnGui( EcsWorld& /*_world*/, EcsSingleton& _singleton )
        {
            Mouse& mouse = static_cast<Mouse&>( _singleton );
            ImGui::PushReadOnly();
            ImGui::InputFloat2( "position", &mouse.mPosition.x );
            ImGui::InputFloat2( "local position", &mouse.mLocalPosition.x );
            ImGui::InputFloat2( "scroll delta", &mouse.mScrollDelta.x );
            ImGui::InputFloat2( "position delta", &mouse.mPositionDelta.x );
            ImGui::InputFloat2( "screen position", &mouse.mScreenPosition.x );
            ImGui::InputFloat2( "screen size", &mouse.mScreenSize.x );
            ImGui::Checkbox( "window hovered", &mouse.mWindowHovered );
            ImGui::Checkbox( "locked", &mouse.sLocked );
            ImGui::PopReadOnly();

            if( ImGui::CollapsingHeader( "key state" ) )
            {
                ImGui::PushReadOnly();
                ImGui::Columns( 4 );
                ImGui::Text( "key" );
                ImGui::NextColumn();
                ImGui::Text( "down" );
                ImGui::NextColumn();
                ImGui::Text( "pressed" );
                ImGui::NextColumn();
                ImGui::Text( "released" );
                ImGui::Spacing();
                ImGui::NextColumn();
                for( int i = 0; i < Mouse::Button::count; i++ )
                {
                    ImGui::PushID( i );
                    ImGui::Text( "button%d", i + 1 );
                    ImGui::NextColumn();
                    ImGui::Checkbox( "##down", &mouse.mDown[i] );
                    ImGui::NextColumn();
                    ImGui::Checkbox( "##pressed", &mouse.mPressed[i] );
                    ImGui::NextColumn();
                    ImGui::Checkbox( "##released", &mouse.mReleased[i] );
                    ImGui::NextColumn();
                    ImGui::PopID();
                }
                ImGui::Columns( 1 );
                ImGui::PopReadOnly();
            }
        }
    };
}
