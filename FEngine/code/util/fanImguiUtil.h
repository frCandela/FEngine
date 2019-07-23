#pragma once

namespace util {
	class Imgui {
	public:
		static const int colorEditFlags;

		static void ShowHelpMarker(const char* _desc);

		static bool SaveFileModal( 
			const char * _popupName,
			std::experimental::filesystem::path & _currentPath,
			const std::set<std::string>& _extensionWhiteList
		);		

		static bool LoadFileModal( 		
			const char * _popupName,
			std::experimental::filesystem::path & _currentPath,
			std::experimental::filesystem::path & _currentFile,
			std::set<std::string>& _extensionWhiteList 
		);

	private:
		static std::experimental::filesystem::directory_entry FilesSelector(
			const std::experimental::filesystem::path _currentPath,
			const std::set<std::string>& _extensionWhiteList);

	};
}