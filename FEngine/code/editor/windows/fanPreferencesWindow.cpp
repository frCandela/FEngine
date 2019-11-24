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


		DrawJoysticks();
		DrawShortcuts();
	}

	//================================================================================================================================
	//================================================================================================================================
	void PreferencesWindow::DrawShortcuts()
	{
		// INPUT
		if ( ImGui::CollapsingHeader( "Shortcuts" ) )
		{
			const float column0_size = 150.f;

			// Axis keys
			{
				std::map< std::string, Axis >&		  axisList = Input::Get().Manager().GetListAxis();

				// Header
				ImGui::Text( "Axis                         type        invert      (+)             (-)         " );
				
				// Reset
				ImGui::SameLine();
				if ( ImGui::Button( "Save" ) ) { SerializedValues::Get().SaveValuesToDisk(); }

				// Reset
				ImGui::SameLine(); 
				if ( ImGui::Button( "Reset" ) ) { SerializedValues::Get().LoadKeyBindings(); }

				ImGui::SameLine(); ImGui::FanShowHelpMarker( " for a reset to engine default, delete the file editor_data.json" );

				ImGui::Indent();
				ImGui::Columns( 2, "columns_axis" );
				ImGui::SetColumnWidth( 0, column0_size );
				for ( auto& pair : axisList )
				{

					ImGui::Text( pair.first.c_str() ); // name
					ImGui::NextColumn();
					ImGui::FanAxis( "", &pair.second );// axis
					ImGui::NextColumn();

				} ImGui::Unindent();
			}

			ImGui::Columns( 1 );
			ImGui::Spacing();
			ImGui::Spacing();

			// Shortcut keys
			{
				std::map< std::string, InputManager::KeyboardEvent >& eventList = Input::Get().Manager().GetListKeyboardEvents();

				ImGui::Text( "Shortcuts                    key         __________________ modifiers __________________" );
				ImGui::Indent();
				ImGui::Columns( 2, "columns_keys" );
				ImGui::SetColumnWidth( 0, column0_size );
				for ( auto& pair : eventList )
				{
					ImGui::Text( pair.first.c_str() );
					ImGui::NextColumn();
					ImGui::FanKeyboardKey( "", &pair.second.key );  ImGui::SameLine();
					ImGui::FanKeyboardKey( "", &pair.second.mod0 ); ImGui::SameLine();
					ImGui::FanKeyboardKey( "", &pair.second.mod1 ); ImGui::SameLine();
					ImGui::FanKeyboardKey( "", &pair.second.mod2 );
					ImGui::NextColumn();
				}ImGui::Unindent();
			}
			ImGui::SetColumnWidth( 0, column0_size );
			ImGui::Columns( 1 );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void PreferencesWindow::DrawJoysticks()
	{
		if( ImGui::CollapsingHeader("joysticks") )
		{
			// creates columns
			const int numJoysticks = Joystick::Get().NumConnectedJoysticks();		
			if( numJoysticks > 1 ) {
				ImGui::Columns( numJoysticks, "columns_joysticks" );
			}

			// draw joysticks
			for ( int joystickIndex = 0; joystickIndex < Joystick::NUM_JOYSTICK; joystickIndex++ )
			{
				if ( Joystick::Get().IsConnected( joystickIndex ) )
				{					
					ImGui::Text( Joystick::Get().GetName( joystickIndex ).c_str() );

					// gamepad
					if ( Joystick::Get().IsGamepad( joystickIndex ) )
					{
						ImGui::SameLine();
						ImGui::Text( " -  %s", Joystick::Get().GetGamepadName( joystickIndex ).c_str() );

						if( numJoysticks > 1 ){ImGui::SetColumnWidth(-1, 400.f );}
						else { ImGui::PushItemWidth(400.f);}

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

						if ( numJoysticks > 1 ) { ImGui::NextColumn(); }
						else { ImGui::PopItemWidth(); }
						
					}
					else
					{
						ImGui::Text( "Unrecognized gamepad" );
					}					
				}
			}
			ImGui::Columns(1);
		}
		
	}
}