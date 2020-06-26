#include "scene/components/fanRigidbody.hpp"

#include "render/fanRenderSerializable.hpp"
#include "scene/singletons/fanPhysicsWorld.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "network/fanPacket.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void Rigidbody::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::RIGIDBODY16;
		_info.onGui = &Rigidbody::OnGui;
		_info.destroy = &Rigidbody::Destroy;
		_info.load  = &Rigidbody::Load;
		_info.save  = &Rigidbody::Save;
		_info.netSave= &Rigidbody::NetSave;
		_info.netLoad = &Rigidbody::NetLoad;
		_info.rollbackLoad = &Rigidbody::RollbackLoad;
		_info.rollbackSave = &Rigidbody::RollbackSave;
		_info.editorPath = "/";
		_info.name = "rigidbody";
	}
	   
	//================================================================================================================================
	//================================================================================================================================
	void Rigidbody::Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component )
	{
		// clear
		Rigidbody& rb = static_cast<Rigidbody&>( _component );
		rb.rigidbody = new btRigidBody( 1.f, nullptr, nullptr );
		rb.rigidbody->setUserPointer( &_world );
		rb.rigidbody->setUserIndex( _world.GetHandle( _entity ) );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Rigidbody::Destroy( EcsWorld& _world, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		PhysicsWorld& physicsWorld = _world.GetSingleton<PhysicsWorld>();
		Rigidbody& rb = static_cast<Rigidbody&>( _component );
		assert( rb.rigidbody != nullptr );

		physicsWorld.dynamicsWorld->removeRigidBody( rb.rigidbody );

		delete rb.rigidbody;
		rb.rigidbody = nullptr;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Rigidbody::Save( const EcsComponent& _component, Json& _json )
	{
		const Rigidbody& rb = static_cast<const Rigidbody&>( _component );
		Serializable::SaveFloat( _json, "mass", rb.GetMass() );
		Serializable::SaveBool( _json, "enable_desactivation", rb.IsDeactivationEnabled() );
		Serializable::SaveBool( _json, "is_kinematic", rb.IsKinematic() );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Rigidbody::Load( EcsComponent& _component, const Json& _json )
	{
		Rigidbody& rb = static_cast<Rigidbody&>( _component );

		float tmpMass;
		bool tmpEnableDesactivation;
		bool tmpKinematic;

		Serializable::LoadFloat( _json, "mass", tmpMass );
		Serializable::LoadBool( _json, "enable_desactivation", tmpEnableDesactivation );
		Serializable::LoadBool( _json, "is_kinematic", tmpKinematic );

		rb.SetMass( tmpMass );
		rb.EnableDeactivation( tmpEnableDesactivation );
		if( tmpKinematic ) { rb.SetKinematic(); }
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Rigidbody::NetSave( const EcsComponent& _component, sf::Packet& _packet )
	{
		const Rigidbody& rb = static_cast<const Rigidbody&>( _component );
		const btVector3 velocity = rb.GetVelocity();
		const btVector3 angularVelocity = rb.GetAngularVelocity();

		_packet << velocity[0] << velocity[2];
		_packet << angularVelocity[1];
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Rigidbody::NetLoad( EcsComponent& _component, sf::Packet& _packet )
	{
		Rigidbody& rb = static_cast<Rigidbody&>( _component );
		btVector3 velocity( 0.f, 0.f, 0.f );
		btVector3 angularVelocity( 0.f, 0.f, 0.f );
		_packet >> velocity[0] >> velocity[2];
		_packet >> angularVelocity[1] ;
		rb.SetVelocity( velocity );
		rb.SetAngularVelocity( angularVelocity );
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Rigidbody::RollbackSave( const EcsComponent& _component, sf::Packet& _packet )
	{
		const Rigidbody&	rb = static_cast<const Rigidbody&>( _component );

		const btVector3&	velocity = rb.GetVelocity();
		const btVector3&	angularVelocity = rb.GetAngularVelocity();
		_packet << velocity[0] << velocity[2];
		_packet << angularVelocity[1];
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Rigidbody::RollbackLoad( EcsComponent& _component, sf::Packet& _packet )
	{
		btVector3		velocity( 0, 0, 0 );
		btVector3		angularVelocity( 0, 0, 0 );

		_packet >> velocity[0] >> velocity[2];
		_packet >> angularVelocity[1];

		Rigidbody& rb = static_cast<Rigidbody&>( _component );
		rb.ClearForces();
		rb.SetVelocity( velocity );
		rb.SetAngularVelocity( angularVelocity );
	}

	//================================================================================================================================
	//================================================================================================================================	
	float  Rigidbody::GetMass() const
	{
		const float invMass = rigidbody->getInvMass();
		return ( invMass > 0.f ? 1.f / invMass : 0.f );
	}

	//================================================================================================================================
	//================================================================================================================================	
	void  Rigidbody::SetMass( const float _mass )
	{
		btCollisionShape * collisionShape = rigidbody->getCollisionShape();


		float mass = _mass >= 0.f ? _mass : 0.f;
		btVector3 localInertia( 0.f, 0.f, 0.f );
		if( mass != 0.f && collisionShape )
		{
			collisionShape->calculateLocalInertia( mass, localInertia );
		}
		rigidbody->setMassProps( mass, localInertia );

		if( mass > 0.f ) 
		{ 
			rigidbody->setCollisionFlags( rigidbody->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT );
		}
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Rigidbody::SetStatic()
	{
		SetMass( 0.f );
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Rigidbody::SetKinematic()
	{
		SetMass( 0.f );
		rigidbody->setCollisionFlags( rigidbody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT );
		rigidbody->setActivationState( DISABLE_DEACTIVATION );
	}

	//================================================================================================================================
	//================================================================================================================================	
	bool Rigidbody::IsStatic() const
	{
		return  !IsKinematic() && rigidbody->getInvMass() <= 0.f;
	}

	//================================================================================================================================
	//================================================================================================================================	
	bool Rigidbody::IsKinematic() const
	{
		return  rigidbody->getCollisionFlags() & btCollisionObject::CF_KINEMATIC_OBJECT;
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Rigidbody::Activate()
	{
		rigidbody->activate();
	}

	//================================================================================================================================
	//================================================================================================================================	
	bool Rigidbody::IsActive() const
	{
		return rigidbody->isActive();
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Rigidbody::SetIgnoreCollisionCheck( const Rigidbody& _rb, const bool state )
	{
		rigidbody->setIgnoreCollisionCheck( _rb.rigidbody, state );
	}

	//================================================================================================================================
	//================================================================================================================================	
	bool Rigidbody::IsDeactivationEnabled() const
	{
		return  rigidbody->getActivationState() != DISABLE_DEACTIVATION;
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Rigidbody::EnableDeactivation( const bool _enable )
	{
		rigidbody->forceActivationState( _enable ? ACTIVE_TAG : DISABLE_DEACTIVATION );
	}

	//================================================================================================================================
	//================================================================================================================================	
	btVector3			Rigidbody::GetPosition() const { return rigidbody->getWorldTransform().getOrigin(); }
	btQuaternion		Rigidbody::GetRotation() const { return rigidbody->getWorldTransform().getRotation(); }
	const btVector3&	Rigidbody::GetVelocity() const { return rigidbody->getLinearVelocity(); }
	const btVector3&	Rigidbody::GetAngularVelocity() const { return rigidbody->getAngularVelocity(); }
	const btTransform&	Rigidbody::GetTransform() const { return rigidbody->getWorldTransform(); }

	//================================================================================================================================
	//================================================================================================================================	
	void Rigidbody::SetVelocity( const btVector3& _velocity )
	{
		rigidbody->setLinearVelocity( _velocity );
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Rigidbody::SetAngularVelocity( const btVector3& _velocity )
	{
		rigidbody->setAngularVelocity( _velocity );
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Rigidbody::SetCollisionShape( btCollisionShape* _collisionShape )
	{
		const float mass = GetMass();
		btVector3 localInertia( 0, 0, 0 );
		if( mass != 0.f && _collisionShape != nullptr)
		{
			_collisionShape->calculateLocalInertia( mass, localInertia );
			_collisionShape->setUserPointer( &rigidbody );
		}
		rigidbody->setCollisionShape( _collisionShape );
		rigidbody->setMassProps( mass, localInertia );
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Rigidbody::SetMotionState( btDefaultMotionState* _motionState )
	{		
		rigidbody->setMotionState( _motionState );
		if( _motionState != nullptr )
		{
			_motionState->m_userPointer = this;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Rigidbody::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
	{
		Rigidbody& rb = static_cast<Rigidbody&>( _component );

		enum ComboState { DYNAMIC = 0, KINEMATIC = 1, STATIC = 2 };

		int state = rb.GetMass() > 0 ? DYNAMIC : ( rb.IsKinematic() ? KINEMATIC : STATIC );
		if( ImGui::Combo( "state", &state, "dynamic\0kinematic\0static\0" ) )
		{
			switch( state )
			{
			case DYNAMIC: 	if( rb.GetMass() <= 0 ) { rb.SetMass( 1.f ); } break;
			case KINEMATIC: rb.SetKinematic(); break;
			case STATIC: 	rb.SetStatic(); break;
			default:
				assert( false );
				break;
			}
		}

		// Active
		bool isActive = rb.IsActive();
		if( ImGui::Checkbox( "active ", &isActive ) )
		{
			if( isActive )
			{
				rb.Activate();
			}
		} ImGui::SameLine();

		// Deactivation
		bool enableDesactivation = rb.IsDeactivationEnabled();
		if( ImGui::Checkbox( "enable deactivation ", &enableDesactivation ) )
		{
			rb.EnableDeactivation( enableDesactivation );
		}

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 );
		{
			// Mass
			if( ImGui::Button( "##Mass" ) )
			{
				rb.SetMass( 0.f );
			} ImGui::SameLine();
			float invMass = rb.rigidbody->getInvMass();
			float mass = ( invMass > 0.f ? 1.f / invMass : 0.f );
			if( ImGui::DragFloat( "mass", &mass, 1.f, 0.f, 1000.f ) )
			{
				rb.SetMass( mass );
			}

			// Velocity
			if( ImGui::Button( "##Velocity" ) )
			{
				rb.SetVelocity( btVector3( 0, 0, 0 ) );
			} ImGui::SameLine();
			btVector3 velocity = rb.GetVelocity();
			if( ImGui::DragFloat3( "velocity", &velocity[0], 1.f, -1000.f, 1000.f ) )
			{
				rb.SetVelocity( velocity );
			}

			// Angular velocity
			if( ImGui::Button( "##AngularVelocity" ) )
			{
				rb.SetAngularVelocity( btVector3( 0, 0, 0 ) );
			} ImGui::SameLine();
			btVector3 angularVelocity = rb.GetAngularVelocity();
			if( ImGui::DragFloat3( "angular velocity", &angularVelocity[0], 1.f, -1000.f, 1000.f ) )
			{
				rb.SetAngularVelocity( angularVelocity );
			}

		} ImGui::PopItemWidth();
	}
}