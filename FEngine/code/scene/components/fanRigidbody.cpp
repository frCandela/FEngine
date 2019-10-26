#include "fanGlobalIncludes.h"
#include "scene/components/fanRigidbody.h"

#include "scene/fanGameobject.h"
#include "physics/fanPhysicsManager.h"
#include "scene/components/fanColliderShape.h"

namespace fan {
	REGISTER_EDITOR_COMPONENT( Rigidbody );
	REGISTER_TYPE_INFO( Rigidbody )

	//================================================================================================================================
	// Updates the rigidbody pointers
	// Happens when too much rigidbodies are created in the ecs and their container is resized
	//================================================================================================================================	
	void Rigidbody::Refresh() {	
		m_colShape = FindCollisionShape();
		m_motionState = & m_gameobject->GetEcsComponent<ecsMotionState>()->Get();
		m_rigidbody   = & m_gameobject->GetEcsComponent<ecsRigidbody>()->Get();
		m_rigidbody->setCollisionShape( m_colShape );
		m_rigidbody->setMotionState(m_motionState);
	}

	//================================================================================================================================
	//================================================================================================================================	
	btCollisionShape * Rigidbody::FindCollisionShape() {
		const std::vector<Component*> & components = m_gameobject->GetComponents();
		for (int componentIndex = 0; componentIndex < components.size(); componentIndex++) {
			Component* component = components[componentIndex];
			if ( component->IsCollider() ) {
				return static_cast<ColliderShape*>(component)->GetCollisionShape();
			}
		}
		return nullptr;
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Rigidbody::SetCollisionShape( btCollisionShape * _collisionShape ) {
		if ( _collisionShape != nullptr ) {
			const float mass = GetMass();
			btVector3 localInertia( 0, 0, 0 );
			if ( mass != 0.f ) {
				_collisionShape->calculateLocalInertia( mass, localInertia );
			}
			m_rigidbody->setCollisionShape( _collisionShape );
			m_rigidbody->setMassProps( mass, localInertia );

			if ( m_colShape == nullptr ) {
				m_gameobject->GetScene()->GetPhysicsManager()->AddRigidbody( this );
			}
			m_colShape = _collisionShape;

		} else {
			m_colShape = nullptr;
			m_rigidbody->setCollisionShape(nullptr);	
			m_gameobject->GetScene()->GetPhysicsManager()->RemoveRigidbody( this );
		}
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Rigidbody::OnAttach() {
		Component::OnAttach();

		ecsMotionState * motionState = m_gameobject->AddEcsComponent<ecsMotionState>();
		ecsRigidbody * rigidbody	 = m_gameobject->AddEcsComponent<ecsRigidbody>();
		ecsTranform * transform		 = m_gameobject->GetEcsComponent<ecsTranform>();

		float startMass = 0.f;

		m_colShape = FindCollisionShape();

		bool isDynamic = ( startMass != 0.f );
		btVector3 localInertia( 0, 0, 0 );
		if ( isDynamic && m_colShape != nullptr ) {
			m_colShape->calculateLocalInertia( startMass, localInertia );
		}

		m_motionState = motionState->Init( transform->transform );
		btRigidBody::btRigidBodyConstructionInfo rbInfo( 1.f, m_motionState, m_colShape, localInertia );
		m_rigidbody = rigidbody->Init( rbInfo );

		if( m_colShape != nullptr ){
			m_gameobject->GetScene()->GetPhysicsManager()->AddRigidbody( this );
		}
		m_gameobject->GetScene()->GetPhysicsManager()->RegisterRigidbody( this );
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Rigidbody::OnDetach() {
		Component::OnDetach();
		m_gameobject->GetScene()->GetPhysicsManager()->RemoveRigidbody( this );
		m_gameobject->RemoveEcsComponent<ecsMotionState>();
		m_gameobject->RemoveEcsComponent<ecsRigidbody>();
		m_gameobject->GetScene()->GetPhysicsManager()->UnRegisterRigidbody( this );
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
		Activate();
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
			Activate();
		}
	}

	//================================================================================================================================
	//================================================================================================================================	
	bool Rigidbody::IsDesactivationEnabled() const {
		return  m_rigidbody->getActivationState() != DISABLE_DEACTIVATION;
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Rigidbody::EnableDesactivation( const bool _enable ) {
		m_rigidbody->forceActivationState( _enable ? ACTIVE_TAG :  DISABLE_DEACTIVATION );
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
		Activate();
		m_rigidbody->setLinearVelocity( _velocity );
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Rigidbody::OnGui() {
		// Static
		bool isStatic = IsStatic();
		if ( ImGui::Checkbox( "static ", &isStatic ) ) {
			SetStatic( isStatic );
		} ImGui::SameLine();

		// Active
		bool isActive = IsActive();
		if ( ImGui::Checkbox( "active ", &isActive ) ) {
			if ( isActive ) {
				Activate();
			}
		} ImGui::SameLine();

		// Desactivation
		bool enableDesactivation =IsDesactivationEnabled();
		if ( ImGui::Checkbox( "enable desactivation ", &enableDesactivation ) ) {
			EnableDesactivation(enableDesactivation);
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
		if ( ImGui::DragFloat3( "velocity", &velocity[0], 1.f, -1000.f, 1000.f ) ) {
			SetVelocity(velocity);
		}

		if ( m_colShape != nullptr ) {
			ImGui::Text("linked");
		}

		// Test
		if ( ImGui::Button( "tests" ) ) {
			Activate();
		}
	}

	//================================================================================================================================
	//================================================================================================================================	
	bool Rigidbody::Load( Json & _json ) {
		float tmpMass;
		bool tmpEnableDesactivation;

		LoadFloat(_json,"mass", tmpMass );
		LoadBool( _json, "enable_desactivation", tmpEnableDesactivation );

		SetMass( tmpMass );
		EnableDesactivation(tmpEnableDesactivation);

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================	
	bool Rigidbody::Save( Json & _json ) const {
		SaveFloat(_json, "mass", GetMass());
		SaveBool( _json, "enable_desactivation", IsDesactivationEnabled() );

		Component::Save( _json );
		return true;
	}
}
