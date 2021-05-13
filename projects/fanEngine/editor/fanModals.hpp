#pragma once

#include <set>
#include <string>
#include "imgui/imgui.h"
#include "core/math/fanFixedPoint.hpp"

namespace ImGui
{
    const int fanColorEditFlags = ImGuiColorEditFlags_PickerHueWheel |
                                  ImGuiColorEditFlags_AlphaBar |
                                  ImGuiColorEditFlags_Float;

    bool DragFixed( const char* label, fan::Fixed* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0 );
    bool DragFixed2( const char* label, fan::Fixed v[2], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0 );
    bool DragFixed3( const char* label, fan::Fixed v[3], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0 );

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
