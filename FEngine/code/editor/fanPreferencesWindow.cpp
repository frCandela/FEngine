#include "fanIncludes.h"

#include "editor/fanPreferencesWindow.h"
#include "fanEngine.h"
#include "util/fanImguiUtil.h"

namespace editor {

	//================================================================================================================================
	//================================================================================================================================
	PreferencesWindow::PreferencesWindow() {

	}

	//================================================================================================================================
	//================================================================================================================================
	void PreferencesWindow::Draw() {
		fan::Engine& engine = fan::Engine::GetEngine();

		if (IsVisible() == true ) {
			bool isVisible = IsVisible();
			if (ImGui::Begin("Preferences", &isVisible)) {
				if (ImGui::CollapsingHeader("3D Grid")) {
					fan::Engine::EditorGrid  gridData = engine.GetEditorGrid();

					if (ImGui::DragFloat("spacing", &gridData.spacing)) {
						engine.SetEditorGrid(gridData);
					}

					if (ImGui::DragInt("lines count", &gridData.linesCount)) {
						engine.SetEditorGrid(gridData);
					}

					if (ImGui::ColorEdit3("Filter##1", &gridData.color.r, util::Imgui::colorEditFlags)) {
						engine.SetEditorGrid(gridData);
					}

				}
			} ImGui::End();
			SetVisible(isVisible);
		}
	}
}