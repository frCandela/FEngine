#pragma once

#include "platform/input/fanInputManager.hpp"

namespace ImGui
{
    //==================================================================================================================================================================================================
    // Imgui function for setting a keyboard key
    // left clic opens a capture popup
    // right clic resets the key
    //==================================================================================================================================================================================================
    bool FanKeyboardKey( const char* _label, fan::Keyboard::Key* _key )
    {
        using namespace fan;
        ImGui::PushID( _key );
        {
            // Button
            if( ImGui::Button( Keyboard::GetKeyName( *_key ).c_str() ) )
            {
                ImGui::OpenPopup( "capture_keyboard_key" );
            }

            // Reset key
            if( ImGui::IsItemClicked( 1 ) )
            {
                *_key = Keyboard::NONE;
            }

            // Text
            if( !std::string( _label ).empty() )
            {
                ImGui::SameLine();
                ImGui::Text( _label );
            }

            // capture popup
            if( ImGui::BeginPopup( "capture_keyboard_key" ) )
            {
                ImGui::Text( "PRESS ANY KEY" );
                const std::vector<Keyboard::Key>& keysList = Keyboard::Get().GetKeysList();
                for( int keyIndex = 0; keyIndex < (int)keysList.size(); keyIndex++ )
                {
                    if( Keyboard::IsKeyDown( keysList[keyIndex] ) )
                    {
                        *_key = keysList[keyIndex];
                        ImGui::CloseCurrentPopup();
                        break;
                    }
                }
                ImGui::EndPopup();
            }
        }
        ImGui::PopID();
        return false;
    }
}
