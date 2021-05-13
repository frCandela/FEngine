#pragma once

#include <unordered_map>
#include "core/ecs/fanEcsSingleton.hpp"
#include "core/ecs/fanEcsComponent.hpp"
#include "editor/fanImguiIcons.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

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

    //==================================================================================================================================================================================================
    // Holds editor data specific to every type of component & singleton
    //==================================================================================================================================================================================================
    struct EditorGuiInfo : public EcsSingleton
    {
    ECS_SINGLETON( EditorGuiInfo )

        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _singleton );

        GuiComponentInfo& GetComponentInfo( const uint32_t _type ) { return mComponentInfos.at( _type ); }
        GuiSingletonInfo& GetSingletonInfo( const uint32_t _type ) { return mSingletonInfos.at( _type ); }
        const GuiComponentInfo& GetComponentInfo( const uint32_t _type ) const { return mComponentInfos.at( _type ); }
        const GuiSingletonInfo& GetSingletonInfo( const uint32_t _type ) const { return mSingletonInfos.at( _type ); }

        std::unordered_map<uint32_t, GuiSingletonInfo> mSingletonInfos;
        std::unordered_map<uint32_t, GuiComponentInfo> mComponentInfos;
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiEditorGuiInfo
    {
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mEditorName = "editor gui info";
            info.mIcon       = ImGui::None16;
            info.mGroup      = EngineGroups::Editor;
            info.onGui       = &GuiEditorGuiInfo::OnGui;
            return info;
        }

        static void OnGui( EcsWorld&, EcsSingleton& _singleton )
        {
            EditorGuiInfo& gui = static_cast<EditorGuiInfo&>( _singleton );
            (void)gui;
        }
    };
}