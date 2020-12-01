#pragma once

#include "project_spaceships/game/singletons/fanSunLight.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    struct GuiSunLight
    {
        //====================================================================================================
        //====================================================================================================
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mIcon  = ImGui::Sun16;
            info.mGroup = EngineGroups::Game;
            info.onGui  = &GuiSunLight::OnGui;
            info.mEditorName  = "sun light";
            return info;
        }

        //========================================================================================================
        //========================================================================================================
        static void OnGui( EcsWorld&, EcsSingleton& _component )
        {
            SunLight& sunLight = static_cast<SunLight&>( _component );
            ImGui::DragFloat( "sub angle", &sunLight.mSubAngle, 1.f, 0.f, 90.f );
            ImGui::DragFloat( "radius", &sunLight.mRadius, 1.f, 1.f, 1000.f );
        }
    };
}