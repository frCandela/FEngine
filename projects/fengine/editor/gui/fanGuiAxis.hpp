#pragma once

#include "core/input/fanAxis.hpp"

namespace ImGui
{
    //========================================================================================================
    //========================================================================================================
    bool FanAxis( const char* _label, fan::Axis* _axis )
    {
        using namespace fan;

        ImGui::PushID( _axis );
        {
            ImGui::PushItemWidth( 150 );

            int type = _axis->GetType();
            if( ImGui::Combo( "", &type, "keyboard\0joystick axis\0joystick buttons\0" ) &&
                Axis::Type( type ) != _axis->GetType() )
            {
                *_axis = Axis( _axis->GetName(), Axis::Type( type ) );
            }
            ImGui::PopItemWidth();

            ImGui::SameLine();

            bool invert = _axis->GetInvert();
            if ( ImGui::Checkbox( "##invert", &invert ) )
            {
                _axis->SetInvert( invert );
            }
            ImGui::SameLine();

            static Axis* sCapturingAxis = nullptr;
            static bool sPositiveKeyCaptured = true;


            bool openPopup[ 3 ] = { false,false,false };

            switch ( _axis->GetType() )
            {
                case fan::Axis::Keyboard:
                {
                    // keyboard button +
                    if ( ImGui::Button( Keyboard::GetKeyName( _axis->GetKeyPositive() ).c_str() ) )
                    {
                        openPopup[ Axis::Keyboard ] = true;
                        sCapturingAxis       = _axis;
                        sPositiveKeyCaptured = true;
                    }
                    // reset +
                    if( ImGui::IsItemClicked( 1 ) )
                    {
                        _axis->SetFromKeyboardKeys( Keyboard::NONE, _axis->GetKeyNegative() );
                    }

                    // keyboard button -
                    ImGui::SameLine();
                    const std::string name_neg = Keyboard::GetKeyName( _axis->GetKeyNegative() ) +
                                                 "##key_but_neg";
                    if( ImGui::Button( name_neg.c_str() ) )
                    {
                        openPopup[Axis::Keyboard] = true;
                        sCapturingAxis       = _axis;
                        sPositiveKeyCaptured = false;
                    }
                    // reset -
                    if( ImGui::IsItemClicked( 1 ) )
                    {
                        _axis->SetFromKeyboardKeys( _axis->GetKeyPositive(), Keyboard::NONE );
                    }
                }
                    break;

                case fan::Axis::JoystickAxis:
                    // Joystick buttons
                    if ( ImGui::Button( Joystick::Get().GetAxisName( _axis->GetJoystickAxis() ).c_str() ) )
                    {
                        openPopup[ Axis::JoystickAxis ] = true;
                        sCapturingAxis = _axis;
                    }
                    // reset
                    if ( ImGui::IsItemClicked( 1 ) ) { _axis->SetFromJoystickAxis( -1, Joystick::AXIS_NONE ); }
                    break;

                case fan::Axis::JoystickButton:
                {
                    // Joystick button +
                    ImGui::PushID( "joystick_button_pos" );
                    if ( ImGui::Button( Joystick::Get().GetButtonName( _axis->GetButtonPositive() ).c_str() ) )
                    {
                        openPopup[ Axis::JoystickButton ] = true;
                        sCapturingAxis       = _axis;
                        sPositiveKeyCaptured = true;
                    } ImGui::PopID();
                    // reset +
                    if ( ImGui::IsItemClicked( 1 ) )
                    {
                        _axis->SetFromJoystickButtons( _axis->GetJoystickID(), -1, _axis->GetButtonNegative() );
                    }

                    // Joystick button -
                    ImGui::SameLine();
                    const std::string name_neg = Joystick::Get().GetButtonName( _axis->GetButtonNegative() ) +
                                                 "##joy_but_neg";
                    if ( ImGui::Button( name_neg.c_str() ) )
                    {
                        openPopup[ Axis::JoystickButton ] = true;
                        sCapturingAxis       = _axis;
                        sPositiveKeyCaptured = false;
                    }
                    // reset
                    if ( ImGui::IsItemClicked( 1 ) )
                    {
                        _axis->SetFromJoystickButtons( _axis->GetJoystickID(), _axis->GetButtonPositive(), -1 );
                    }

                } break;
                default:
                    fanAssert( false );
                    break;
            }

            ImGui::SameLine();
            ImGui::Text( _label );

            // Capture joystick axis popup
            if ( openPopup[ Axis::JoystickAxis ] ) { ImGui::OpenPopup( "capture_joystick_axis" ); }
            if ( ImGui::BeginPopup( "capture_joystick_axis" ) )
            {
                if ( sCapturingAxis == _axis )
                {
                    ImGui::Text( "PRESS ANY KEY" );

                    // Look for a pressed axis
                    for ( int joystickIndex = 0; joystickIndex < Joystick::NUM_JOYSTICK; joystickIndex++ )
                    {
                        if ( Joystick::Get().IsConnected( joystickIndex ) &&
                             Joystick::Get().IsGamepad( joystickIndex ) )
                        {
                            const std::vector< Joystick::Axis >& axes = Joystick::Get().GetGamepadAxisList();
                            for ( int axisIndex = 0; axisIndex < (int) axes.size(); axisIndex++ )
                            {
                                float axisValue = Joystick::Get().GetAxis( joystickIndex, axes[ axisIndex ] );
                                if ( axisValue == 1.f )
                                {
                                    *sCapturingAxis = Axis( sCapturingAxis->GetName(),
                                                            fan::Axis::JoystickAxis );
                                    sCapturingAxis->SetFromJoystickAxis( joystickIndex, axisIndex );
                                    sCapturingAxis = nullptr;
                                    ImGui::CloseCurrentPopup();
                                    break;
                                }
                            }
                        }
                    }
                }
                ImGui::EndPopup();
            }

            // Capture keyboard axis popup
            if ( openPopup[ Axis::Keyboard ] ) { ImGui::OpenPopup( "capture_keyboard_axis" ); }
            if ( ImGui::BeginPopup( "capture_keyboard_axis" ) )
            {
                if ( sCapturingAxis == _axis )
                {
                    ImGui::Text( "PRESS ANY KEY" );
                    Keyboard& toto = Keyboard::Get();
                    const std::vector<Keyboard::Key>& keysList = toto.GetKeysList();
                    for ( int keyIndex = 0; keyIndex < (int)keysList.size(); keyIndex++ )
                    {
                        if ( Keyboard::IsKeyDown( keysList[ keyIndex ] ) )
                        {
                            Keyboard::Key positiveKey = sPositiveKeyCaptured ?
                                    keysList[ keyIndex ] :
                                    sCapturingAxis->GetKeyPositive();
                            Keyboard::Key negativeKey = !sPositiveKeyCaptured ?
                                    keysList[ keyIndex ] :
                                    sCapturingAxis->GetKeyNegative();
                            *sCapturingAxis = Axis( sCapturingAxis->GetName(), fan::Axis::Keyboard );
                            sCapturingAxis->SetFromKeyboardKeys( positiveKey, negativeKey );
                            sCapturingAxis = nullptr;
                            ImGui::CloseCurrentPopup();
                            break;
                        }
                    }
                }
                ImGui::EndPopup();
            }

            // Capture joystick button popup
            if ( openPopup[ Axis::JoystickButton ] ) { ImGui::OpenPopup( "capture_joystick_button" ); }
            if ( ImGui::BeginPopup( "capture_joystick_button" ) )
            {
                if ( sCapturingAxis == _axis )
                {
                    ImGui::Text( "PRESS ANY KEY" );

                    // Look for a pressed axis
                    for ( int joystickIndex = 0; joystickIndex < Joystick::NUM_JOYSTICK; joystickIndex++ )
                    {
                        if ( Joystick::Get().IsConnected( joystickIndex ) &&
                             Joystick::Get().IsGamepad( joystickIndex ) )
                        {
                            const std::vector< Joystick::Button >& buttons = Joystick::Get().GetGamepadButtonsList();
                            for ( int buttonIndex = 0; buttonIndex < (int)buttons.size(); buttonIndex++ )
                            {
                                if( Joystick::Get().GetButton( joystickIndex, buttons[buttonIndex] ) )
                                {
                                    Joystick::Button positiveButton = sPositiveKeyCaptured ?
                                            buttons[buttonIndex] :
                                            sCapturingAxis->GetButtonPositive();
                                    Joystick::Button negativeButton = !sPositiveKeyCaptured ?
                                            buttons[buttonIndex] :
                                            sCapturingAxis->GetButtonNegative();
                                    sCapturingAxis->SetFromJoystickButtons( joystickIndex,
                                                                            positiveButton,
                                                                            negativeButton );
                                    sCapturingAxis = nullptr;
                                    ImGui::CloseCurrentPopup();
                                    break;
                                }
                            }
                        }
                    }
                }
                ImGui::EndPopup();
            }
        }
        ImGui::PopID();
        return false;
    }
}