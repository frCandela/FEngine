#include "fanGlobalIncludes.h"
#include "editor/windows/fanGameWindow.h"

#include "core/input/fanMouse.h"
#include "scene/fanScene.h"
#include "core/time/fanTime.h"

namespace fan {

	//================================================================================================================================
	//================================================================================================================================
	GameWindow::GameWindow( ) :	EditorWindow("game", ImGui::IconType::JOYSTICK16 )
	{
		AddFlag(ImGuiWindowFlags_MenuBar); 
	}

	//================================================================================================================================
	//================================================================================================================================
	void GameWindow::OnGui() {

		// update window size
		const ImVec2 imGuiSize = ImGui::GetContentRegionAvail();
		btVector2 size = btVector2( imGuiSize.x,imGuiSize.y);
		if ( m_size != size )
		{
			m_size = size;			
			onSizeChanged.Emmit(size);
		}

		// draw menu bar
		if ( ImGui::BeginMenuBar() )
		{
			ImGui::Text( "%d x %d", (int)size.x(), (int)size.y() );

			const ImVec4 disabledColor = ImVec4( 0.3f, 0.3f, 0.3f, 0.3f );
			const Scene::State state = m_scene->GetState();
			
			if ( state == Scene::STOPPED )
			{
				// Play
				if ( ImGui::ButtonIcon( ImGui::PLAY16, { 16,16 }, -1, ImVec4( 0, 0, 0, 0 ) , ImVec4( 1.f, 1.f, 1.f, 1.f ) ) )
				{
					onPlay.Emmit();
				}
			}
			else
			{
				// Stop
				if ( ImGui::ButtonIcon( ImGui::STOP16, { 16,16 }, -1, ImVec4( 0, 0, 0, 0 ) ) )
				{
					onStop.Emmit();
				}
			}				
			
			const ImVec4 pauseTint 
				= state == Scene::PLAYING ? ImVec4( 1.f, 1.f, 1.f, 1.f )
				: state == Scene::PAUSED ? ImVec4( 0.9f, 0.9f, 0.9f, 1.f )
				: disabledColor;

			// Pause
			if ( ImGui::ButtonIcon( ImGui::PAUSE16, { 16,16 }, -1, ImVec4( 0, 0, 0, 0.f ), pauseTint ) )
			{
				if ( state == Scene::PLAYING	 ) { onPause.Emmit();  }			
				else if ( state == Scene::PAUSED ) { onResume.Emmit(); }
			}			

			// Step
			const ImVec4 stepTint = state == Scene::PAUSED ? ImVec4( 1.f, 1.f, 1.f, 1.f ) : disabledColor;
			if ( ImGui::ButtonIcon( ImGui::STEP16, { 16,16 }, -1, ImVec4( 0, 0, 0, 0 ), stepTint ) && state == Scene::PAUSED )
			{
				onStep.Emmit();
			}

			ImGui::EndMenuBar();
		}

		// update window position
		const ImVec2 cursorPos = ImGui::GetCursorPos( );
		const ImVec2 windowPos = ImGui::GetWindowPos( );

		m_position = btVector2( cursorPos.x + windowPos.x, cursorPos.y + windowPos.y );

		// Draw game
		ImGui::Image( (void*)12, imGuiSize );

		m_isHovered = ImGui::IsItemHovered();
	}
}