#pragma once

#include "engine/components/fanTransform.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    struct GuiTransform
    {
        //====================================================================================================
        //====================================================================================================
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::Transform16;
            info.mGroup      = EngineGroups::Scene;
            info.onGui       = &GuiTransform::OnGui;
            info.mEditorPath = "/";
            info.mEditorName = "transform";
            return info;
        }

        //========================================================================================================
        //========================================================================================================
        static void OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
        {
            Transform& transform = static_cast<Transform&>( _component );
            ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 );
            {
                // Position
                if( ImGui::Button( "##TransPos" ) )
                {
                    transform.SetPosition( btVector3( 0, 0, 0 ) );
                }
                ImGui::SameLine();
                btVector3 position = transform.GetPosition();
                if( ImGui::DragFloat3( "position", &position[0], 0.1f ) )
                {
                    transform.SetPosition( position );
                }

                // rotation
                if( ImGui::Button( "##TransRot" ) )
                {
                    transform.SetRotationQuat( btQuaternion::getIdentity() );
                }
                ImGui::SameLine();
                btVector3 rotation = transform.GetRotationEuler();
                if( ImGui::DragFloat3( "rotation", &rotation[0], 0.1f ) )
                {
                    transform.SetRotationEuler( rotation );
                }

                // Scale
                if( ImGui::Button( "##TransScale" ) )
                {
                    transform.SetScale( btVector3( 1, 1, 1 ) );
                }
                ImGui::SameLine();
                ImGui::DragFloat3( "scale", &transform.mScale[0], 0.1f );
            }
            ImGui::PopItemWidth();
        }
    };
}