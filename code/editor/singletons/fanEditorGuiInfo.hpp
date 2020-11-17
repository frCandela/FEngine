#pragma once

#include <unordered_map>
#include "core/ecs/fanEcsSingleton.hpp"
#include "core/ecs/fanEcsComponent.hpp"
#include "editor/fanImguiIcons.hpp"

namespace fan
{
    class EcsWorld;

    //====================================================
    //====================================================
    struct GuiSingletonInfo
    {
        std::string            mEditorName;
        ImGui::IconType        mIcon  = ImGui::IconType::None16; // editor icon
        EngineGroups           mGroup = EngineGroups::None;      // editor groups are color coded

        void ( *onGui ) ( EcsWorld&, EcsSingleton& ) = nullptr;
    };


    //====================================================
    //====================================================
    struct GuiComponentInfo
    {
        std::string            mEditorName;
        ImGui::IconType        mIcon  = ImGui::IconType::None16;	// editor icon
        EngineGroups           mGroup = EngineGroups::None;	        // editor groups are color coded
        const char*		       mEditorPath = "";				    // editor path for the addComponent menu

        void ( *onGui )( EcsWorld&, EcsEntity, EcsComponent& ) = nullptr;
    };

    //========================================================================================================
    // Holds editor data specific to every type of component & singleton
    //========================================================================================================
    struct EditorGuiInfo : public EcsSingleton
    {
        ECS_SINGLETON( EditorGuiInfo )

        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _component );

        const GuiComponentInfo&	GetComponentInfo( const uint32_t _type ) const;
        const GuiSingletonInfo& GetSingletonInfo( const uint32_t _type ) const;

        std::unordered_map< uint32_t, GuiSingletonInfo >  mSingletonInfos;
        std::unordered_map< uint32_t, GuiComponentInfo >  mComponentInfos;
    };
}