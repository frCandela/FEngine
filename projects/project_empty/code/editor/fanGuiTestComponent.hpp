#pragma once

#include "../game/components/fanTestComponent.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiTestComponent
    {
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::None16;
            info.mGroup      = EngineGroups::Game;
            info.onGui       = &GuiTestComponent::OnGui;
            info.mEditorName = "test component";
            info.mEditorPath = "game/test/";
            return info;
        }

        static void OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
        {
            TestComponent& testComponent = static_cast<TestComponent&>( _component );
            ImGui::DragInt( "test value", &testComponent.mValue );
        }
    };
}