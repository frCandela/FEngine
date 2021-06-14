#pragma once

#include "engine/physics/fanTransform.hpp"
#include "editor/fanGuiInfos.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiTransform
    {
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::Transform16;
            info.mGroup      = EngineGroups::Scene;
            info.onGui       = &GuiTransform::OnGui;
            info.mEditorPath = "/";
            info.mEditorName = "Transform";
            return info;
        }

        static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component )
        {
            Transform& transform = static_cast<Transform&>( _component );
            ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 );
            {
                ImGui::PushID( "transform" );
                SceneNode& sceneNode = _world.GetComponent<SceneNode>( _entityID );

                // Position
                if( ImGui::Button( "##TransPos" ) )
                {
                    transform.mPosition = Vector3::sZero;
                    sceneNode.AddFlag( SceneNode::BoundsOutdated );
                }
                ImGui::SameLine();
                glm::vec3 position = transform.mPosition.ToGlm();
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
                glm::vec3 rotation    = oldRotation.ToGlm();
                rotation = {
                        std::abs( rotation.x ) < 0.01 ? 0.f : rotation.x,
                        std::abs( rotation.y ) < 0.01 ? 0.f : rotation.y,
                        std::abs( rotation.z ) < 0.01 ? 0.f : rotation.z
                };
                const float speed = 0.1f;
                if( ImGui::DragFloat3( "rotation", &rotation[0], speed ) )
                {
                    Vector3       newRotation = { Fixed::FromFloat( rotation.x ), Fixed::FromFloat( rotation.y ), Fixed::FromFloat( rotation.z ) };
                    const Vector3 axis        = newRotation - oldRotation;
                    const Fixed   angle       = axis.Magnitude();
                    if( angle > Fixed::FromFloat( speed ) )
                    {
                        const Quaternion rotationChange = Quaternion::AngleAxis( angle, axis );
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