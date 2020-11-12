#pragma once

#include "scene/components/fanSphereShape.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    struct GuiSphereShape
    {
        //====================================================================================================
        //====================================================================================================
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::SphereShape16;
            info.mGroup      = EngineGroups::ScenePhysics;
            info.onGui       = &GuiSphereShape::OnGui;
            info.mEditorPath = "/";
            info.mEditorName = "sphere shape";
            return info;
        }

        //========================================================================================================
        //========================================================================================================
        static void OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
        {
            SphereShape& sphereShape = static_cast<SphereShape&>( _component );

            ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
            {
                float radius = sphereShape.GetRadius();
                if( ImGui::DragFloat( "radius##sphshapradius", &radius, 0.1f, 0.f ) )
                {
                    sphereShape.SetRadius( radius );
                }
            }
            ImGui::PopItemWidth();
        }
    };
}