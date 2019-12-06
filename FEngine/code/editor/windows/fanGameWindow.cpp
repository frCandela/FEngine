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

			if ( m_scene->GetState() == Scene::STOPPED )
			{
				if ( ImGui::ButtonIcon( ImGui::PLAY16, { 16,16 }, -1, ImVec4( 0, 0, 0, 0 ) ) )
				{
					m_scene->Play();
				}
			}
			else
			{
				if ( ImGui::ButtonIcon( ImGui::STOP16, { 16,16 }, -1, ImVec4( 0, 0, 0, 0 ) ) )
				{
					m_scene->Stop();
				}
			}	

			if ( ImGui::ButtonIcon( ImGui::PAUSE16, { 16,16 }, -1, ImVec4( 0, 0, 0, 0 ) ) )
			{
				Debug::Error("toto");
				if ( m_scene->GetState() == Scene::PLAYING )
				{
					m_scene->Pause();
				}
				else if ( m_scene->GetState() == Scene::PAUSED )
				{
					m_scene->Resume();
				}
			}

			if ( ImGui::ButtonIcon( ImGui::STEP16, { 16,16 }, -1, ImVec4( 0, 0, 0, 0 ) ) )
			{
				m_scene->Step( Time::Get().GetLogicDelta() );
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