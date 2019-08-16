#include "fanIncludes.h"

#include "editor/windows/fanConsoleWindow.h"

namespace editor {
	void ConsoleWindow::Draw() {
		if (IsVisible() == true) {
			bool visible = IsVisible();
			ImGui::Begin("Console", &visible); {

			} ImGui::End();
			SetVisible(visible);
		}
	}
}