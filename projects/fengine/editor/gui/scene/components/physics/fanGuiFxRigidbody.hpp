#pragma once

#include "engine/physics/fanFxRigidbody.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    struct GuiFxRigidbody
    {
        //====================================================================================================
        //====================================================================================================
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::Rigidbody16;
            info.mGroup      = EngineGroups::ScenePhysics;
            info.onGui       = &OnGui;
            info.mEditorPath = "fixedPoint/";
            info.mEditorName = "fxRigidbody";
            return info;
        }

        //========================================================================================================
        //========================================================================================================
        static void OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
        {
            FxRigidbody& rb = static_cast<FxRigidbody&>( _component );
            ImGui::PushID( "FxRigidbody" );
            ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 );
            {
                // Mass
                if( ImGui::Button( "##Mass" ) )
                {
                    rb.mInverseMass = 1;
                }
                ImGui::SameLine();
                float invMass = rb.mInverseMass.ToFloat();
                float mass    = ( invMass > 0.f ? 1.f / invMass : 0.f );
                if( ImGui::DragFloat( "mass", &mass, 2.f, 0.01f, 1000 ) )
                {
                    if( mass <= 0 )
                    {
                        rb.mInverseMass = 0;
                    }
                    else
                    {
                        rb.mInverseMass = 1 / Fixed::FromFloat( mass );
                    }
                }
                ImGui::PopItemWidth();

                // Velocity
                if( ImGui::Button( "##Velocity" ) )
                {
                    rb.mVelocity = Vector3( 0, 0, 0 );
                }
                ImGui::SameLine();
                glm::vec3 velocity = Math::ToGLM( rb.mVelocity );
                if( ImGui::DragFloat3( "velocity", &velocity[0], 1.f, -1000.f, 1000.f ) )
                {
                    rb.mVelocity = Math::ToFixed( velocity );
                }

                // Rotation
                if( ImGui::Button( "##Rotation" ) )
                {
                    rb.mRotation = Vector3( 0, 0, 0 );
                }
                ImGui::SameLine();
                glm::vec3 rotation = Math::ToGLM( rb.mRotation );
                if( ImGui::DragFloat3( "rotation", &rotation[0], 1.f, -1000.f, 1000.f ) )
                {
                    rb.mRotation = Math::ToFixed( rotation );
                }

                // Velocity
                if( ImGui::Button( "##Acceleration" ) )
                {
                    rb.mAcceleration = Vector3( 0, 0, 0 );
                }
                ImGui::SameLine();
                glm::vec3 acceleration = Math::ToGLM( rb.mAcceleration );
                if( ImGui::DragFloat3( "acceleration", &acceleration[0], 0.1f, -1000.f, 1000.f ) )
                {
                    rb.mAcceleration = Math::ToFixed( acceleration );
                }
            }
            ImGui::PopID();
        }
    };
}