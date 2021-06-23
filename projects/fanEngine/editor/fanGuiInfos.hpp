#pragma once

#include <unordered_map>
#include "ecs/fanEcsSingleton.hpp"
#include "ecs/fanEcsComponent.hpp"
#include "editor/fanImguiIcons.hpp"
#include "editor/fanGuiInfos.hpp"

namespace fan
{
    class EcsWorld;

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiSingletonInfo
    {
        enum class Type
        {
            Default,    // drawn in the singleton window
            ToolWindow, // drawn as a tool window with an entry in the tools menu
            Other // has to be drawn manually (ex: main menu bar)
        };

        std::string     mEditorName;
        ImGui::IconType mIcon             = ImGui::IconType::None16; // editor icon
        EngineGroups    mGroup            = EngineGroups::None;      // editor groups are color coded
        Type            mType             = Type::Default;
        int             mImGuiWindowFlags = ImGuiWindowFlags_None;
        void ( * onGui )( EcsWorld&, EcsSingleton& ) = nullptr;
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiComponentInfo
    {
        std::string     mEditorName;
        ImGui::IconType mIcon  = ImGui::IconType::None16;    // editor icon
        EngineGroups    mGroup = EngineGroups::None;            // editor groups are color coded
        const char* mEditorPath = "";                    // editor path for the addComponent menu

        void ( * onGui )( EcsWorld&, EcsEntity, EcsComponent& ) = nullptr;
    };
}