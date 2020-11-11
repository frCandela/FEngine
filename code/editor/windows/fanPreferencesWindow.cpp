#include "editor/windows/fanPreferencesWindow.hpp"

#include "core/fanColor.hpp"
#include "core/input/fanInput.hpp"
#include "core/memory/fanSerializedValues.hpp"
#include "core/time/fanProfiler.hpp"
#include "core/fanDebug.hpp"
#include "scene/fanFullscreen.hpp"
#include "render/fanRenderer.hpp"
#include "editor/fanModals.hpp"
#include "editor/gui/fanGroupsColors.hpp"

namespace fan
{
	//========================================================================================================
    //========================================================================================================
    PreferencesWindow::PreferencesWindow( Renderer& _renderer, FullScreen& _fullScreen ) :
            EditorWindow( "preferences", ImGui::IconType::Preferences16 ),
            mRenderer( _renderer ),
            mFullScreen( _fullScreen )
    {
		// loads clear color
		Color clearColor;
		if( SerializedValues::Get().GetColor( "clear_color", clearColor ) )
		{
            mRenderer.mClearColor = clearColor.ToGLM();
		}

		// loads imgui colors
        PreferencesWindow::SetDefaultColors();
		ImGuiStyle& style = ImGui::GetStyle();
		for ( int i = 0; i < ImGuiCol_COUNT; i++ )
		{
			std::string name = "imgui_" + std::string( ImGui::GetStyleColorName( i ) );
			Color color;
			if ( SerializedValues::Get().GetColor( name.c_str(), color ) )
			{
				style.Colors[ i ] = color.ToImGui();
			}
		}

		// loads groups colors
		for( int i = 0; i < GroupsColors::sCount; i++ )
		{
			std::string name = "imgui_" + std::string( GetEngineGroupName( EngineGroups( i ) ) );
			Color color;
			if( SerializedValues::Get().GetColor( name.c_str(), color ) )
			{
				GroupsColors::sColors[i] = color.ToImGui();
			}
		}
	}

	//========================================================================================================
	//========================================================================================================
	PreferencesWindow::~PreferencesWindow()
	{
		SerializedValues::Get().SetColor( "clear_color", mRenderer.mClearColor );

		// saves imgui colors
		ImGuiStyle& style = ImGui::GetStyle();
		for ( int i = 0; i < ImGuiCol_COUNT; i++ )
		{
			std::string name = "imgui_" + std::string( ImGui::GetStyleColorName( i ) );
			SerializedValues::Get().SetColor( name.c_str(), style.Colors[ i ] );
		}

		// saves groups colors
		for( int i = 0; i < GroupsColors::sCount; i++ )
		{
			std::string name = "imgui_" + std::string( GetEngineGroupName( EngineGroups( i ) ) );
			SerializedValues::Get().SetColor( name.c_str(), GroupsColors::sColors[i] );
		}
	}

