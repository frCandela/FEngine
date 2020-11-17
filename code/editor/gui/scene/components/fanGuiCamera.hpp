#pragma once

#include "engine/components/fanCamera.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    struct GuiCamera
    {
        //====================================================================================================
        //====================================================================================================
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::Camera16;
            info.mGroup      = EngineGroups::Scene;
            info.onGui       = &GuiCamera::OnGui;
            info.mEditorPath = "/";
            info.mEditorName       = "camera";
            return info;
        }

        //========================================================================================================
        //========================================================================================================
        static void OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
        {
            Camera& camera = static_cast<Camera&>( _component );

            ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 );
            {

                int item = static_cast<int>( camera.mType );
                if( ImGui::Combo( "type", &item, "perspective\0orthogonal\0" ) )
                {
                    camera.mType = Camera::Type( item );
                }

                if( camera.mType == Camera::Type::Perspective )
                {
                    // fov
                    if( ImGui::Button( "##fov" ) )
                    {
                        camera.mFov = 110.f;
                    }
                    ImGui::SameLine();
                    ImGui::DragFloat( "fov", &camera.mFov, 1.f, 1.f, 179.f );
                }
                else if( camera.mType == Camera::Type::Orthogonal )
                {
                    // fov
                    if( ImGui::Button( "##size" ) )
                    {
                        camera.mOrthoSize = 10.f;
                    }
                    ImGui::SameLine();
                    ImGui::DragFloat( "size", &camera.mOrthoSize, 1.f, 0.f, 100.f );
                }

                // nearDistance
                if( ImGui::Button( "##nearDistance" ) )
                {
                    camera.mNearDistance = 0.01f;
                }
                ImGui::SameLine();
                ImGui::DragFloat( "near distance", &camera.mNearDistance, 0.001f, 0.01f, 10.f );


                // far distance
                if( ImGui::Button( "##fardistance" ) )
                {
                    camera.mFarDistance = 1000.f;
                }
                ImGui::SameLine();
                ImGui::DragFloat( "far distance", &camera.mFarDistance, 10.f, 0.05f, 10000.f );
            }
            ImGui::PopItemWidth();
        }
    };
}