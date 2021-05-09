#pragma once

#include "engine/physics/fanFxRigidbody.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"
#include "engine/physics/fanFxBoxCollider.hpp"
#include "engine/physics/fanFxSphereCollider.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiFxRigidbody
    {
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

        static void OnGui( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component )
        {
            FxRigidbody& rb = static_cast<FxRigidbody&>( _component );
            ImGui::PushID( "FxRigidbody" );
            ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 );
            {
                // Mass
                if( ImGui::Button( "##Mass" ) )
                {
                    rb.mInverseMass = 1;
                    TryUpdateInvInertiaTensorLocal( _world, _entity );
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
                    TryUpdateInvInertiaTensorLocal( _world, _entity );
                }

                // inertia tensor
                if( ImGui::Button( "##reload_tensor" ) )
                {
                    TryUpdateInvInertiaTensorLocal( _world, _entity );
                }
                ImGui::SameLine();
                ImGui::PushReadOnly();
                glm::vec3 invInertiaTensor = Vector3 { rb.mInverseInertiaTensorLocal.e11, rb.mInverseInertiaTensorLocal.e22, rb.mInverseInertiaTensorLocal.e33 }.ToGlm();
                ImGui::DragFloat3( "inverse inertia tensor", &invInertiaTensor.x, 1.f, -1000.f, 1000.f );
                ImGui::PopReadOnly();

                // Velocity
                if( ImGui::Button( "##Velocity" ) )
                {
                    rb.mVelocity = Vector3( 0, 0, 0 );
                }
                ImGui::SameLine();
                glm::vec3 velocity = rb.mVelocity.ToGlm();
                if( ImGui::DragFloat3( "velocity", &velocity[0], 1.f, -1000.f, 1000.f ) )
                {
                    rb.mVelocity = Vector3( velocity );
                    rb.SetSleeping( false );
                }

                // Rotation
                if( ImGui::Button( "##Rotation" ) )
                {
                    rb.mRotation = Vector3( 0, 0, 0 );
                }
                ImGui::SameLine();
                glm::vec3 rotation = rb.mRotation.ToGlm();
                if( ImGui::DragFloat3( "rotation", &rotation[0], 1.f, -1000.f, 1000.f ) )
                {
                    rb.mRotation = Vector3( rotation );
                    rb.SetSleeping( false );
                }

                // acceleration
                if( ImGui::Button( "##Acceleration" ) )
                {
                    rb.mAcceleration = Vector3( 0, 0, 0 );
                }
                ImGui::SameLine();
                glm::vec3 acceleration = rb.mAcceleration.ToGlm();
                if( ImGui::DragFloat3( "acceleration", &acceleration[0], 0.1f, -1000.f, 1000.f ) )
                {
                    rb.mAcceleration = Vector3( acceleration );
                }

                // sleep
                if( ImGui::Checkbox( "can sleep", &rb.mCanSleep ) )
                {
                    if( rb.mIsSleeping ){ rb.SetSleeping( false ); }
                }
                ImGui::PushReadOnly();
                ImGui::Checkbox( "sleeping", &rb.mIsSleeping );
                ImGui::SameLine();
                float motion = rb.mMotion.ToFloat();
                ImGui::DragFloat( "motion", &motion );
                ImGui::PopReadOnly();

                ImGui::PopItemWidth();
            }
            ImGui::PopID();
        }

        //==================================================================================================================================================================================================
        //==================================================================================================================================================================================================
        static void TryUpdateInvInertiaTensorLocal( EcsWorld& _world, EcsEntity _entity )
        {
            if( !_world.HasComponent<FxRigidbody>( _entity ) ){ return; }
            FxRigidbody& rb = _world.GetComponent<FxRigidbody>( _entity );

            if( rb.mInverseMass == 0 )
            {
                rb.mInverseInertiaTensorLocal = Matrix3::sZero;
            }
            else if( _world.HasComponent<FxSphereCollider>( _entity ) )
            {
                const FxSphereCollider& sphereCollider = _world.GetComponent<FxSphereCollider>( _entity );
                rb.mInverseInertiaTensorLocal = FxRigidbody::SphereInertiaTensor( rb.mInverseMass, sphereCollider.mRadius ).Inverse();
            }
            else if( _world.HasComponent<FxBoxCollider>( _entity ) )
            {
                const FxBoxCollider& boxCollider = _world.GetComponent<FxBoxCollider>( _entity );
                rb.mInverseInertiaTensorLocal = FxRigidbody::BoxInertiaTensor( rb.mInverseMass, boxCollider.mHalfExtents ).Inverse();
            }
            else
            {
                rb.mInverseInertiaTensorLocal = Matrix3::sZero;
            }
        }
    };
}