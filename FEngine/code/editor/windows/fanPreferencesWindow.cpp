#include "fanGlobalIncludes.h"

#include "editor/windows/fanPreferencesWindow.h"
#include "fanEngine.h"
#include "editor/fanModals.h"

namespace fan
{

	//================================================================================================================================
	//================================================================================================================================
	PreferencesWindow::PreferencesWindow() :
		EditorWindow("preferences") {
	}

	//================================================================================================================================
	//================================================================================================================================
	void PreferencesWindow::OnGui() {
		ImGui::Text("Hello there ! ");
		ImGui::Text("There is nothing here, but you can stay if you want,");
		ImGui::Text("We have plenty of space for you :)");
	}
}