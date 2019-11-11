#pragma once

namespace std {
	namespace experimental {
		namespace filesystem {
			static string file_name(const path& _path);
		}
	}
}

namespace ImGui
{
	const int fanColorEditFlags = ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_Float;;

	void FanShowHelpMarker(const char* _desc);
	void TFanoolTip( const char* _desc );
	bool FanSaveFileModal(const char * _popupName,const std::set<std::string>& _extensionWhiteList,std::fs::path & _currentPath,int & _extensionIndex);
	bool FanLoadFileModal(const char * _popupName,const std::set<std::string>& _extensionWhiteList,	std::fs::path & _path);

	namespace impl {
		bool FilesSelector(
			const std::set<std::string>& _extensionWhiteList,
			std::fs::path & _path
		);
	}
}
