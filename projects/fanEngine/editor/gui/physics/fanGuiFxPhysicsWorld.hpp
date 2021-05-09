#pragma once

#include "engine/physics/fanFxPhysicsWorld.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct GuiFxPhysicsWorld
    {
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mIcon       = ImGui::Rigidbody16;
            info.mGroup      = EngineGroups::ScenePhysics;
            info.onGui       = &GuiFxPhysicsWorld::OnGui;
            info.mEditorName = "fxPhysicsWorld";
            return info;
        }

        static void OnGui( EcsWorld&, EcsSingleton& _component )
        {
            FxPhysicsWorld& physicsWorld = static_cast<FxPhysicsWorld&>( _component );

            // gravity
            glm::vec3 gravity = physicsWorld.mGravity.ToGlm();
            if( ImGui::DragFloat3( "gravity", &gravity.x ) )
            {
                physicsWorld.mGravity = Vector3( gravity );
            }

            // damping
            float damping = physicsWorld.mLinearDamping.ToFloat();
            if( ImGui::DragFloat( "damping", &damping ) )
            {
                physicsWorld.mLinearDamping = Fixed::FromFloat( damping );
            }

            // angular damping
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

            // friction
            float friction = physicsWorld.mContactSolver.mFriction.ToFloat();
            if( ImGui::DragFloat( "friction", &friction, 0.01f, 0.f, 1.f ) )
            {
                physicsWorld.mContactSolver.mFriction = Fixed::FromFloat( friction );
            }

            // resting velocity limit
            float restingVelocityLimit = physicsWorld.mContactSolver.mRestingVelocityLimit.ToFloat();
            if( ImGui::DragFloat( "resting velocity limit", &restingVelocityLimit, 0.01f, 0.f, 1.f ) )
            {
                physicsWorld.mContactSolver.mRestingVelocityLimit = Fixed::FromFloat( restingVelocityLimit );
            }

            // angular limit non linear projection
            float angularLimitNonLinearProjection = physicsWorld.mContactSolver.mAngularLimitNonLinearProjection.ToFloat();
            if( ImGui::DragFloat( "angular limit non linear projection", &angularLimitNonLinearProjection, 0.01f, 0.f, 10.f ) )
            {
                physicsWorld.mContactSolver.mAngularLimitNonLinearProjection = Fixed::FromFloat( angularLimitNonLinearProjection );
            }

            // iterations
            if( ImGui::DragInt( "num position iterations", &physicsWorld.mContactSolver.mMaxPositionsIterations ) ){}
            if( ImGui::DragInt( "num contact  iterations", &physicsWorld.mContactSolver.mMaxVelocityIterations ) ){}

            ImGui::PushReadOnly();
            if( ImGui::DragInt( "used position iterations", &physicsWorld.mContactSolver.mPositionIterationsUsed ) ){}
            if( ImGui::DragInt( "used contact  iterations", &physicsWorld.mContactSolver.mVelocityIterationsUsed ) ){}
            ImGui::PopReadOnly();
        }
    };
}