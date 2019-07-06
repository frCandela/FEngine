#include "fanIncludes.h"

#include "util/fanImguiUtil.h"

namespace util {

	//================================================================================================================================
	// Constants
	//================================================================================================================================
	const int Imgui::colorEditFlags = ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_AlphaBar;
	
	//================================================================================================================================
	//================================================================================================================================
	void Imgui::ShowHelpMarker(const char* _desc)
	{
		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered())	{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(_desc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	std::experimental::filesystem::directory_entry Imgui::FilesSelector(
		const std::experimental::filesystem::path _currentPath,
		std::set< std::string >& _extensionWhiteList )
	{
		using namespace std::experimental::filesystem;

		directory_entry nextPath = directory_entry(_currentPath);

		ImGui::SetNextTreeNodeOpen(true);
		if (ImGui::TreeNode("File Selection")) {


			if (_currentPath != "." && ImGui::Selectable(".", false, ImGuiSelectableFlags_DontClosePopups | ImGuiSelectableFlags_AllowDoubleClick)) {
				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
					nextPath = directory_entry(_currentPath.parent_path());
					if (nextPath.path().empty()) {
						nextPath = directory_entry(".");
					}
				}
			}
				// Lists all directories
			for (const directory_entry& childPath : directory_iterator(_currentPath)) {

				std::string extension = childPath.path().extension().generic_string();
				
				
				// Clicked
				if ( is_directory(childPath) || _extensionWhiteList.find(extension) != _extensionWhiteList.end() ){



					if (ImGui::Selectable(childPath.path().string().c_str(), false, ImGuiSelectableFlags_DontClosePopups | ImGuiSelectableFlags_AllowDoubleClick)) {

						if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
							nextPath = directory_entry(childPath);
						}
					}
				}
			}
			
			ImGui::TreePop();
		}
		return nextPath;
	}
}