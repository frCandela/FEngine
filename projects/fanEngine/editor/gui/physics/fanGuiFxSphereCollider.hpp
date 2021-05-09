#pragma once

#include "engine/physics/fanFxSphereCollider.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    struct GuiFxSphereCollider
    {
        //====================================================================================================
        //====================================================================================================
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::SphereShape16;
            info.mGroup      = EngineGroups::ScenePhysics;
            info.onGui       = &OnGui;
            info.mEditorPath = "fixedPoint/";
            info.mEditorName = "fxSphereCollider";
            return info;
        }

        //========================================================================================================
        //========================================================================================================
        static void OnGui( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component )
        {
            FxSphereCollider& sphere = static_cast<FxSphereCollider&>( _component );
            ImGui::PushID( "fxSphereCollider" );
            ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 );
            {
                // radius
                if( ImGui::Button( "##radius" ) )
                {
                    sphere.mRadius = FIXED(0.5);
                    _world.GetComponent<SceneNode>(_entity).AddFlag( SceneNode::BoundsOutdated );
                    GuiFxRigidbody::TryUpdateInvInertiaTensorLocal(  _world, _entity );
                }
                ImGui::SameLine();
                float radius = sphere.mRadius.ToFloat();
                if( ImGui::DragFloat( "radius", &radius, 0.01f, -1000.f, 1000.f ) )
                {
                    sphere.mRadius = Fixed::FromFloat( radius );
                    _world.GetComponent<SceneNode>(_entity).AddFlag( SceneNode::BoundsOutdated );
                    GuiFxRigidbody::TryUpdateInvInertiaTensorLocal(  _world, _entity );
                }

                // offset
                if( ImGui::Button( "##offset" ) )
                {
                    sphere.mOffset = Vector3::sZero;
                    _world.GetComponent<SceneNode>( _entity ).AddFlag( SceneNode::BoundsOutdated );
                }
                ImGui::SameLine();
                glm::vec3 offset = Math::ToGLM( sphere.mOffset );
                if( ImGui::DragFloat3( "offset", &offset.x, 0.01f, -1000.f, 1000.f ) )
                {
                    sphere.mOffset = Math::ToFixed( offset );
                    _world.GetComponent<SceneNode>( _entity ).AddFlag( SceneNode::BoundsOutdated );
                }
            }
            ImGui::PopID();
        }
    };
}