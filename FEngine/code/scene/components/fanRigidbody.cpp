#include "fanGlobalIncludes.h"
#include "scene/components/fanRigidbody.h"

#include "scene/fanGameobject.h"
#include "physics/fanPhysicsManager.h"
#include "scene/components/fanSphereShape.h"
#include "scene/components/fanBoxShape.h"

namespace fan {
	REGISTER_EDITOR_COMPONENT( Rigidbody );
	REGISTER_TYPE_INFO( Rigidbody )

	//================================================================================================================================
	// Updates the rigidbody pointers
	// Happens when too much rigidbodies are created in the ecs and their container is resized
	//================================================================================================================================	
	void Rigidbody::Refresh() {	
		FindCollisionShape();
		m_motionState = & GetGameobject()->GetEcsComponent<ecsMotionState>()->Get();
		m_rigidbody = & GetGameobject()->GetEcsComponent<ecsRigidbody>()->Get();
		m_rigidbody->setCollisionShape( m_colShape );
		m_rigidbody->setMotionState(m_motionState);
	}

	//================================================================================================================================
	// TODO refacto of collision shapes
	//================================================================================================================================	
	void Rigidbody::FindCollisionShape() {
		m_colShape = nullptr;

		SphereShape * sphereShape = GetGameobject()->GetComponent<SphereShape>();
		if ( sphereShape != nullptr ) {
			m_colShape = sphereShape->GetBtShape();
			return;
		}

		BoxShape * boxShape = GetGameobject()->GetComponent<BoxShape>();
		if ( boxShape != nullptr ) {
			m_colShape = boxShape->GetBtShape();
			return;
		}
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Rigidbody::OnAttach() {
		Component::OnAttach();

		ecsMotionState * motionState = GetGameobject()->AddEcsComponent<ecsMotionState>();
		ecsRigidbody * rigidbody = GetGameobject()->AddEcsComponent<ecsRigidbody>();
		ecsTranform * transform = GetGameobject()->GetEcsComponent<ecsTranform>();

		float startMass = 1.f;

		FindCollisionShape();

		bool isDynamic = ( startMass != 0.f );
		btVector3 localInertia( 0, 0, 0 );
		if ( isDynamic && m_colShape != nullptr ) {
			m_colShape->calculateLocalInertia( startMass, localInertia );
		}

		m_motionState = motionState->Init( transform->transform );
		btRigidBody::btRigidBodyConstructionInfo rbInfo( 1.f, m_motionState, m_colShape );
		m_rigidbody = rigidbody->Init( rbInfo );

		GetGameobject()->GetScene()->GetPhysicsManager()->AddRigidbody( this );
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Rigidbody::OnDetach() {
		Component::OnDetach();
		GetGameobject()->GetScene()->GetPhysicsManager()->RemoveRigidbody( this );
		GetGameobject()->RemoveEcsComponent<ecsMotionState>();
		GetGameobject()->RemoveEcsComponent<ecsRigidbody>();
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
		if ( mass != 0.f  && m_colShape ) {			
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
		if ( ImGui::Button( "##Mass" ) ) {
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

		// Test
		if ( ImGui::Button( "test" ) ) {
			FindCollisionShape();
			m_rigidbody->setCollisionShape(m_colShape);
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
