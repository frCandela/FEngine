#include "fanIncludes.h"

#include "editor/fanModals.h"
#include "core/fanInput.h"



//================================================================================================================================
// Extracts the file name of a path without the extension
//================================================================================================================================
std::string std::fs::file_name(const std::fs::path& _path) {
	if (std::fs::is_directory(_path) == true || _path.has_filename() == false) {
				return "";
	}
	std::string filename = _path.filename().string();
	if (_path.has_extension()) {
		const size_t lastindexBuffer = filename.find_last_of(".");
		filename = filename.substr(0, lastindexBuffer);
	}
	return filename;
}


namespace fan
{
	//================================================================================================================================
	// Constants
	//================================================================================================================================
	const int gui::colorEditFlags = ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_AlphaBar;

	//================================================================================================================================
	//================================================================================================================================
	void gui::ShowHelpMarker(const char* _desc)
	{
		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered()) {
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(_desc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	bool gui::FilesSelector(
		const std::set< std::string >& _extensionWhiteList
		, std::fs::path& _path)
	{
		bool returnValue = false;

		//std::fs::directory_entry _nextPath = std::fs::directory_entry(_path);

		ImGui::SetNextTreeNodeOpen(true);
		if (ImGui::TreeNode("File Selection")) {

			// '.' Selectable to go to the parent 
			if (_path != "." && ImGui::Selectable(".", false, ImGuiSelectableFlags_DontClosePopups | ImGuiSelectableFlags_AllowDoubleClick)) {
				_path = std::fs::directory_entry(_path.parent_path());
				if (_path.empty()) {
					_path = std::fs::path(".");
				}
			}

			// Lists all directories
			const std::fs::path dir = std::fs::is_directory(_path) ? _path : _path.parent_path(); // iterates on a dir, bot a file
			for (const std::fs::path& childPath : std::fs::directory_iterator(dir)) {

				std::string extension = childPath.extension().string();
				const bool isFile = _extensionWhiteList.find(extension) != _extensionWhiteList.end();
				const bool isDirectory = std::fs::is_directory(childPath);
				// Filter
				if (isFile || isDirectory) {
					const std::string filename = childPath.filename().generic_string();

					if (isFile) {
						ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)(ImColor(1.0f, 0.712f, 0.0f)));
					}
					if (ImGui::Selectable(filename.c_str(), false, ImGuiSelectableFlags_DontClosePopups | ImGuiSelectableFlags_AllowDoubleClick)) {
						_path = std::fs::directory_entry(childPath);

						if (ImGui::IsMouseDoubleClicked(0) && isFile) {
							returnValue = true;
						}
					}
					if (isFile) {
						ImGui::PopStyleColor();
					}
				}
			}

			ImGui::TreePop();
		}

		return returnValue;
	}



	//================================================================================================================================
	//================================================================================================================================
	bool gui::SaveFileModal(const char * _popupName, const std::set<std::string>& _extensionWhiteList, std::fs::path & _path, int & _extensionIndex) {
		bool returnValue = false;

		ImGui::SetNextWindowSize({ 316,410 });
		if (ImGui::BeginPopupModal(_popupName))
		{
			// Files hierarchy
			bool itemDoubleClicked = false;
			ImGui::BeginChild("load_scene_hierarchy", { 300,300 }, true); {
				itemDoubleClicked = gui::FilesSelector(_extensionWhiteList, _path);
			} ImGui::EndChild();


			// Input name
			const int bufferSize = 32;
			char buffer[bufferSize];
			strcpy_s(buffer, std::fs::file_name(_path).c_str());
			ImGui::InputText("name", buffer, bufferSize);
			std::string stringBuffer = buffer;

			// Format selection
			std::stringstream extensions;
			std::vector<std::string> tmpExtensions;
			for (const std::string& extension : _extensionWhiteList) {
				extensions << extension << '\0';
				tmpExtensions.push_back(extension);
			}
			if (_extensionWhiteList.size() > 1) {
				ImGui::Combo("format", &_extensionIndex, extensions.str().c_str());
			}
			else {
				ImGui::TextDisabled(extensions.str().c_str());
			}
			ImGui::Separator();

			// re format to path/.../filename.extension	
			if (std::fs::is_directory(_path) == false) {
				_path.remove_filename();
			}
			if (stringBuffer.length() > 0) {
				_path.append(stringBuffer + tmpExtensions[_extensionIndex]);
			}

			// Ok Button
			if (itemDoubleClicked == true || ImGui::Button("Ok") || Keyboard::IsKeyPressed(GLFW_KEY_ENTER)) {
				if (std::fs::is_directory(_path) == false) {
					ImGui::CloseCurrentPopup();
					returnValue = true;
				}
			}

			// Cancel button
			ImGui::SameLine();
			if (ImGui::Button("Cancel") || Keyboard::IsKeyPressed(GLFW_KEY_ESCAPE)) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
		return returnValue;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool gui::LoadFileModal(
		const char * _popupName,
		const std::set<std::string>& _extensionWhiteList,
		std::fs::path & _path) {

		bool returnValue = false;

		ImGui::SetNextWindowSize({ 316,400 });
		if (ImGui::BeginPopupModal(_popupName)) {
			ImGui::BeginChild("load_scene_hierarchy", { 300,300 }, true);
			std::fs::directory_entry newEntry;
			bool itemDoubleClicked = gui::FilesSelector(_extensionWhiteList, _path);
			if (std::fs::is_directory(newEntry)) {
				_path = newEntry;
			}
			else if (std::fs::is_regular_file(newEntry)) {
				_path = newEntry;
			}
			ImGui::EndChild();


			ImGui::Text(_path.string().c_str());
			ImGui::Separator();


			if (itemDoubleClicked == true || ImGui::Button("Ok") || Keyboard::IsKeyPressed(GLFW_KEY_ENTER)) {

				if (std::fs::is_regular_file(_path)) {
					ImGui::CloseCurrentPopup();
					returnValue = true;
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel") || Keyboard::IsKeyPressed(GLFW_KEY_ESCAPE)) {
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
		return returnValue;
	}
}
