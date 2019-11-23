#include "fanGlobalIncludes.h"
#include "scene/components/fanRigidbody.h"

#include "scene/fanGameobject.h"
#include "physics/fanPhysicsManager.h"
#include "scene/components/fanColliderShape.h"

namespace fan {
	REGISTER_TYPE_INFO( Rigidbody, TypeInfo::Flags::EDITOR_COMPONENT )

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

		btDefaultMotionState ** tmpMotionState = &const_cast<btDefaultMotionState*>( m_motionState );
		*tmpMotionState = motionState->Init( transform->transform );

		btRigidBody::btRigidBodyConstructionInfo rbInfo( 1.f, m_motionState, m_colShape, localInertia );

		btRigidBody ** tmpRigidbody = &const_cast<btRigidBody*>( m_rigidbody );
		*tmpRigidbody = rigidbody->Init( rbInfo );
		m_rigidbody->setUserPointer(this);

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

		if ( mass > 0.f ){	m_rigidbody->setCollisionFlags( m_rigidbody->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT );}
	}

	//================================================================================================================================
	//================================================================================================================================	
	bool Rigidbody::IsStatic() const {
		return  ! IsKinematic() && m_rigidbody->getInvMass() <= 0.f;
	}

	//================================================================================================================================
	//================================================================================================================================	
	bool Rigidbody::IsKinematic() const
	{
		return  m_rigidbody->getCollisionFlags() & btCollisionObject::CF_KINEMATIC_OBJECT;
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Rigidbody::SetStatic( ) {
		SetMass( 0.f );
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Rigidbody::SetKinematic()
	{
		SetMass( 0.f );
		m_rigidbody->setCollisionFlags( m_rigidbody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT );
		m_rigidbody->setActivationState( DISABLE_DEACTIVATION );
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
		m_rigidbody->setLinearVelocity( _velocity );
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Rigidbody::OnGui() {
		int state = GetMass() > 0 ? DYNAMIC : ( IsKinematic() ? KINEMATIC : STATIC );
		if ( ImGui::Combo( "state", &state, "dynamic\0kinematic\0static\0" ) )
		{
			switch ( state )
			{
				case DYNAMIC : 	if( GetMass() <= 0 ){ SetMass(1.f); } break;
				case KINEMATIC: SetKinematic(); break;
				case STATIC: 	SetStatic(); break;
			default:
				assert(false);
				break;
			}
		}



		// Active
		bool isActive = IsActive();
		if ( ImGui::Checkbox( "active ", &isActive ) )
		{
			if ( isActive )
			{
				Activate();
			}
		} ImGui::SameLine();

		// Desactivation
		bool enableDesactivation = IsDesactivationEnabled();
		if ( ImGui::Checkbox( "enable desactivation ", &enableDesactivation ) )
		{
			EnableDesactivation( enableDesactivation );
		}

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 );
		{
			// Mass
			if ( ImGui::Button( "##Mass" ) )
			{
				SetMass( 0.f );
			} ImGui::SameLine();
			float invMass = m_rigidbody->getInvMass();
			float mass = ( invMass > 0.f ? 1.f / invMass : 0.f );
			if ( ImGui::DragFloat( "mass", &mass, 1.f, 0.f, 1000.f ) )
			{
				SetMass( mass );
			}

			// Velocity
			if ( ImGui::Button( "##Velocity" ) )
			{
				SetVelocity( btVector3( 0, 0, 0 ) );
			} ImGui::SameLine();
			btVector3 velocity = GetVelocity();
			if ( ImGui::DragFloat3( "velocity", &velocity[0], 1.f, -1000.f, 1000.f ) )
			{
				SetVelocity( velocity );
			}
		} ImGui::PopItemWidth();
	}

	//================================================================================================================================
	//================================================================================================================================	
	bool Rigidbody::Load( const Json & _json ) {
		float tmpMass;
		bool tmpEnableDesactivation;
		bool tmpKinematic;

		LoadFloat(_json,"mass", tmpMass );
		LoadBool( _json, "enable_desactivation", tmpEnableDesactivation );
		LoadBool( _json, "is_kinematic", tmpKinematic );

		SetMass( tmpMass );
		EnableDesactivation(tmpEnableDesactivation);
		if( tmpKinematic ) { SetKinematic();}
		
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================	
	bool Rigidbody::Save( Json & _json ) const {
		SaveFloat(_json, "mass", GetMass());
		SaveBool( _json, "enable_desactivation", IsDesactivationEnabled() );
		SaveBool( _json, "is_kinematic", IsKinematic() );

		Component::Save( _json );
		return true;
	}
}
