#pragma once

#include "engine/physics/fanFxBoxCollider.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    struct GuiFxBoxCollider
    {
        //====================================================================================================
        //====================================================================================================
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::CubeShape16;
            info.mGroup      = EngineGroups::ScenePhysics;
            info.onGui       = &OnGui;
            info.mEditorPath = "fixedPoint/";
            info.mEditorName = "fxBoxCollider";
            return info;
        }

        //========================================================================================================
        //========================================================================================================
        static void OnGui( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component )
        {
            FxBoxCollider& box = static_cast<FxBoxCollider&>( _component );
            ImGui::PushID( "fxBoxCollider" );
            ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 );
            {
                // Half extents
                if( ImGui::Button( "##half_extents" ) )
                {
                    box.mHalfExtents = FIXED(0.5) * Vector3::sOne;
                    _world.GetComponent<SceneNode>(_entity).AddFlag( SceneNode::BoundsOutdated );
                    GuiFxRigidbody::TryUpdateInvInertiaTensorLocal(  _world, _entity );
                }
                ImGui::SameLine();
                glm::vec3 halfExtents = Math::ToGLM( box.mHalfExtents );
                if( ImGui::DragFloat3( "half extents", &halfExtents[0], 0.01f, -1000.f, 1000.f ) )
                {
                    box.mHalfExtents = Math::ToFixed( halfExtents );
                    _world.GetComponent<SceneNode>(_entity).AddFlag( SceneNode::BoundsOutdated );
                    GuiFxRigidbody::TryUpdateInvInertiaTensorLocal(  _world, _entity );
                }
            }
            ImGui::PopID();
        }
    };
}