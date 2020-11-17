#pragma once

#include "engine/singletons/fanPhysicsWorld.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"

namespace fan
{
    struct GuiPhysicsWorld
    {
        //====================================================================================================
        //====================================================================================================
        static GuiSingletonInfo GetInfo()
        {
            GuiSingletonInfo info;
            info.mIcon  = ImGui::Rigidbody16;
            info.mGroup = EngineGroups::ScenePhysics;
            info.onGui  = &GuiPhysicsWorld::OnGui;
            info.mEditorName  = "physics world";
            return info;
        }

        //========================================================================================================
        //========================================================================================================
        static void OnGui( EcsWorld&, EcsSingleton& _component )
        {
            PhysicsWorld& physicsWorld = static_cast<PhysicsWorld&>( _component );

            ImGui::Indent();
            ImGui::Indent();
            {
                btVector3 gravity = physicsWorld.mDynamicsWorld->getGravity();
                if( ImGui::DragFloat3( "gravity", &gravity[0], 0.1f, -20.f, 20.f ) )
                {
                    physicsWorld.mDynamicsWorld->setGravity( gravity );
                }
                ImGui::Text( "num rigidbodies : %d", physicsWorld.mDynamicsWorld->getNumCollisionObjects() );
            }
            ImGui::Unindent();
            ImGui::Unindent();
        }
    };
}