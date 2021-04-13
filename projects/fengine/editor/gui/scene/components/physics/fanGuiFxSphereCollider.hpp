#pragma once

#include "engine/physics/fanFxSphereCollider.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    struct GuiFxSphereCollider
    {
        //====================================================================================================
        //====================================================================================================
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::SphereShape16;
            info.mGroup      = EngineGroups::ScenePhysics;
            info.onGui       = &OnGui;
            info.mEditorPath = "fixedPoint/";
            info.mEditorName = "fxSphereCollider";
            return info;
        }

        //========================================================================================================
        //========================================================================================================
        static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component )
        {
            FxSphereCollider& sphere = static_cast<FxSphereCollider&>( _component );
            ImGui::PushID( "fxSphereCollider" );
            ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 );
            {
                // Velocity
                if( ImGui::Button( "##radius" ) )
                {
                    sphere.mRadius = FIXED(0.5);
                    _world.GetComponent<SceneNode>(_entityID).AddFlag( SceneNode::BoundsOutdated );
                }
                ImGui::SameLine();
                float radius = sphere.mRadius.ToFloat();
                if( ImGui::DragFloat( "radius", &radius, 0.01f, -1000.f, 1000.f ) )
                {
                    sphere.mRadius = Fixed::FromFloat( radius );
                    _world.GetComponent<SceneNode>(_entityID).AddFlag( SceneNode::BoundsOutdated );
                }
            }
            ImGui::PopID();
        }
    };
}