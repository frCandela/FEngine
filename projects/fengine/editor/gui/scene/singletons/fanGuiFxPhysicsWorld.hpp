#pragma once

#include "engine/physics/fanFxPhysicsWorld.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"
#include "core/math/fanMathUtils.hpp"

namespace fan
{
    struct GuiFxPhysicsWorld
    {
        //====================================================================================================
        //====================================================================================================
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mIcon       = ImGui::Rigidbody16;
            info.mGroup      = EngineGroups::ScenePhysics;
            info.onGui       = &GuiFxPhysicsWorld::OnGui;
            info.mEditorName = "fxPhysicsWorld";
            return info;
        }

        //========================================================================================================
        //========================================================================================================
        static void OnGui( EcsWorld&, EcsSingleton& _component )
        {
            FxPhysicsWorld& physicsWorld = static_cast<FxPhysicsWorld&>( _component );


            // gravity
            glm::vec3 gravity = Math::ToGLM( physicsWorld.mGravity );
            if( ImGui::DragFloat3( "gravity", &gravity.x ) )
            {
                physicsWorld.mGravity = Math::ToFixed( gravity );
            }

            // damping
            float damping = physicsWorld.mLinearDamping.ToFloat();
            if( ImGui::DragFloat( "damping", &damping ) )
            {
                physicsWorld.mLinearDamping = Fixed::FromFloat( damping );
            }

            // anglar damping
            float angularDamping = physicsWorld.mAngularDamping.ToFloat();
            if( ImGui::DragFloat( "angular damping", &angularDamping ) )
            {
                physicsWorld.mAngularDamping = Fixed::FromFloat( angularDamping );
            }

            // restitution
            float restitution = physicsWorld.mContactSolver.mRestitution.ToFloat();
            if( ImGui::DragFloat( "restitution", &restitution, 0.01f, 0.f, 1.f ) )
            {
                physicsWorld.mContactSolver.mRestitution = Fixed::FromFloat( restitution );
            }

            // damping
            if( ImGui::DragInt( "contact solver max iterations", &physicsWorld.mContactSolver.mMaxIterations ) )
            {
            }

            // restitution
            float angularLimitNonLinearProjection = physicsWorld.mContactSolver.mAngularLimitNonLinearProjection.ToFloat();
            if( ImGui::DragFloat( "angular limit non linear projection", &angularLimitNonLinearProjection, 0.01f, 0.f, 10.f ) )
            {
                physicsWorld.mContactSolver.mAngularLimitNonLinearProjection = Fixed::FromFloat( angularLimitNonLinearProjection );
            }
        }
    };
}