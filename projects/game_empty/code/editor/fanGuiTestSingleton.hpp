#pragma once

#include "../game/singletons/fanTestSingleton.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiTestSingleton
    {
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mIcon       = ImGui::None16;
            info.mGroup      = EngineGroups::Game;
            info.onGui       = &GuiTestSingleton::OnGui;
            info.mEditorName = "test singleton";
            return info;
        }

        static void OnGui( EcsWorld& /*_world*/, EcsSingleton& _component )
        {
            TestSingleton& testSingleton = static_cast<TestSingleton&>( _component );
            ImGui::DragFloat( "test value ", &testSingleton.mValue );
        }
    };
}