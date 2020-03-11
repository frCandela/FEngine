#include "scene/ecs/components/fanRigidbody2.hpp"

#include "render/fanRenderSerializable.hpp"

namespace fan
{
	REGISTER_COMPONENT( Rigidbody2, "rigidbody" );

	//================================================================================================================================
	//================================================================================================================================
	Rigidbody2::Rigidbody2() : rigidbody( 1.f, nullptr, nullptr ){}

	//================================================================================================================================
	//================================================================================================================================
	void Rigidbody2::SetInfo( ComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::RIGIDBODY16;
		_info.onGui = &Rigidbody2::OnGui;
		_info.clear = &Rigidbody2::Clear;
		_info.load  = &Rigidbody2::Load;
		_info.save  = &Rigidbody2::Save;
		_info.editorPath = "";
	}

	//================================================================================================================================
	//================================================================================================================================
	void Rigidbody2::Clear( ecComponent& _rigidbody )
	{
		Rigidbody2& rb = static_cast<Rigidbody2&>( _rigidbody );
		rb.rigidbody = btRigidBody( 1.f, nullptr, nullptr );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Rigidbody2::OnGui( ecComponent& _rigidbody )
	{
		Rigidbody2& rb = static_cast<Rigidbody2&>( _rigidbody );

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
			float invMass = rb.rigidbody.getInvMass();
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
		} ImGui::PopItemWidth();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Rigidbody2::Save( const ecComponent& _rigidbody, Json& _json )
	{
		const Rigidbody2& rb = static_cast<const Rigidbody2&>( _rigidbody );
		Serializable::SaveFloat( _json, "mass", rb.GetMass() );
		Serializable::SaveBool( _json, "enable_desactivation", rb.IsDeactivationEnabled() );
		Serializable::SaveBool( _json, "is_kinematic", rb.IsKinematic() );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Rigidbody2::Load( ecComponent& _rigidbody, const Json& _json )
	{
		Rigidbody2& rb = static_cast<Rigidbody2&>( _rigidbody );

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
	float  Rigidbody2::GetMass() const
	{
		const float invMass = rigidbody.getInvMass();
		return ( invMass > 0.f ? 1.f / invMass : 0.f );
	}

	//================================================================================================================================
	//================================================================================================================================	
	void  Rigidbody2::SetMass( const float _mass )
	{
		btCollisionShape * collisionShape = rigidbody.getCollisionShape();


		float mass = _mass >= 0.f ? _mass : 0.f;
		btVector3 localInertia( 0.f, 0.f, 0.f );
		if( mass != 0.f && collisionShape )
		{
			collisionShape->calculateLocalInertia( mass, localInertia );
		}
		rigidbody.setMassProps( mass, localInertia );

		if( mass > 0.f ) 
		{ 
			rigidbody.setCollisionFlags( rigidbody.getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT ); 
		}
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Rigidbody2::SetStatic()
	{
		SetMass( 0.f );
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Rigidbody2::SetKinematic()
	{
		SetMass( 0.f );
		rigidbody.setCollisionFlags( rigidbody.getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT );
		rigidbody.setActivationState( DISABLE_DEACTIVATION );
	}

	//================================================================================================================================
	//================================================================================================================================	
	bool Rigidbody2::IsStatic() const
	{
		return  !IsKinematic() && rigidbody.getInvMass() <= 0.f;
	}

	//================================================================================================================================
	//================================================================================================================================	
	bool Rigidbody2::IsKinematic() const
	{
		return  rigidbody.getCollisionFlags() & btCollisionObject::CF_KINEMATIC_OBJECT;
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Rigidbody2::Activate()
	{
		rigidbody.activate();
	}

	//================================================================================================================================
	//================================================================================================================================	
	bool Rigidbody2::IsActive() const
	{
		return rigidbody.isActive();
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Rigidbody2::SetIgnoreCollisionCheck( const Rigidbody2& _rb, const bool state )
	{
		rigidbody.setIgnoreCollisionCheck( &_rb.rigidbody, state );
	}

	//================================================================================================================================
	//================================================================================================================================	
	bool Rigidbody2::IsDeactivationEnabled() const
	{
		return  rigidbody.getActivationState() != DISABLE_DEACTIVATION;
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Rigidbody2::EnableDeactivation( const bool _enable )
	{
		rigidbody.forceActivationState( _enable ? ACTIVE_TAG : DISABLE_DEACTIVATION );
	}

	//================================================================================================================================
	//================================================================================================================================	
	btVector3 Rigidbody2::GetVelocity() const
	{
		return rigidbody.getLinearVelocity();
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Rigidbody2::SetVelocity( const btVector3& _velocity )
	{
		rigidbody.setLinearVelocity( _velocity );
	}

	//================================================================================================================================
	//================================================================================================================================	
	void Rigidbody2::SetCollisionShape( btCollisionShape* _collisionShape )
	{
		const float mass = GetMass();
		btVector3 localInertia( 0, 0, 0 );
		if( mass != 0.f && _collisionShape != nullptr)
		{
			_collisionShape->calculateLocalInertia( mass, localInertia );
		}
		rigidbody.setCollisionShape( _collisionShape );
		rigidbody.setMassProps( mass, localInertia );
	}
}