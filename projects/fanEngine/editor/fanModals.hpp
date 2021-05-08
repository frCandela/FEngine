#pragma once

#include <set>
#include <string>
#include "imgui/imgui.h"

namespace ImGui
{
    const int fanColorEditFlags = ImGuiColorEditFlags_PickerHueWheel |
                                  ImGuiColorEditFlags_AlphaBar |
                                  ImGuiColorEditFlags_Float;

	void FanShowHelpMarker( const char* _desc );
	void FanToolTip( const char* _desc );
    bool FanSaveFileModal( const char* _popupName,
                           const std::set<std::string>& _extensionWhiteList,
                           std::string& _outCurrentPath );
    bool FanLoadFileModal( const char* _popupName,
                           const std::set<std::string>& _extensionWhiteList,
                           std::string& _path );
	void PushReadOnly();
	void PopReadOnly();

    namespace impl
    {
        bool FilesSelector( const std::set<std::string>& _extensionWhiteList, std::string& _path );
    }
}
