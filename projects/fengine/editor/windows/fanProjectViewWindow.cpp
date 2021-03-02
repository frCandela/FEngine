#include "editor/windows/fanProjectViewWindow.hpp"

#include "network/singletons/fanTime.hpp"
#include "engine/singletons/fanScene.hpp"
#include "editor/singletons/fanEditorPlayState.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	ProjectViewWindow::ProjectViewWindow( /*const LaunchSettings::NetworkMode _launchMode*/ ) //@todo repair this
		: EditorWindow( "project view", ImGui::IconType::Joystick16 )
		, mIsHovered( false )
	{
		AddFlag( ImGuiWindowFlags_MenuBar );

		// compute project world str for
		/*switch( _launchMode )
		{
            case LaunchSettings::NetworkMode::EditorClient:
                memcpy( mStringProjectSelectionCombo, "client\0\0", 8 );
                break;
            case LaunchSettings::NetworkMode::EditorServer:
                memcpy( mStringProjectSelectionCombo, "server\0\0", 8 );
                break;
            case LaunchSettings::NetworkMode::EditorClientServer:*/
                memcpy( mStringProjectSelectionCombo, "client\0server\0\0,", 16 );/*
                break;
            default:
                fanAssert( false );
                break;
		}*/
	}

	//========================================================================================================
	//========================================================================================================
	void ProjectViewWindow::OnGui( EcsWorld& _world )
	{
		// update window size
		const ImVec2 imGuiSize = ImGui::GetContentRegionAvail();
		btVector2 size = btVector2( imGuiSize.x, imGuiSize.y );
		if ( mSize != size )
		{
            mSize = size;
			mOnSizeChanged.Emmit( { (uint32_t)size[0], (uint32_t)size[1] } );
		}

		// draw menu bar
		if ( ImGui::BeginMenuBar() )
		{
			
			ImGui::Text( "%d x %d", ( int ) size.x(), ( int ) size.y() );

			const ImVec4 disabledColor = ImVec4( 0.3f, 0.3f, 0.3f, 0.3f );

            const EditorPlayState& playState = _world.GetSingleton<EditorPlayState>();
			if ( playState.mState == EditorPlayState::STOPPED )
			{
				// Play
                if( ImGui::ButtonIcon( ImGui::Play16,
                                       { 16, 16 },
                                       -1,
                                       ImVec4( 0, 0, 0, 0 ),
                                       ImVec4( 1.f, 1.f, 1.f, 1.f ) ) )
                {
					mOnPlay.Emmit();
				}
			}
			else
			{
				// Stop
				if ( ImGui::ButtonIcon( ImGui::Stop16, { 16, 16 }, -1, ImVec4( 0, 0, 0, 0 ) ) )
				{
					mOnStop.Emmit();
				}
			}

			const ImVec4 pauseTint
				= playState.mState == EditorPlayState::PLAYING ? ImVec4( 1.f, 1.f, 1.f, 1.f )
				: playState.mState == EditorPlayState::PAUSED ? ImVec4( 0.9f, 0.9f, 0.9f, 1.f )
				: disabledColor;

			// Pause
			if ( ImGui::ButtonIcon( ImGui::Pause16, { 16, 16 }, -1, ImVec4( 0, 0, 0, 0.f ), pauseTint ) )
			{
				if ( playState.mState == EditorPlayState::PLAYING ) { mOnPause.Emmit(); }
				else if ( playState.mState == EditorPlayState::PAUSED ) { mOnResume.Emmit(); }
			}

            // Step
            const ImVec4 stepTint = playState.mState == EditorPlayState::PAUSED
                    ? ImVec4( 1.f, 1.f, 1.f, 1.f )
                    : disabledColor;
            if( ImGui::ButtonIcon( ImGui::Step16, { 16, 16 }, -1, ImVec4( 0, 0, 0, 0 ), stepTint ) &&
                playState.mState == EditorPlayState::PAUSED )
			{
				mOnStep.Emmit();
			}

			// combo current world			
			ImGui::Spacing();
			ImGui::SetNextItemWidth( 200.f );
			if( ImGui::Combo( "##current project", &mCurrentProject, mStringProjectSelectionCombo ) )
			{
				mOnSelectProject.Emmit( mCurrentProject );
			}

			ImGui::EndMenuBar();
		}

		// update window position
		const ImVec2 cursorPos = ImGui::GetCursorPos();
		const ImVec2 windowPos = ImGui::GetWindowPos();

        mPosition = btVector2( cursorPos.x + windowPos.x, cursorPos.y + windowPos.y );

		// Draw project
		ImGui::Image( ( void* ) 12, imGuiSize );

        mIsHovered = ImGui::IsItemHovered();
	}
}