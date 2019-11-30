#include "fanGlobalIncludes.h"
#include "editor/windows/fanGameWindow.h"


namespace fan {

	//================================================================================================================================
	//================================================================================================================================
	GameWindow::GameWindow( ) :	EditorWindow("game", ImGui::IconType::JOYSTICK16 )
	{}

	//================================================================================================================================
	//================================================================================================================================
	void GameWindow::OnGui() {
		// Update window size
		 ImVec2 imGuiSize = ImGui::GetContentRegionAvail();
		const glm::ivec2 size = glm::ivec2( (int)imGuiSize.x,(int)imGuiSize.y);
		if ( m_size != size )
		{
			m_size = size;
			
			onSizeChanged.Emmit(size);
		}

		// Draw game
		ImGui::Image( (void*)12, imGuiSize );
	}
}