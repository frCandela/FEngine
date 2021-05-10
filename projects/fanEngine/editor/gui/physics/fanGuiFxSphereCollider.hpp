#pragma once

#include "engine/physics/fanSphereCollider.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiFxSphereCollider
    {
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::SphereShape16;
            info.mGroup      = EngineGroups::ScenePhysics;
            info.onGui       = &OnGui;
            info.mEditorPath = "physics/";
            info.mEditorName = "fxSphereCollider";
            return info;
        }

        static void OnGui( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component )
        {
            SphereCollider& sphere = static_cast<SphereCollider&>( _component );
            ImGui::PushID( "fxSphereCollider" );
            ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 );
            {
                // radius
                if( ImGui::Button( "##radius" ) )
                {
                    sphere.mRadius = FIXED( 0.5 );
                    _world.GetComponent<SceneNode>( _entity ).AddFlag( SceneNode::BoundsOutdated );
                    GuiFxRigidbody::TryUpdateInvInertiaTensorLocal( _world, _entity );
                }
                ImGui::SameLine();
                float radius = sphere.mRadius.ToFloat();
                if( ImGui::DragFloat( "radius", &radius, 0.01f, -1000.f, 1000.f ) )
                {
                    sphere.mRadius = Fixed::FromFloat( radius );
                    _world.GetComponent<SceneNode>( _entity ).AddFlag( SceneNode::BoundsOutdated );
                    GuiFxRigidbody::TryUpdateInvInertiaTensorLocal( _world, _entity );
                }

                // offset
                if( ImGui::Button( "##offset" ) )
                {
                    sphere.mOffset = Vector3::sZero;
                    _world.GetComponent<SceneNode>( _entity ).AddFlag( SceneNode::BoundsOutdated );
                }
                ImGui::SameLine();
                glm::vec3 offset = sphere.mOffset.ToGlm();
                if( ImGui::DragFloat3( "offset", &offset.x, 0.01f, -1000.f, 1000.f ) )
                {
                    sphere.mOffset = Vector3( offset );
                    _world.GetComponent<SceneNode>( _entity ).AddFlag( SceneNode::BoundsOutdated );
                }
            }
            ImGui::PopID();
        }
    };
}