#pragma once

#include "engine/components/fanRigidbody.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    struct GuiRigidbody
    {
        //====================================================================================================
        //====================================================================================================
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::Rigidbody16;
            info.mGroup      = EngineGroups::ScenePhysics;
            info.onGui       = &GuiRigidbody::OnGui;
            info.mEditorPath = "/";
            info.mEditorName = "rigidbody";
            return info;
        }

        //========================================================================================================
        //========================================================================================================
        static void OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
        {
            Rigidbody& rb = static_cast<Rigidbody&>( _component );

            enum ComboState
            {
                DYNAMIC = 0, KINEMATIC = 1, STATIC = 2
            };

            int state = rb.GetMass() > 0 ? DYNAMIC : ( rb.IsKinematic() ? KINEMATIC : STATIC );
            if( ImGui::Combo( "state", &state, "dynamic\0kinematic\0static\0" ) )
            {
                switch( state )
                {
                    case DYNAMIC:
                        if( rb.GetMass() <= 0 ){ rb.SetMass( 1.f ); }
                        break;
                    case KINEMATIC:
                        rb.SetKinematic();
                        break;
                    case STATIC:
                        rb.SetStatic();
                        break;
                    default:
                        fanAssert( false );
                        break;
                }
            }

            // Active
            bool isActive = rb.IsActive();
            if( ImGui::Checkbox( "active ", &isActive ) )
            {
                if( isActive )
                {
                    rb.Activate();
                }
            }
            ImGui::SameLine();

            // Deactivation
            bool enableDesactivation = rb.IsDeactivationEnabled();
            if( ImGui::Checkbox( "enable deactivation ", &enableDesactivation ) )
            {
                rb.EnableDeactivation( enableDesactivation );
            }

            ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 );
            {
                // Mass
                if( ImGui::Button( "##Mass" ) )
                {
                    rb.SetMass( 0.f );
                }
                ImGui::SameLine();
                float invMass = rb.mRigidbody->getInvMass();
                float mass    = ( invMass > 0.f ? 1.f / invMass : 0.f );
                if( ImGui::DragFloat( "mass", &mass, 1.f, 0.f, 1000.f ) )
                {
                    rb.SetMass( mass );
                }

                // Velocity
                if( ImGui::Button( "##Velocity" ) )
                {
                    rb.SetVelocity( btVector3( 0, 0, 0 ) );
                }
                ImGui::SameLine();
                btVector3 velocity = rb.GetVelocity();
                if( ImGui::DragFloat3( "velocity", &velocity[0], 1.f, -1000.f, 1000.f ) )
                {
                    rb.SetVelocity( velocity );
                }

                // Angular velocity
                if( ImGui::Button( "##AngularVelocity" ) )
                {
                    rb.SetAngularVelocity( btVector3( 0, 0, 0 ) );
                }
                ImGui::SameLine();
                btVector3 angularVelocity = rb.GetAngularVelocity();
                if( ImGui::DragFloat3( "angular velocity", &angularVelocity[0], 1.f, -1000.f, 1000.f ) )
                {
                    rb.SetAngularVelocity( angularVelocity );
                }
            }
            ImGui::PopItemWidth();
        }
    };
}