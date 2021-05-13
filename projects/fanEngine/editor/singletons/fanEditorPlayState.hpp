#pragma once

#include "core/ecs/fanEcsSingleton.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct EditorPlayState : public EcsSingleton
    {
    ECS_SINGLETON( EditorPlayState )
        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _component );
        static void Save( const EcsSingleton& _component, Json& _json );
        static void Load( EcsSingleton& _component, const Json& _json );

        enum State
        {
            STOPPED, PLAYING, PAUSED
        };
        State mState;
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiEditorPlayState
    {
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mEditorName = "editor play state";
            info.mIcon       = ImGui::IconType::Play16;
            info.mGroup      = EngineGroups::Editor;
            return info;
        }
    };
}