#pragma once

#include "engine/components/fanScale.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiFxScale
    {
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::Transform16;
            info.mGroup      = EngineGroups::Scene;
            info.onGui       = &GuiFxScale::OnGui;
            info.mEditorPath = "/";
            info.mEditorName = "fxScale";
            return info;
        }

        static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component )
        {
            Scale& scale = static_cast<Scale&>( _component );
            ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 );
            {
                ImGui::PushID( "fixed_scale" );
                SceneNode& sceneNode = _world.GetComponent<SceneNode>( _entityID );

                // Scale
                if( ImGui::Button( "##TransScale" ) )
                {
                    scale.mScale = Vector3::sOne;
                    sceneNode.AddFlag( SceneNode::BoundsOutdated );
                }
                ImGui::SameLine();
                glm::vec3 glmScale =scale.mScale.ToGlm();
                if( ImGui::DragFloat3( "scale", &glmScale[0], 0.1f ) )
                {
                    Vector3 newScale = { Fixed::FromFloat( glmScale.x ), Fixed::FromFloat( glmScale.y ), Fixed::FromFloat( glmScale.z ) };
                    scale.mScale = {
                            Fixed::IsFuzzyZero( newScale.x - scale.mScale.x ) ? scale.mScale.x : newScale.x,
                            Fixed::IsFuzzyZero( newScale.y - scale.mScale.y ) ? scale.mScale.y : newScale.y,
                            Fixed::IsFuzzyZero( newScale.z - scale.mScale.z ) ? scale.mScale.z : newScale.z
                    };
                    sceneNode.AddFlag( SceneNode::BoundsOutdated );
                }

                ImGui::PopID();
            }
            ImGui::PopItemWidth();
        }
    };
}