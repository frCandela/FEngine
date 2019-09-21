#include "fanGlobalIncludes.h"

#include "editor/windows/fanPreferencesWindow.h"
#include "fanEngine.h"
#include "editor/fanModals.h"

namespace fan
{
	namespace editor {

		//================================================================================================================================
		//================================================================================================================================
		PreferencesWindow::PreferencesWindow() :
			Window("preferences") {
		}

		//================================================================================================================================
		//================================================================================================================================
		void PreferencesWindow::OnGui() {
			fan::Engine& engine = fan::Engine::GetEngine();
			if (ImGui::CollapsingHeader("3D Grid")) {
				fan::Engine::EditorGrid  gridData = engine.GetEditorGrid();

				if (ImGui::Checkbox("is visible", &gridData.isVisible)) {
					engine.SetEditorGrid(gridData);
				}

				if (ImGui::DragFloat("spacing", &gridData.spacing, 0.25f, 0.f, 100.f)) {
					engine.SetEditorGrid(gridData);
				}

				if (ImGui::DragInt("lines count", &gridData.linesCount, 1.f, 0, 1000)) {
					engine.SetEditorGrid(gridData);
				}

				if (ImGui::ColorEdit3("color", &gridData.color[0], gui::colorEditFlags)) {
					engine.SetEditorGrid(gridData);
				}
			}
		}
	}
}