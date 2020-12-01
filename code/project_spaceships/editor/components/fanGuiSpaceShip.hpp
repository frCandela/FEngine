#pragma once

#include "project_spaceships/game/components/fanSpaceShip.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    struct GuiSpaceShip
    {
        //====================================================================================================
        //====================================================================================================
        static GuiComponentInfo GetInfo()
        {
            GuiComponentInfo info;
            info.mIcon       = ImGui::IconType::Spaceship16;
            info.mGroup      = EngineGroups::Game;
            info.onGui       = &GuiSpaceShip::OnGui;
            info.mEditorPath = "game/player/";
            info.mEditorName = "spaceship";
            return info;
        }

        //========================================================================================================
        //========================================================================================================
        static void OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
        {
            SpaceShip& spaceship = static_cast<SpaceShip&>( _component );

            ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
            {
                ImGui::DragFloat( "reverse force",
                                  &spaceship.mForwardForces[SpaceShip::SpeedMode::REVERSE],
                                  1.f,
                                  0.f,
                                  100000.f );
                ImGui::DragFloat( "slow forward force",
                                  &spaceship.mForwardForces[SpaceShip::SpeedMode::SLOW],
                                  1.f,
                                  0.f,
                                  100000.f );
                ImGui::DragFloat( "normal forward force",
                                  &spaceship.mForwardForces[SpaceShip::SpeedMode::NORMAL],
                                  1.f,
                                  0.f,
                                  100000.f );
                ImGui::DragFloat( "fast forward force",
                                  &spaceship.mForwardForces[SpaceShip::SpeedMode::FAST],
                                  1.f,
                                  0.f,
                                  100000.f );
                ImGui::DragFloat( "lateral force", &spaceship.mLateralForce, 1.f, 0.f, 100000.f );
                ImGui::DragFloat( "active drag", &spaceship.mActiveDrag, 0.001f, 0.f, 1.f );
                ImGui::DragFloat( "passive drag", &spaceship.mPassiveDrag, 0.001f, 0.f, 1.f );
                ImGui::DragFloat( "energyConsumedPerUnitOfForce",
                                  &spaceship.mEnergyConsumedPerUnitOfForce,
                                  0.0001f,
                                  0.f,
                                  1.f );
                ImGui::DragFloat( "planet damage", &spaceship.mPlanetDamage );
                ImGui::DragFloat( "collision repulsion force", &spaceship.mCollisionRepulsionForce );

                ImGui::Spacing();

                ImGui::FanComponent( "fast forward particles L", spaceship.mFastForwardParticlesL );
                ImGui::FanComponent( "fast forward particles R", spaceship.mFastForwardParticlesR );
                ImGui::FanComponent( "slow forward particles L", spaceship.mSlowForwardParticlesL );
                ImGui::FanComponent( "slow forward particles R", spaceship.mSlowForwardParticlesR );
                ImGui::FanComponent( "reverse particles", spaceship.mReverseParticles );
                ImGui::FanComponent( "left particles", spaceship.mLeftParticles );
                ImGui::FanComponent( "right particles", spaceship.mRightParticles );

                ImGui::Spacing();

                ImGui::FanPrefab( "death fx", spaceship.mDeathFx );
            }
            ImGui::PopItemWidth();
        }
    };
}