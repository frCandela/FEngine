#include "fanGlobalIncludes.h"
#include "scene/components/fanRigidbody.h"
#include "scene/fanGameobject.h"
#include "physics/fanPhysicsManager.h"

namespace fan {
	REGISTER_EDITOR_COMPONENT( Rigidbody );
	REGISTER_TYPE_INFO( Rigidbody )

	//================================================================================================================================
	//================================================================================================================================	
	Rigidbody::Rigidbody() {

	}

	//================================================================================================================================
	//================================================================================================================================	
	Rigidbody::~Rigidbody() {

	}

	//================================================================================================================================
	//================================================================================================================================	
	void Rigidbody::OnAttach() {
		ecsMotionState * motionState = GetGameobject()->AddEcsComponent<ecsMotionState>();
		ecsRigidbody * rigidbody = GetGameobject()->AddEcsComponent<ecsRigidbody>();
		ecsTranform * transform = GetGameobject()->GetEcsComponent<ecsTranform>();

		float startMass = 1.f;

		m_colShape = new btSphereShape( btScalar( 1.f ) );
		bool isDynamic = ( startMass != 0.f );
		btVector3 localInertia( 0, 0, 0 );
		if ( isDynamic ) {
			m_colShape->calculateLocalInertia( startMass, localInertia );
		}

		m_motionState = motionState->Init( transform->transform );
		btRigidBody::btRigidBodyConstructionInfo rbInfo( 1.f, m_motionState, m_colShape );
		m_rigidbody = rigidbody->Init( rbInfo );

		GetGameobject()->GetScene()->GetPhysicsManager()->AddRigidbody( m_rigidbody );
	}

	//================================================================================================================================
	//================================================================================================================================	
	void  Rigidbody::SetMass( const float _mass ) {
		assert( _mass >= 0 );
		bool isDynamic = ( _mass != 0.f );
		btVector3 localInertia( 0, 0, 0 );
		if ( isDynamic ) {
			m_colShape->calculateLocalInertia( _mass, localInertia );
		}
		m_rigidbody->setMassProps( _mass, localInertia );

		m_rigidbody->activate();
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Rigidbody::OnGui() {
		float invMass =  m_rigidbody->getInvMass();
		float mass = ( invMass > 0.f ? 1.f / invMass : 0.f );
		if ( ImGui::DragFloat( "mass", &mass, 1.f, 0.f, 1000.f ) ) {
			SetMass( mass );			
		}

		btVector3 velocity = m_rigidbody->getLinearVelocity();
		if ( ImGui::DragFloat3( "velocity", &velocity[0], 1.f, 0.f, 1000.f ) ) {
			m_rigidbody->setLinearVelocity(velocity);
		}
	}

	//================================================================================================================================
	//================================================================================================================================	
	bool Rigidbody::Load( Json & /*_json*/ ) {
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================	
	bool Rigidbody::Save( Json & _json ) const {
		Component::Save( _json );
		return true;
	}
}
