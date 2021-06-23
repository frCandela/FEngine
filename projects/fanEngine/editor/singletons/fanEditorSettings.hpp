#pragma once

#include <unordered_map>
#include "core/ecs/fanEcsSingleton.hpp"
#include "fanJson.hpp"
#include "fanDisableWarnings.hpp"
#include "fanGlm.hpp"
#include "editor/fanGuiInfos.hpp"
#include "editor/fanGroupsColors.hpp"

namespace fan
{
    class Color;

    //==================================================================================================================================================================================================
    // only one instance in the editor
    //==================================================================================================================================================================================================
    struct EditorSettingsData
    {
        std::unordered_map<uint32_t, bool> mToolWindowsVisibility;
        GroupsColors                       mGroupsColors;
        Json                               mJson;

        bool IsWindowVisible( const uint32_t _singletonType ) { return mToolWindowsVisibility[_singletonType]; }

        static void LoadSettingsFromJson( EditorSettingsData& _settings );
        static void SaveSettingsToJson( EditorSettingsData& _settings );
        static void SaveJsonToDisk( Json& _json );
        static void LoadJsonFromDisk( Json& _json );

        static void SaveWindowSizeAndPosition( Json& _json, const glm::ivec2& _size, const glm::ivec2& _position );
        static bool LoadWindowSizeAndPosition( const Json& _json, glm::ivec2& _outSize, glm::ivec2& _outPosition );
    };

    //==================================================================================================================================================================================================
    // holds a reference to global editor settings
    //==================================================================================================================================================================================================
    struct EditorSettings : EcsSingleton
    {
    ECS_SINGLETON( EditorSettings )
        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _component );

        GuiComponentInfo& GetComponentInfo( const uint32_t _type ) { return mComponentInfos.at( _type ); }
        GuiSingletonInfo& GetSingletonInfo( const uint32_t _type ) { return mSingletonInfos.at( _type ); }
        const GuiComponentInfo& GetComponentInfo( const uint32_t _type ) const { return mComponentInfos.at( _type ); }
        const GuiSingletonInfo& GetSingletonInfo( const uint32_t _type ) const { return mSingletonInfos.at( _type ); }
        static void InitSingletonInfos(std::unordered_map<uint32_t, GuiSingletonInfo>& _singletonInfos);
        static void InitComponentInfos(std::unordered_map<uint32_t, GuiComponentInfo>& _componentInfos);

        EditorSettingsData* mData;
        std::unordered_map<uint32_t, GuiSingletonInfo> mSingletonInfos;
        std::unordered_map<uint32_t, GuiComponentInfo> mComponentInfos;
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiEditorSettings
    {
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mEditorName = "editor settings";
            info.mIcon       = ImGui::None16;
            info.mGroup      = EngineGroups::Editor;
            return info;
        }
    };
}