	//========================================================================================================
	//========================================================================================================
	void PreferencesWindow::OnGui( EcsWorld& /*_world*/ )
	{
		SCOPED_PROFILE( preferences );

		ImGui::Icon( GetIconType(), { 16,16 } ); ImGui::SameLine();
		ImGui::Text( "Preferences" );

		// RENDERING
		if ( ImGui::CollapsingHeader( "Rendering" ) )
		{
			ImGui::Indent();
		    mFullScreen.OnGui( mRenderer.mWindow );
			// Filter color
			glm::vec4& color = mRenderer.mDrawPostprocess.mUniforms.mColor;
			ImGui::ColorEdit3( "Filter##1", &color[ 0 ], ImGui::fanColorEditFlags );

			// Clear color
			glm::vec4 clearColor = mRenderer.mClearColor;
			if ( ImGui::ColorEdit3( "Clear color", &clearColor.r, ImGui::fanColorEditFlags ) )
			{
                mRenderer.mClearColor = clearColor;
			}
			ImGui::Unindent();
		}

		// IMGUI COLORS
		if ( ImGui::CollapsingHeader( "Imgui Colors" ) )
		{
			ImGui::Indent();
			ImGuiStyle& style = ImGui::GetStyle();
			for ( int i = 0; i < ImGuiCol_COUNT; i++ )
			{
				const char* name = ImGui::GetStyleColorName( i );
				ImGui::PushID( i );
				ImGui::ColorEdit4( name, ( float* ) &style.Colors[ i ], ImGui::fanColorEditFlags );
				ImGui::PopID();
			}
            if( ImGui::Button("log colors cpp code")){ LogColorsCppInitCode(); }

			ImGui::Unindent();
		}

		// Groups colors
		if( ImGui::CollapsingHeader( "Groups Colors" ) )
		{
			ImGui::Indent();
			for ( int i = 0; i < GroupsColors::sCount; i++)
			{
				const char* name = GetEngineGroupName( EngineGroups(i) );
				ImGui::PushID( i );
				ImGui::ColorEdit4( name, (float*)&GroupsColors::sColors[i], ImGui::fanColorEditFlags );
				ImGui::PopID();
			}
			ImGui::Unindent();
		}

		DrawJoysticks();
		DrawShortcuts();
	}
	//========================================================================================================
	//========================================================================================================
	void PreferencesWindow::DrawShortcuts()
	{
		// INPUT
		if ( ImGui::CollapsingHeader( "Shortcuts" ) )
		{
			const float column0_size = 150.f;

			// Axis keys
			{
				std::map< std::string, Axis >& axisList = Input::Get().Manager().GetListAxis();

				// Header
				ImGui::Text( "Axis                         "
                             "type        invert      (+)             (-)         " );

				// Reset
				ImGui::SameLine();
				if ( ImGui::Button( "Save" ) ) { SerializedValues::Get().SaveValuesToDisk(); }

				// Reset
				ImGui::SameLine();
				if ( ImGui::Button( "Reset" ) ) { SerializedValues::Get().LoadKeyBindings(); }

				ImGui::SameLine();
				ImGui::FanShowHelpMarker(" for a reset to engine default, delete the file editor_data.json" );

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
				std::map< std::string, InputManager::KeyboardEvent >& eventList =
				        Input::Get().Manager().GetListKeyboardEvents();

				ImGui::Text( "Shortcuts                    "
                             "key         __________________ modifiers __________________" );
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

	//========================================================================================================
	//========================================================================================================
	void PreferencesWindow::DrawJoysticks()
	{
		if ( ImGui::CollapsingHeader( "joysticks" ) )
		{
			// creates columns
			const int numJoysticks = Joystick::Get().NumConnectedJoysticks();
			if ( numJoysticks > 1 )
			{
				ImGui::Columns( numJoysticks, "columns_joysticks" );
			}

			// draw joysticks
			for ( int joystickIndex = 0; joystickIndex < Joystick::NUM_JOYSTICK; joystickIndex++ )
			{
				if ( ! Joystick::Get().IsConnected( joystickIndex ) ) { continue; }

				ImGui::Text( Joystick::Get().GetName( joystickIndex ).c_str() );

				// gamepad
				if ( Joystick::Get().IsGamepad( joystickIndex ) )
				{
					ImGui::SameLine();
					ImGui::Text( " -  %s", Joystick::Get().GetGamepadName( joystickIndex ).c_str() );

					if ( numJoysticks > 1 ) { ImGui::SetColumnWidth( -1, 400.f ); }
					else { ImGui::PushItemWidth( 400.f ); }

					// axes
					const std::vector< Joystick::Axis >& axes = Joystick::Get().GetGamepadAxisList();
					for ( int axisIndex = 0; axisIndex < (int)axes.size(); axisIndex++ )
					{
						float axisValue = Joystick::Get().GetAxis( joystickIndex, axes[ axisIndex ] );
                        ImGui::SliderFloat( Joystick::Get().sAxisNames[axisIndex],
                                            &axisValue, -1.f, 1.f );
					}

					// buttons
					const std::vector< Joystick::Button >& buttons =
					        Joystick::Get().GetGamepadButtonsList();
					for ( int buttonindex = 0; buttonindex < (int)buttons.size(); buttonindex++ )
					{
						bool buttonValue = Joystick::Get().GetButton( joystickIndex, buttons[ buttonindex ] );
						ImGui::Checkbox( Joystick::Get().sButtonsNames[ buttonindex ], &buttonValue );
					}

					if ( numJoysticks > 1 ) { ImGui::NextColumn(); }
					else { ImGui::PopItemWidth(); }

				}
				else
				{
					ImGui::Text( "Unrecognized gamepad" );
				}

			}
			ImGui::Columns( 1 );
		}

	}

    //========================================================================================================
    //========================================================================================================
    void PreferencesWindow::LogColorsCppInitCode()
    {
        ImGuiStyle& style = ImGui::GetStyle();
        for ( int i = 0; i < ImGuiCol_COUNT; i++ )
        {
            const char* name = ImGui::GetStyleColorName( i );
            ImVec4 color = style.Colors[ i ];
            Debug::Log() << "style.Colors[ "
                    << "ImGuiCol_" << name
                    << " ] = { "
                    << "(float)" << color.x << ", "
                    << "(float)" << color.y << ", "
                    << "(float)" << color.z << ", "
                    << "(float)" << color.w << " }; "
                    << Debug::Endl();

            ImGui::PushID( i );
            ImGui::ColorEdit4( name, ( float* ) &style.Colors[ i ], ImGui::fanColorEditFlags );
            ImGui::PopID();
        }
    }

    //========================================================================================================
    //========================================================================================================
    void PreferencesWindow::SetDefaultColors()
    {
        ImGuiStyle& style = ImGui::GetStyle();
        // code generated using PreferencesWindow::LogColorsCppInitCode
        style.Colors[ ImGuiCol_Text ] = { (float)0.635294, (float)0.635294, (float)0.635294, (float)1 };
        style.Colors[ ImGuiCol_TextDisabled ] = { (float)0.5, (float)0.5, (float)0.5, (float)1 };
        style.Colors[ ImGuiCol_WindowBg ] = { (float)0.131201, (float)0.131188, (float)0.131188, (float)1 };
        style.Colors[ ImGuiCol_ChildBg ] = { (float)1, (float)1, (float)1, (float)0 };
        style.Colors[ ImGuiCol_PopupBg ] = { (float)0.08, (float)0.08, (float)0.08, (float)0.94 };
        style.Colors[ ImGuiCol_Border ] = { (float)0.172522, (float)0.172539, (float)0.172529, (float)1 };
        style.Colors[ ImGuiCol_BorderShadow ] = { (float)0.0001, (float)9.999e-05, (float)9.999e-05, (float)0 };
        style.Colors[ ImGuiCol_FrameBg ] = { (float)0.247059, (float)0.247059, (float)0.247059, (float)0.541176 };
        style.Colors[ ImGuiCol_FrameBgHovered ] = { (float)0.509804, (float)0.509804, (float)0.509804, (float)0.4 };
        style.Colors[ ImGuiCol_FrameBgActive ] = { (float)0.564706, (float)0.564706, (float)0.564706, (float)0.670588 };
        style.Colors[ ImGuiCol_TitleBg ] = { (float)0.0941176, (float)0.0941176, (float)0.0941176, (float)1 };
        style.Colors[ ImGuiCol_TitleBgActive ] = { (float)0.0950447, (float)0.0950352, (float)0.0950352, (float)1 };
        style.Colors[ ImGuiCol_TitleBgCollapsed ] = { (float)0, (float)0, (float)0, (float)1 };
        style.Colors[ ImGuiCol_MenuBarBg ] = { (float)0.14, (float)0.14, (float)0.14, (float)1 };
        style.Colors[ ImGuiCol_ScrollbarBg ] = { (float)0.02, (float)0.02, (float)0.02, (float)0.53 };
        style.Colors[ ImGuiCol_ScrollbarGrab ] = { (float)0.31, (float)0.31, (float)0.31, (float)1 };
        style.Colors[ ImGuiCol_ScrollbarGrabHovered ] = { (float)0.41, (float)0.41, (float)0.41, (float)1 };
        style.Colors[ ImGuiCol_ScrollbarGrabActive ] = { (float)0.51, (float)0.51, (float)0.51, (float)1 };
        style.Colors[ ImGuiCol_CheckMark ] = { (float)0.403922, (float)1, (float)0.67451, (float)1 };
        style.Colors[ ImGuiCol_SliderGrab ] = { (float)0.658824, (float)0.658824, (float)0.658824, (float)1 };
        style.Colors[ ImGuiCol_SliderGrabActive ] = { (float)0.454902, (float)1, (float)0.756863, (float)1 };
        style.Colors[ ImGuiCol_Button ] = { (float)0.537255, (float)0.537255, (float)0.537255, (float)0.4 };
        style.Colors[ ImGuiCol_ButtonHovered ] = { (float)0.415686, (float)0.415686, (float)0.415686, (float)1 };
        style.Colors[ ImGuiCol_ButtonActive ] = { (float)0.482353, (float)0.482353, (float)0.482353, (float)1 };
        style.Colors[ ImGuiCol_Header ] = { (float)0.321569, (float)0.321569, (float)0.321569, (float)0.309804 };
        style.Colors[ ImGuiCol_HeaderHovered ] = { (float)0.352941, (float)0.352941, (float)0.352941, (float)0.8 };
        style.Colors[ ImGuiCol_HeaderActive ] = { (float)0.396078, (float)0.396078, (float)0.396078, (float)1 };
        style.Colors[ ImGuiCol_Separator ] = { (float)0.439216, (float)0.439216, (float)0.439216, (float)0.501961 };
        style.Colors[ ImGuiCol_SeparatorHovered ] = { (float)0.498039, (float)0.498039, (float)0.498039, (float)0.780392 };
        style.Colors[ ImGuiCol_SeparatorActive ] = { (float)0.509804, (float)0.509804, (float)0.509804, (float)1 };
        style.Colors[ ImGuiCol_ResizeGrip ] = { (float)0.360784, (float)0.360784, (float)0.360784, (float)0.25098 };
        style.Colors[ ImGuiCol_ResizeGripHovered ] = { (float)0.478431, (float)0.478431, (float)0.478431, (float)0.670588 };
        style.Colors[ ImGuiCol_ResizeGripActive ] = { (float)0.4, (float)0.4, (float)0.4, (float)0.94902 };
        style.Colors[ ImGuiCol_Tab ] = { (float)0.18, (float)0.35, (float)0.58, (float)0.862 };
        style.Colors[ ImGuiCol_TabHovered ] = { (float)0.26, (float)0.59, (float)0.98, (float)0.8 };
        style.Colors[ ImGuiCol_TabActive ] = { (float)0.2, (float)0.41, (float)0.68, (float)1 };
        style.Colors[ ImGuiCol_TabUnfocused ] = { (float)0.068, (float)0.102, (float)0.148, (float)0.9724 };
        style.Colors[ ImGuiCol_TabUnfocusedActive ] = { (float)0.136, (float)0.262, (float)0.424, (float)1 };
        style.Colors[ ImGuiCol_DockingPreview ] = { (float)0.26, (float)0.59, (float)0.98, (float)0.7 };
        style.Colors[ ImGuiCol_DockingEmptyBg ] = { (float)0.2, (float)0.2, (float)0.2, (float)1 };
        style.Colors[ ImGuiCol_PlotLines ] = { (float)0.529412, (float)0.529412, (float)0.529412, (float)1 };
        style.Colors[ ImGuiCol_PlotLinesHovered ] = { (float)1, (float)0.43, (float)0.35, (float)1 };
        style.Colors[ ImGuiCol_PlotHistogram ] = { (float)0.9, (float)0.7, (float)0, (float)1 };
        style.Colors[ ImGuiCol_PlotHistogramHovered ] = { (float)1, (float)0.6, (float)0, (float)1 };
        style.Colors[ ImGuiCol_TextSelectedBg ] = { (float)0.26, (float)0.59, (float)0.98, (float)0.35 };
        style.Colors[ ImGuiCol_DragDropTarget ] = { (float)1, (float)1, (float)0, (float)0.9 };
        style.Colors[ ImGuiCol_NavHighlight ] = { (float)0.26, (float)0.59, (float)0.98, (float)1 };
        style.Colors[ ImGuiCol_NavWindowingHighlight ] = { (float)1, (float)1, (float)1, (float)0.7 };
        style.Colors[ ImGuiCol_NavWindowingDimBg ] = { (float)0.8, (float)0.8, (float)0.8, (float)0.2 };
        style.Colors[ ImGuiCol_ModalWindowDimBg ] = { (float)0.8, (float)0.8, (float)0.8, (float)0.35 };
    }
}