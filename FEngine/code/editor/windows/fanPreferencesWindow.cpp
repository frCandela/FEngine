#include "fanGlobalIncludes.h"

#include "editor/windows/fanPreferencesWindow.h"
#include "editor/fanModals.h"
#include "renderer/fanRenderer.h"
#include "renderer/fanRenderer.h"
#include "renderer/pipelines/fanPostprocessPipeline.h"
#include "core/input/fanInputManager.h"
#include "core/input/fanInput.h"
#include "core/input/fanKeyboard.h"
#include "core/input/fanJoystick.h"
#include "core/files/fanSerializedValues.h"
#include "core/time/fanProfiler.h"
#include "editor/fanModals.h"


namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	PreferencesWindow::PreferencesWindow( Renderer * _renderer ) :
		EditorWindow("preferences", ImGui::IconType::PREFERENCES )
		, m_renderer( _renderer ) 
	{
		Color clearColor;
		if( SerializedValues::Get().GetColor( "clear_color", clearColor ) ) {
			m_renderer->SetClearColor( clearColor.ToGLM() );
		}

		ImGuiStyle& style = ImGui::GetStyle();
		for ( int i = 0; i < ImGuiCol_COUNT; i++ ) {
			std::string name = "imgui_" + std::string( ImGui::GetStyleColorName( i ) );
			Color color;
			if ( SerializedValues::Get().GetColor( name.c_str(), color ) ) {
				style.Colors[i] = ImVec4( color[0], color[1], color[2], color[3] );
			}
		}
	}
	//================================================================================================================================
	//================================================================================================================================
	PreferencesWindow::~PreferencesWindow() {
		SerializedValues::Get().SetColor( "clear_color", m_renderer->GetClearColor() );

		ImGuiStyle& style = ImGui::GetStyle();
		for ( int i = 0; i < ImGuiCol_COUNT; i++ ) {
			std::string name = "imgui_" + std::string( ImGui::GetStyleColorName( i ) );
			SerializedValues::Get().SetColor( name.c_str(), style.Colors[i] );
		}
		
	}

	//================================================================================================================================
	//================================================================================================================================
	void PreferencesWindow::OnGui() {
		SCOPED_PROFILE( preferences )

		ImGui::Icon( GetIconType(), { 16,16 } ); ImGui::SameLine();
		ImGui::Text( "Preferences" );

		// RENDERING
		if ( ImGui::CollapsingHeader( "Rendering" ) ) {
			// Filter color
			glm::vec4& color = m_renderer->GetPostprocessPipeline()->uniforms.color;
			ImGui::ColorEdit3( "Filter##1", &color[0], ImGui::fanColorEditFlags );
			
			// Clear color
			glm::vec4 clearColor = m_renderer->GetClearColor();
			if ( ImGui::ColorEdit3( "Clear color", &clearColor.r, ImGui::fanColorEditFlags ) ) {
				m_renderer->SetClearColor( clearColor );
			}
		}

		// IMGUI COLORS
		if ( ImGui::CollapsingHeader( "Imgui Colors" ) ) {
			ImGuiStyle& style = ImGui::GetStyle();
			for ( int i = 0; i < ImGuiCol_COUNT; i++ ) {
				const char* name = ImGui::GetStyleColorName( i );
				ImGui::PushID( i );
				ImGui::ColorEdit4( name, (float*)&style.Colors[i], ImGui::fanColorEditFlags );				
				ImGui::PopID();
			}
		}

		// Joysticks
		DrawJoysticks();

		// INPUT
		if ( ImGui::CollapsingHeader( "Shortcuts" ) ) {			
			const float column0_size = 150.f;		

			// Axis keys
			{
				std::map< std::string, Axis >&		  axisList = Input::Get().Manager().GetListAxis();

				m_uniqueKeyIndex = 0;
				ImGui::Text( "Axis                         type           ____ (+) ____    ____ (-) ____" );
				ImGui::SameLine(); ImGui::Text("        "); ImGui::SameLine(); if ( ImGui::Button( "Reset" ) ) { SerializedValues::Get().LoadKeyBindings(); }
				ImGui::SameLine(); ImGui::FanShowHelpMarker(" for a reset to engine default, delete the file editor_data.json");
				
				ImGui::Indent();
				ImGui::Columns( 2 );
				ImGui::SetColumnWidth( 0, column0_size );
				ImGui::PushItemWidth( 10.f );
				for ( auto& pair : axisList ) {

					ImGui::Text( pair.first.c_str() );
					ImGui::NextColumn();
					ImGui::FanAxis("", &pair.second );
					ImGui::NextColumn();

				} ImGui::Unindent();
				ImGui::PopItemWidth();
			}

			ImGui::Columns( 1 );
			ImGui::Spacing();
			ImGui::Spacing();

			// Shortcut keys
			{
				std::map< std::string, InputManager::KeyboardEvent >& eventList = Input::Get().Manager().GetListKeyboardEvents();

				ImGui::Text( "Shortcuts               ____ key ____    __________________ modifiers __________________" ); 
				ImGui::Indent();
				ImGui::Columns( 2 );
				ImGui::SetColumnWidth( 0, column0_size );
				for ( auto& pair : eventList ) {
					ImGui::Text( pair.first.c_str() );
					ImGui::NextColumn();
					SetKeyboardEventButton( pair.second.key );  ImGui::SameLine();
					SetKeyboardEventButton( pair.second.mod0 ); ImGui::SameLine();
					SetKeyboardEventButton( pair.second.mod1 ); ImGui::SameLine();
					SetKeyboardEventButton( pair.second.mod2 );
					ImGui::NextColumn();
				}ImGui::Unindent();
			}
		}		

		CaptureKeyPopup();
		DeleteKeyPopup();
	}

	//================================================================================================================================
	//================================================================================================================================
	void PreferencesWindow::DrawJoysticks()
	{
		if( ImGui::CollapsingHeader("joysticks") )
		{
			// creates columns
			const int numJoysticks = Joystick::Get().NumConnectedJoysticks();			
			if( numJoysticks > 1 ){
				ImGui::Columns( numJoysticks );
				for ( int columnIndex = 0; columnIndex < numJoysticks; columnIndex++ ) { ImGui::SetColumnWidth( columnIndex, 400 ); }
			}

			// draw joysticks
			for ( int joystickIndex = 0; joystickIndex <= GLFW_JOYSTICK_LAST; joystickIndex++ )
			{
				if ( Joystick::Get().IsConnected( joystickIndex ) )
				{					
					ImGui::Text( Joystick::Get().GetName( joystickIndex ).c_str() );

					// gamepad
					if ( Joystick::Get().IsGamepad( joystickIndex ) )
					{
						ImGui::SameLine();
						ImGui::Text( " -  %s", Joystick::Get().GetGamepadName( joystickIndex ).c_str() );

						// axes
						const std::vector< Joystick::Axis >& axes = Joystick::Get().GetGamepadAxisList();
						for ( int axisIndex = 0; axisIndex < axes.size(); axisIndex++ )
						{
							float axisValue = Joystick::Get().GetAxis( joystickIndex, axes[axisIndex] );
							ImGui::SliderFloat( Joystick::Get().s_axisNames[axisIndex], &axisValue, -1.f, 1.f );
						}

						// buttons
						const std::vector< Joystick::Button >& buttons = Joystick::Get().GetGamepadButtonsList();
						for ( int buttonindex = 0; buttonindex < buttons.size(); buttonindex++ )
						{
							bool buttonValue = Joystick::Get().GetButton( joystickIndex, buttons[buttonindex] );
							ImGui::Checkbox( Joystick::Get().s_buttonsNames[buttonindex], &buttonValue );
						}
					}
					else
					{
						ImGui::Text( "Unrecognized gamepad" );
					}
					ImGui::NextColumn();
				}
			}
			ImGui::Columns(1);
		}
		
	}

	//================================================================================================================================
	// Popup displayed when changing a keyboard key
	//================================================================================================================================
	void PreferencesWindow::CaptureKeyPopup() {
		if ( m_openCaptureKeyPopup ) {
			ImGui::OpenPopup( "capture_key_pref" );
			m_openCaptureKeyPopup = false;
		}

		if ( !ImGui::IsPopupOpen( "capture_key_pref" ) && m_buttonCaptureDestination != nullptr ) {
			StopCaptureKey();
		}

		// Capture key popup
		if ( ImGui::BeginPopup( "capture_key_pref" ) ) {
			ImGui::Text( "PRESS ANY KEY" );

			if ( m_buttonCaptureDestination == nullptr ) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}

	//================================================================================================================================
	// Popup displayed for deleting a keyboard key
	//================================================================================================================================
	void PreferencesWindow::DeleteKeyPopup() {
		// Delete key popup
		if ( m_openDeletePopup ) {
			ImGui::OpenPopup( "delete_key_pref" );
			m_openDeletePopup = false;
		}

		// delete popup
		if ( ImGui::BeginPopup( "delete_key_pref" ) ) {
			if ( ImGui::MenuItem( "Remove" ) ) {
				*m_buttonDeleteDestination = Keyboard::NONE;
				m_buttonDeleteDestination = nullptr;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		if ( !ImGui::IsPopupOpen( "delete_key_pref" ) && m_buttonCaptureDestination != nullptr ) {
			m_buttonDeleteDestination = nullptr;
		}
	}

	//================================================================================================================================
	// Display a keyboard key button and trigger the capture/deletion when clicked
	//================================================================================================================================
	void PreferencesWindow::SetKeyboardEventButton( int & _button ) {
		ImGui::PushID( m_uniqueKeyIndex++ );
		if ( ImGui::Button( Keyboard::GetKeyName( _button ).c_str() ) ) {
			StartCaptureKey(_button);
		}
		if ( ImGui::IsItemClicked( 1 ) ) {
			m_openDeletePopup = true;
			m_buttonDeleteDestination = &_button;
		}
		ImGui::PopID();
	}

	//================================================================================================================================
	// Called back when capturing a key
	// _key is the key captured
	//================================================================================================================================
	void PreferencesWindow::CaptureKeyCallback( Keyboard::Key _key ) {
		assert( m_buttonCaptureDestination != nullptr );
		* m_buttonCaptureDestination = _key;
		StopCaptureKey();
	}

	//================================================================================================================================
	// Launch the keyboard key capture
	//================================================================================================================================
	void PreferencesWindow::StartCaptureKey( int & _button ) {
		m_openCaptureKeyPopup = true;
		m_buttonCaptureDestination = &_button;
		Keyboard::Get().onKeyPressed.Connect( &PreferencesWindow::CaptureKeyCallback, this );
	}

	//================================================================================================================================
	// Stops the keyboard key capture
	//================================================================================================================================
	void PreferencesWindow::StopCaptureKey() {
		Keyboard::Get().onKeyPressed.Disconnect( &PreferencesWindow::CaptureKeyCallback, this );
		m_buttonCaptureDestination = nullptr;
	}
}