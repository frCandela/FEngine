#prama once

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    void UIMainMenu::SetInfo( EcsSingletonInfo& _info )
    {
        _info.mIcon       = ImGui::IconType::MainMenu16;
        _info.mGroup      = EngineGroups::Game;
        _info.onGui       = &UIMainMenu::OnGui;
        _info.mName       = "main menu";
    }

    //========================================================================================================
    //========================================================================================================
    void UIMainMenu::OnGui( EcsWorld& /*_world*/, EcsSingleton& _singleton )
    {
        UIMainMenu& menu = static_cast<UIMainMenu&>( _singleton );
        ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
        {
            ImGui::FanComponent( "main menu", menu.mMainMenuNode);
            ImGui::FanComponent( "options", menu.mOptionsNode );
            ImGui::FanComponent( "credits", menu.mCreditsNode );

            ImGui::Spacing();

            if( ImGui::Button("hide all")){ HideAll( menu ); }
            ImGui::SameLine();
            if( ImGui::Button("show main menu")){ ShowMainMenu( menu ); }
            ImGui::SameLine();
            if( ImGui::Button("show options")){ ShowOptions( menu ); }
            ImGui::SameLine();
            if( ImGui::Button("show credits")){ ShowCredits( menu ); }

        } ImGui::PopItemWidth();
    }
}
