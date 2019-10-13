#include "fanGlobalIncludes.h"
#include "physics/fanPhysicsManager.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================	
	PhysicsManager::PhysicsManager( const btVector3 _gravity ) {
		m_collisionConfiguration = new btDefaultCollisionConfiguration();
		m_dispatcher = new btCollisionDispatcher( m_collisionConfiguration );
		m_overlappingPairCache = new btDbvtBroadphase();
		m_solver = new btSequentialImpulseConstraintSolver;
		m_dynamicsWorld = new btDiscreteDynamicsWorld( m_dispatcher, m_overlappingPairCache, m_solver, m_collisionConfiguration );

		m_dynamicsWorld->setGravity( _gravity );
	}

	//================================================================================================================================
	//================================================================================================================================	
	PhysicsManager::~PhysicsManager() {
		delete m_dynamicsWorld;
		delete m_solver;
		delete m_overlappingPairCache;
		delete m_dispatcher;
		delete m_collisionConfiguration;		
	}

	//================================================================================================================================
	//================================================================================================================================	
	void PhysicsManager::OnGui() {
		ImGui::Begin( "Physics" ); {
			ImGui::Text(std::to_string(sizeof( btRigidBody )).c_str());

		} ImGui::End();
	}

}