#include "editor/windows/fanPreferencesWindow.hpp"

#include "platform/input/fanInput.hpp"
#include "core/time/fanProfiler.hpp"
#include "core/fanDebug.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "render/fanRenderer.hpp"
#include "editor/fanModals.hpp"
#include "editor/gui/fanGuiAxis.hpp"
#include "editor/fanGroupsColors.hpp"
#include "editor/gui/fanGuiInputManager.hpp"
#include "editor/singletons/fanEditorSettings.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void PreferencesWindow::SetInfo( EcsSingletonInfo& _info )
    {
        _info.mFlags |= EcsSingletonFlags::InitOnce;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void PreferencesWindow::Init( EcsWorld&, EcsSingleton& _singleton )
    {
        PreferencesWindow& preferencesWindow = static_cast<PreferencesWindow&>( _singleton );
        (void)preferencesWindow;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GuiPreferencesWindow::OnGui( EcsWorld& _world, EcsSingleton& _singleton )
    {
        PreferencesWindow& preferencesWindow = static_cast<PreferencesWindow&>( _singleton );
        SCOPED_PROFILE( preferences );

        ImGui::Icon( ImGui::Preferences16, { 16, 16 } );
        ImGui::SameLine();
        ImGui::Text( "Preferences" );

        EditorSettings& editorSettings = _world.GetSingleton<EditorSettings>();

        // IMGUI COLORS
        if( ImGui::CollapsingHeader( "Imgui Colors" ) )
        {
            ImGui::Indent();
            ImGuiStyle& style = ImGui::GetStyle();
            for( int i = 0; i < ImGuiCol_COUNT; i++ )
            {
                const char* name = ImGui::GetStyleColorName( i );
                ImGui::PushID( i );
                ImGui::ColorEdit4( name, (float*)&style.Colors[i], ImGui::fanColorEditFlags );
                ImGui::PopID();
            }
            if( ImGui::Button( "log colors cpp code" ) ){ PreferencesWindow::LogColorsCppInitCode(); }

            ImGui::Unindent();
        }

        // Groups colors
        if( ImGui::CollapsingHeader( "Groups Colors" ) )
        {
            ImGui::Indent();
            EditorSettings& settings = _world.GetSingleton<EditorSettings>();
            for( int i = 0; i < GroupsColors::sCount; i++ )
            {
                const char* name = GetEngineGroupName( EngineGroups( i ) );
                ImGui::PushID( i );
                ImGui::ColorEdit4( name, (float*)&settings.mData->mGroupsColors.mColors[i], ImGui::fanColorEditFlags );
                ImGui::PopID();
            }
            ImGui::Unindent();
        }

        DrawJoysticks( preferencesWindow );
        DrawShortcuts( editorSettings );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GuiPreferencesWindow::DrawShortcuts( EditorSettings& _editorSettings )
    {
        // INPUT
        if( ImGui::CollapsingHeader( "Shortcuts" ) )
        {
            const float column0_size = 150.f;

            // Axis keys
            {
                std::map<std::string, Axis>& axisList = Input::Get().Manager().GetListAxis();

                // Header
                ImGui::Text( "Axis                         "
                             "type        invert      (+)             (-)         " );

                // Reset
                ImGui::SameLine();
                if( ImGui::Button( "Save" ) )
                {
                    EditorSettingsData::SaveSettingsToJson( *_editorSettings.mData );
                    EditorSettingsData::SaveJsonToDisk( _editorSettings.mData->mJson );
                }

                // Reset
                ImGui::SameLine();
                if( ImGui::Button( "Reset" ) )
                {
                    EditorSettingsData::LoadJsonFromDisk( _editorSettings.mData->mJson );
                    EditorSettingsData::LoadSettingsFromJson( *_editorSettings.mData );
                }

                ImGui::SameLine();
                ImGui::FanShowHelpMarker( " for a reset to engine default, delete the file editor_data.json" );

                ImGui::Indent();
                ImGui::Columns( 2, "columns_axis" );
                ImGui::SetColumnWidth( 0, column0_size );
                for( auto& pair : axisList )
                {
                    ImGui::Text( pair.first.c_str() ); // name
                    ImGui::NextColumn();
                    ImGui::FanAxis( "", &pair.second );// axis
                    ImGui::NextColumn();
                }
                ImGui::Unindent();
            }

            ImGui::Columns( 1 );
            ImGui::Spacing();
            ImGui::Spacing();

            // Shortcut keys
            {
                std::map<std::string, InputManager::KeyboardEvent>& eventList = Input::Get().Manager().GetListKeyboardEvents();

                ImGui::Text( "Shortcuts                    "
                             "key         __________________ modifiers __________________" );
                ImGui::Indent();
                ImGui::Columns( 2, "columns_keys" );
                ImGui::SetColumnWidth( 0, column0_size );
                for( auto& pair : eventList )
                {
                    ImGui::Text( pair.first.c_str() );
                    ImGui::NextColumn();
                    ImGui::FanKeyboardKey( "", &pair.second.key );
                    ImGui::SameLine();
                    ImGui::FanKeyboardKey( "", &pair.second.mod0 );
                    ImGui::SameLine();
                    ImGui::FanKeyboardKey( "", &pair.second.mod1 );
                    ImGui::SameLine();
                    ImGui::FanKeyboardKey( "", &pair.second.mod2 );
                    ImGui::NextColumn();
                }
                ImGui::Unindent();
            }
            ImGui::SetColumnWidth( 0, column0_size );
            ImGui::Columns( 1 );
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GuiPreferencesWindow::DrawJoysticks( PreferencesWindow& _preferencesWindow )
    {
        (void)_preferencesWindow;

        if( ImGui::CollapsingHeader( "joysticks" ) )
        {
            // creates columns
            const int numJoysticks = Joystick::Get().NumConnectedJoysticks();
            if( numJoysticks > 1 )
            {
                ImGui::Columns( numJoysticks, "columns_joysticks" );
            }

            // draw joysticks
            for( int joystickIndex = 0; joystickIndex < Joystick::NUM_JOYSTICK; joystickIndex++ )
            {
                if( !Joystick::Get().IsConnected( joystickIndex ) ){ continue; }

                ImGui::Text( Joystick::Get().GetName( joystickIndex ).c_str() );

                // gamepad
                if( Joystick::Get().IsGamepad( joystickIndex ) )
                {
                    ImGui::SameLine();
                    ImGui::Text( " -  %s", Joystick::Get().GetGamepadName( joystickIndex ).c_str() );

                    if( numJoysticks > 1 ){ ImGui::SetColumnWidth( -1, 400.f ); }
                    else{ ImGui::PushItemWidth( 400.f ); }

                    // axes
                    const std::vector<Joystick::Axis>& axes = Joystick::Get().GetGamepadAxisList();
                    for( int axisIndex = 0; axisIndex < (int)axes.size(); axisIndex++ )
                    {
                        float axisValue = Joystick::Get().GetAxis( joystickIndex, axes[axisIndex] );
                        ImGui::SliderFloat( Joystick::Get().sAxisNames[axisIndex],
                                            &axisValue, -1.f, 1.f );
                    }

                    // buttons
                    const std::vector<Joystick::Button>& buttons =
                                                               Joystick::Get().GetGamepadButtonsList();
                    for( int buttonindex = 0; buttonindex < (int)buttons.size(); buttonindex++ )
                    {
                        bool buttonValue = Joystick::Get().GetButton( joystickIndex, buttons[buttonindex] );
                        ImGui::Checkbox( Joystick::Get().sButtonsNames[buttonindex], &buttonValue );
                    }

                    if( numJoysticks > 1 ){ ImGui::NextColumn(); }
                    else{ ImGui::PopItemWidth(); }
                }
                else
                {
                    ImGui::Text( "Unrecognized gamepad" );
                }
            }
            ImGui::Columns( 1 );
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void PreferencesWindow::LogColorsCppInitCode()
    {
        ImGuiStyle& style = ImGui::GetStyle();
        for( int i = 0; i < ImGuiCol_COUNT; i++ )
        {
            const char* name = ImGui::GetStyleColorName( i );
            ImVec4 color = style.Colors[i];
            Debug::Log() << "style.Colors[ "
                    << "ImGuiCol_" << name
                    << " ] = { "
                    << "(float)" << color.x << ", "
                    << "(float)" << color.y << ", "
                    << "(float)" << color.z << ", "
                    << "(float)" << color.w << " }; "
                    << Debug::Endl();

            ImGui::PushID( i );
            ImGui::ColorEdit4( name, (float*)&style.Colors[i], ImGui::fanColorEditFlags );
            ImGui::PopID();
        }
    }
}