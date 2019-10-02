#include "fanGlobalIncludes.h"

#include "editor/windows/fanPreferencesWindow.h"
#include "editor/fanModals.h"
#include "renderer/fanRenderer.h"
#include "renderer/fanRenderer.h"
#include "renderer/pipelines/fanPostprocessPipeline.h"

namespace fan
{

	//================================================================================================================================
	//================================================================================================================================
	PreferencesWindow::PreferencesWindow( Renderer * _renderer ) :
		EditorWindow("preferences") 
		, m_renderer( _renderer ) {
	}

	//================================================================================================================================
	//================================================================================================================================
	void PreferencesWindow::OnGui() {
		if ( ImGui::CollapsingHeader( "Post-processing" ) ) {
			// Filter color
			glm::vec4& color = m_renderer->GetPostprocessPipeline()->uniforms.color;
			ImGui::ColorEdit3( "Filter##1", &color[0], gui::colorEditFlags );
		}

		if ( ImGui::CollapsingHeader( "Global" ) ) {
			// Clear color
			glm::vec4 clearColor = m_renderer->GetClearColor();
			if ( ImGui::ColorEdit3( "Clear color", &clearColor.r, gui::colorEditFlags ) ) {
				m_renderer->SetClearColor( clearColor );
			}
		}
	}
}