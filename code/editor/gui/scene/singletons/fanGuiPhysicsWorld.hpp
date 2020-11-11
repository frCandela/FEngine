#prama once

#include "scene/singletons/fanPhysicsWorld.hpp"

#include "network/singletons/fanTime.hpp"
#include "scene/components/fanRigidbody.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void PhysicsWorld::SetInfo( EcsSingletonInfo& _info )
	{
		_info.mIcon  = ImGui::Rigidbody16;
		_info.mGroup = EngineGroups::ScenePhysics;
		_info.onGui  = &PhysicsWorld::OnGui;
		_info.mName  = "physics world";
	}

    //========================================================================================================
    //========================================================================================================
    void PhysicsWorld::OnGui( EcsWorld&, EcsSingleton& _component )
    {
        PhysicsWorld& physicsWorld = static_cast<PhysicsWorld&>( _component );

        ImGui::Indent(); ImGui::Indent();
        {
            btVector3 gravity = physicsWorld.mDynamicsWorld->getGravity();
            if( ImGui::DragFloat3( "gravity", &gravity[0], 0.1f, -20.f, 20.f ) )
            {
                physicsWorld.mDynamicsWorld->setGravity( gravity );
            }
            ImGui::Text( "num rigidbodies : %d", physicsWorld.mDynamicsWorld->getNumCollisionObjects() );
        }
        ImGui::Unindent(); ImGui::Unindent();
    }
}