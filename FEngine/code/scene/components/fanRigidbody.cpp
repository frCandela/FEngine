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
	void Rigidbody::OnDetach() {

	}

	//================================================================================================================================
	//================================================================================================================================	
	float  Rigidbody::GetMass() const { 
		const float invMass = m_rigidbody->getInvMass();
		return ( invMass > 0.f ? 1.f / invMass : 0.f ) ;
	}

	//================================================================================================================================
	//================================================================================================================================	
	void  Rigidbody::SetMass( const float _mass ) {
		float mass = _mass >= 0.f ? _mass : 0.f;
		btVector3 localInertia( 0.f, 0.f, 0.f );
		if ( mass != 0.f ) {			
			m_colShape->calculateLocalInertia( mass, localInertia );
		}
		m_rigidbody->setMassProps( mass, localInertia );
	}

	//================================================================================================================================
	//================================================================================================================================	
	bool Rigidbody::IsStatic() const {
		return m_rigidbody->getInvMass() <= 0.f;
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Rigidbody::SetStatic( const bool _static ) {
		if( _static ) {
			SetMass( 0.f );
		} else {
			if ( m_rigidbody->getInvMass() == 0.f ) {
				SetMass(1.f);
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Rigidbody::Activate() {
		m_rigidbody->activate();
	}

	//================================================================================================================================
	//================================================================================================================================	
	bool Rigidbody::IsActive() const {
		return m_rigidbody->isActive();
	}

	//================================================================================================================================
	//================================================================================================================================	
	btVector3 Rigidbody::GetVelocity() const {
		return m_rigidbody->getLinearVelocity();
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Rigidbody::SetVelocity( const btVector3& _velocity ) {
		m_rigidbody->setLinearVelocity( _velocity );
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Rigidbody::OnGui() {
		// Static
		bool isStatic = IsStatic();
		if ( ImGui::Checkbox( "static", &isStatic ) ) {
			SetStatic( isStatic );
		} ImGui::SameLine();

		// Active
		bool isActive = IsActive();
		if ( ImGui::Checkbox( "active", &isActive ) ) {
			if ( isActive ) {
				Activate();
			}
		}
		
		// Mass
		if ( ImGui::Button( "##Velocity" ) ) {
			SetMass( 0.f );
		} ImGui::SameLine();
		float invMass =  m_rigidbody->getInvMass();
		float mass = ( invMass > 0.f ? 1.f / invMass : 0.f );
		if ( ImGui::DragFloat( "mass", &mass, 1.f, 0.f, 1000.f ) ) {
			SetMass( mass );			
		}

		// Velocity
		if ( ImGui::Button( "##Velocity" ) ) {
			SetVelocity( btVector3( 0, 0, 0 ) );
		} ImGui::SameLine();
		btVector3 velocity = GetVelocity();
		if ( ImGui::DragFloat3( "velocity", &velocity[0], 1.f, 0.f, 1000.f ) ) {
			SetVelocity(velocity);
		}




	}

	//================================================================================================================================
	//================================================================================================================================	
	bool Rigidbody::Load( Json & _json ) {
		float tmpMass;
		LoadFloat(_json,"mass", tmpMass );

		SetMass( tmpMass );

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================	
	bool Rigidbody::Save( Json & _json ) const {
		SaveFloat(_json, "mass", GetMass());
		Component::Save( _json );
		return true;
	}
}
