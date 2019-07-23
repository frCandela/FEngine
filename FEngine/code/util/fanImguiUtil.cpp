#include "fanIncludes.h"

#include "util/fanImguiUtil.h"
#include "util/fanInput.h"

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
		const std::set< std::string >& _extensionWhiteList )
	{
		using namespace std::experimental::filesystem;

		directory_entry nextPath = directory_entry(_currentPath);

		ImGui::SetNextTreeNodeOpen(true);
		if (ImGui::TreeNode("File Selection")) {


			if (_currentPath != "." && ImGui::Selectable(".", false, ImGuiSelectableFlags_DontClosePopups | ImGuiSelectableFlags_AllowDoubleClick)) {
				nextPath = directory_entry(_currentPath.parent_path());
				if (nextPath.path().empty()) {
					nextPath = directory_entry(".");
				}				
			}
				// Lists all directories
			for (const directory_entry& childPath : directory_iterator(_currentPath)) {

				std::string extension = childPath.path().extension().generic_string();		
				const bool isFile = _extensionWhiteList.find(extension) != _extensionWhiteList.end();
				const bool isDirectory = is_directory(childPath);
				// Filter
				if ( isFile || isDirectory ){

					if (isFile) {
						ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)(ImColor(1.0f, 0.712f, 0.0f)));
					}

					std::string filename = childPath.path().filename().generic_string();
					if (ImGui::Selectable(filename.c_str(), false, ImGuiSelectableFlags_DontClosePopups | ImGuiSelectableFlags_AllowDoubleClick)) {
						nextPath = directory_entry(childPath);						
					}
					if (isFile) {
						ImGui::PopStyleColor();
					}
				}
			}
			
			ImGui::TreePop();
		}
		return nextPath;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Imgui::SaveFileModal( const char * _popupName, std::experimental::filesystem::path & _currentPath , const std::set<std::string>& _extensionWhiteList) {
		bool returnValue = false;

		ImGui::SetNextWindowSize({316,410});
		if (ImGui::BeginPopupModal(_popupName))		{
			ImGui::BeginChild("load_scene_hierarchy", {300,300}, true);	{
				std::experimental::filesystem::directory_entry newEntry = util::Imgui::FilesSelector(_currentPath, _extensionWhiteList);
				if (std::experimental::filesystem::is_directory(newEntry)) {
					_currentPath = newEntry;
				}
			} ImGui::EndChild();

			static char buffer[32];
			ImGui::InputText("name", buffer, 32);
			static int item_current_2 = 0;


			std::stringstream extensions;
 			for ( const std::string& extension : _extensionWhiteList ) {
 				extensions << extension << '\0';
 			} extensions << ".tato" << '\0';
			ImGui::Combo("format", &item_current_2, extensions.str().c_str() );
			ImGui::Separator();


			if( ImGui::Button("Ok")|| Keyboard::IsKeyPressed( GLFW_KEY_ENTER )){
				ImGui::CloseCurrentPopup();
				if (std::experimental::filesystem::is_regular_file(_currentPath)) {
					returnValue = true;
				}
			}
			ImGui::SameLine();
			if( ImGui::Button("Cancel") || Keyboard::IsKeyPressed( GLFW_KEY_ESCAPE )){
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
		return returnValue;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Imgui::LoadFileModal( 		
		const char * _popupName,
		std::experimental::filesystem::path & _currentPath,
		std::experimental::filesystem::path & _currentFile,
		std::set<std::string>& _extensionWhiteList ){

		bool returnValue = false;

		ImGui::SetNextWindowSize({316,400});
		if (ImGui::BeginPopupModal(_popupName))		{
			ImGui::BeginChild("load_scene_hierarchy", {300,300}, true);
			std::experimental::filesystem::directory_entry newEntry = util::Imgui::FilesSelector(_currentPath, _extensionWhiteList);

			if (std::experimental::filesystem::is_directory(newEntry)) {
				_currentPath = newEntry;
			} else if (std::experimental::filesystem::is_regular_file(newEntry)) {
				_currentFile = newEntry;					
			}
			ImGui::EndChild();


			ImGui::Text(_currentFile.string().c_str());
			ImGui::Separator();


			if( ImGui::Button("Ok")|| Keyboard::IsKeyPressed( GLFW_KEY_ENTER )){
				ImGui::CloseCurrentPopup();
				if (std::experimental::filesystem::is_regular_file(_currentFile)) {
					returnValue = true;
				}
			}
			ImGui::SameLine();
			if( ImGui::Button("Cancel") || Keyboard::IsKeyPressed( GLFW_KEY_ESCAPE )){
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
		return returnValue;
	}
}