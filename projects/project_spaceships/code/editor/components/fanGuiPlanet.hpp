#pragma once

#include "game/components/fanPlanet.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"
#include "engine/singletons/fanRenderDebug.hpp"

namespace fan
{
    struct GuiPlanet
    {
        //====================================================================================================
        //====================================================================================================
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::Planet16;
            info.mGroup      = EngineGroups::Game;
            info.onGui       = &GuiPlanet::OnGui;
            info.mEditorPath = "game/";
            info.mEditorName = "planet";
            return info;
        }

        //========================================================================================================
        //========================================================================================================
        static void OnGui( EcsWorld& _world, EcsEntity /*_entityID*/, EcsComponent& _component )
        {
            Planet& planet = static_cast<Planet&>( _component );
            ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
            {
                ImGui::DragFloat( "radius", &planet.mRadius, 0.1f, 0.f, 100.f );
                ImGui::DragFloat( "speed", &planet.mSpeed, 0.1f, 0.f, 10.f );
                ImGui::DragFloat( "phase", &planet.mPhase, SIMD_PI / 3, 0.f, 2 * SIMD_PI );
            }
            RenderDebug& renderDebug = _world.GetSingleton<RenderDebug>();
            renderDebug.DebugCircle( btVector3_Zero, planet.mRadius, btVector3_Up, 32, Color::sCyan );
        }
    };
}