#pragma once

#include "game/singletons/ui/fanUIMainMenu.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    struct GuiUIMainMenu
    {
        //====================================================================================================
        //====================================================================================================
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mIcon  = ImGui::IconType::MainMenu16;
            info.mGroup = EngineGroups::Game;
            info.onGui  = &GuiUIMainMenu::OnGui;
            info.mEditorName  = "main menu";
            return info;
        }

        //====================================================================================================
        //====================================================================================================
        static void OnGui( EcsWorld& /*_world*/, EcsSingleton& _singleton )
        {
            UIMainMenu& menu = static_cast<UIMainMenu&>( _singleton );
            ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
            {
                ImGui::FanComponent( "main menu", menu.mMainMenuNode );
                ImGui::FanComponent( "options", menu.mOptionsNode );
                ImGui::FanComponent( "credits", menu.mCreditsNode );

                ImGui::Spacing();

                if( ImGui::Button( "hide all" ) ){ UIMainMenu::HideAll( menu ); }
                ImGui::SameLine();
                if( ImGui::Button( "show main menu" ) ){ UIMainMenu::ShowMainMenu( menu ); }
                ImGui::SameLine();
                if( ImGui::Button( "show options" ) ){ UIMainMenu::ShowOptions( menu ); }
                ImGui::SameLine();
                if( ImGui::Button( "show credits" ) ){ UIMainMenu::ShowCredits( menu ); }
            }
            ImGui::PopItemWidth();
        }
    };
}
