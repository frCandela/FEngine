#pragma once

#include "engine/physics/fanFxTransform.hpp"
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
            info.mEditorPath = "fixedPoint/";
            info.mEditorName = "fxTransform";
            return info;
        }

        //========================================================================================================
        //========================================================================================================
        static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component )
        {
            FxTransform& transform = static_cast<FxTransform&>( _component );
            ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 );
            {
                ImGui::PushID( "fixed_transform" );
                SceneNode& sceneNode = _world.GetComponent<SceneNode>( _entityID );

                // Position
                if( ImGui::Button( "##TransPos" ) )
                {
                    transform.mPosition = Vector3::sZero;
                    sceneNode.AddFlag( SceneNode::BoundsOutdated );
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
                    sceneNode.AddFlag( SceneNode::BoundsOutdated );
                }


                // rotation
                if( ImGui::Button( "##TransRot" ) )
                {
                    transform.mRotation = Quaternion::sIdentity;
                    sceneNode.AddFlag( SceneNode::BoundsOutdated );
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
                    const Vector3 axis = newRotation - oldRotation;
                    const Fixed angle = axis.Magnitude();
                    if( angle > Fixed::FromFloat(speed) )
                    {
                        const Quaternion rotationChange = Quaternion::AngleAxis(angle, axis );
                        transform.mRotation = rotationChange * transform.mRotation;
                        sceneNode.AddFlag( SceneNode::BoundsOutdated );
                    }
                }

                ImGui::PopID();
            }
            ImGui::PopItemWidth();
        }
    };
}