#pragma once

#include "engine/components/fanCamera.hpp"
#include "editor/fanGuiInfos.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiCamera
    {
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::Camera16;
            info.mGroup      = EngineGroups::Scene;
            info.onGui       = &GuiCamera::OnGui;
            info.mEditorPath = "/";
            info.mEditorName = "Camera";
            return info;
        }

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
                        camera.mFov = 110;
                    }
                    ImGui::SameLine();
                    float fov = camera.mFov.ToFloat();
                    if( ImGui::DragFloat( "fov", &fov, 1.f, 1.f, 179.f ) ){ camera.mFov = Fixed::FromFloat( fov ); }
                }
                else if( camera.mType == Camera::Type::Orthogonal )
                {
                    // fov
                    if( ImGui::Button( "##size" ) )
                    {
                        camera.mOrthoSize = 10;
                    }
                    ImGui::SameLine();

                    float orthoSize = camera.mOrthoSize.ToFloat();
                    if( ImGui::DragFloat( "orthoSize", &orthoSize, 1.f, 0.f, 100.f ) ){ camera.mOrthoSize = Fixed::FromFloat( orthoSize ); }
                }

                // nearDistance
                if( ImGui::Button( "##nearDistance" ) )
                {
                    camera.mNearDistance = FIXED( 0.01 );
                }
                ImGui::SameLine();

                float nearDistance = camera.mNearDistance.ToFloat();
                if( ImGui::DragFloat( "near distance", &nearDistance, 0.001f, 0.01f, 10.f ) ){ camera.mNearDistance = Fixed::FromFloat( nearDistance ); }

                // far distance
                if( ImGui::Button( "##fardistance" ) )
                {
                    camera.mFarDistance = 1000;
                }
                ImGui::SameLine();

                float farDistance = camera.mFarDistance.ToFloat();
                if( ImGui::DragFloat( "far distance", &farDistance, 10.f, 0.05f, 10000.f ) ){ camera.mFarDistance = Fixed::FromFloat( farDistance ); }
            }
            ImGui::PopItemWidth();
        }
    };
}