#pragma once

#include "engine/components/fanFxTransform.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
    struct GuiFxTransform
    {
        //====================================================================================================
        //====================================================================================================
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::Transform16;
            info.mGroup      = EngineGroups::Scene;
            info.onGui       = &GuiFxTransform::OnGui;
            info.mEditorPath = "/";
            info.mEditorName = "fxTransform";
            return info;
        }

        //========================================================================================================
        //========================================================================================================
        static void OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
        {
            FxTransform& transform = static_cast<FxTransform&>( _component );
            ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 );
            {
                ImGui::PushID( "fixed_transform" );

                // Position
                if( ImGui::Button( "##TransPos" ) )
                {
                    transform.mPosition = Vector3::sZero;
                }
                ImGui::SameLine();
                glm::vec3 position = Math::ToGLM( transform.mPosition );
                if( ImGui::DragFloat3( "position", &position[0], 0.1f ) )
                {
                    Vector3 newPosition = { Fixed::FromFloat( position.x ), Fixed::FromFloat( position.y ), Fixed::FromFloat( position.z ) };
                    transform.mPosition = {
                            Fixed::IsFuzzyZero( newPosition.x - transform.mPosition.x ) ? transform.mPosition.x : newPosition.x,
                            Fixed::IsFuzzyZero( newPosition.y - transform.mPosition.y ) ? transform.mPosition.y : newPosition.y,
                            Fixed::IsFuzzyZero( newPosition.z - transform.mPosition.z ) ? transform.mPosition.z : newPosition.z
                    };
                }


                // rotation
                if( ImGui::Button( "##TransRot" ) )
                {
                    transform.mRotation = Quaternion::sIdentity;
                }
                ImGui::SameLine();
                Vector3   oldRotation = transform.mRotation.Euler();
                glm::vec3 rotation    = Math::ToGLM( oldRotation );
                rotation = {
                        std::abs( rotation.x ) < 0.01 ? 0.f : rotation.x,
                        std::abs( rotation.y ) < 0.01 ? 0.f : rotation.y,
                        std::abs( rotation.z ) < 0.01 ? 0.f : rotation.z
                };
                const float speed = 0.1f;
                if( ImGui::DragFloat3( "rotation", &rotation[0], speed ) )
                {
                    Vector3 newRotation = { Fixed::FromFloat( rotation.x ), Fixed::FromFloat( rotation.y ), Fixed::FromFloat( rotation.z ) };
                    if( ( newRotation - oldRotation ).Magnitude() > Fixed::FromFloat(speed) )
                    {
                        transform.mRotation = Quaternion::Euler( newRotation );
                    }
                }

                // Scale
                if( ImGui::Button( "##TransScale" ) )
                {
                    transform.mScale = Vector3::sOne;
                }
                ImGui::SameLine();
                glm::vec3 scale = Math::ToGLM( transform.mScale );
                if( ImGui::DragFloat3( "scale", &scale[0], 0.1f ) )
                {
                    Vector3 newScale = { Fixed::FromFloat( scale.x ), Fixed::FromFloat( scale.y ), Fixed::FromFloat( scale.z ) };
                    transform.mScale = {
                            Fixed::IsFuzzyZero( newScale.x - transform.mScale.x ) ? transform.mScale.x : newScale.x,
                            Fixed::IsFuzzyZero( newScale.y - transform.mScale.y ) ? transform.mScale.y : newScale.y,
                            Fixed::IsFuzzyZero( newScale.z - transform.mScale.z ) ? transform.mScale.z : newScale.z
                    };
                }

                ImGui::PopID();
            }
            ImGui::PopItemWidth();
        }
    };
}