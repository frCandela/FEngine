#pragma once

namespace util {
	class Imgui {
	public:
		static const int colorEditFlags;

		static void ShowHelpMarker(const char* _desc);
		static std::experimental::filesystem::directory_entry FilesSelector(
			const std::experimental::filesystem::path _currentPath,
			std::set<std::string>& _extensionWhiteList);
	};
}