#pragma once

#include <map>
#include <set>
#include "editor/fanImguiIcons.hpp"
#include "core/fanString.hpp"

namespace fan
{
    struct Resource;

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct EditorResourceInfo
    {
        ImGui::IconType mIcon;
        String<32>      mDefaultPath;
        String<32>      mResourceName;
        const std::set<std::string>* mExtensions = nullptr;
        void ( * mDrawTooltip )( Resource* _resource ) = nullptr;

        static const std::map<uint32_t, EditorResourceInfo> sResourceInfos;
    };
}