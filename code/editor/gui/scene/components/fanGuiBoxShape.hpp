#pragma once

#include "scene/components/fanBoxShape.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    struct GuiBoxShape
    {
        //====================================================================================================
        //====================================================================================================
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::CubeShape16;
            info.mGroup      = EngineGroups::ScenePhysics;
            info.onGui       = &GuiBoxShape::OnGui;
            info.mEditorPath = "/";
            info.mEditorName       = "box_shape";
            return info;
        }

        //========================================================================================================
        //========================================================================================================
        static  void OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
        {
            BoxShape& boxShape = static_cast<BoxShape&>( _component );
            ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
            {
                btVector3 extent = boxShape.GetScaling();
                if( ImGui::DragFloat3( "scaling", &extent[0], 0.05f, 0.f ) )
                {
                    boxShape.SetScaling( extent );
                }
            }
            ImGui::PopItemWidth();
        }
    };
}