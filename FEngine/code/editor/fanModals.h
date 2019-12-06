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
	void FanToolTip( const char* _desc );
	bool FanSaveFileModal(const char * _popupName,const std::set<std::string>& _extensionWhiteList,std::fs::path & _outCurrentPath);
	bool FanLoadFileModal(const char * _popupName,const std::set<std::string>& _extensionWhiteList,	std::fs::path & _path);
	void PushDisable();
	void PopDisable();

	namespace impl {
		bool FilesSelector(
			const std::set<std::string>& _extensionWhiteList,
			std::fs::path & _path
		);
	}
}
