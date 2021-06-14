#pragma once

#include "engine/physics/fanBoxCollider.hpp"
#include "editor/fanGuiInfos.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiBoxCollider
    {
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::CubeShape16;
            info.mGroup      = EngineGroups::ScenePhysics;
            info.onGui       = &OnGui;
            info.mEditorPath = "physics/";
            info.mEditorName = "BoxCollider";
            return info;
        }

        static void OnGui( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component )
        {
            BoxCollider& box = static_cast<BoxCollider&>( _component );
            ImGui::PushID( "boxCollider" );
            ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 );
            {
                // Half extents
                if( ImGui::Button( "##half_extents" ) )
                {
                    box.mHalfExtents = FIXED( 0.5 ) * Vector3::sOne;
                    _world.GetComponent<SceneNode>( _entity ).AddFlag( SceneNode::BoundsOutdated );
                    GuiRigidbody::TryUpdateInvInertiaTensorLocal( _world, _entity );
                }
                ImGui::SameLine();
                glm::vec3 halfExtents = box.mHalfExtents.ToGlm();
                if( ImGui::DragFloat3( "half extents", &halfExtents[0], 0.01f, -1000.f, 1000.f ) )
                {
                    box.mHalfExtents = Vector3( halfExtents );
                    _world.GetComponent<SceneNode>( _entity ).AddFlag( SceneNode::BoundsOutdated );
                    GuiRigidbody::TryUpdateInvInertiaTensorLocal( _world, _entity );
                }
            }
            ImGui::PopID();
        }
    };
}