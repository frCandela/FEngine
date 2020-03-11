#pragma  once

#include "scene/fanScenePrecompiled.hpp"

#include "scene/ecs/fanEcComponent.hpp"

namespace fan
{
	struct ComponentInfo;

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct Rigidbody2 : public ecComponent
	{
		DECLARE_COMPONENT( Rigidbody2 )
	public:
		Rigidbody2();

		static void SetInfo( ComponentInfo& _info );
		static void Clear( ecComponent& _rigidbody );
		static void OnGui( ecComponent& _rigidbody );
		static void Save( const ecComponent& _rigidbody, Json& _json );
		static void Load( ecComponent& _rigidbody, const Json& _json );		

		float		GetMass() const;
		void		SetMass( const float _mass );
		void		SetStatic();
		void		SetKinematic();
		bool		IsStatic() const;
		bool		IsKinematic() const;
		void		Activate();
		bool		IsActive() const;
		void		SetIgnoreCollisionCheck( const Rigidbody2& _rb, const bool state );
		bool		IsDeactivationEnabled() const;
		void		EnableDeactivation( const bool _enable );
		btVector3	GetVelocity() const;
		void		SetVelocity( const btVector3& _velocity );
		void		SetTransform( const btTransform& _transform ) { rigidbody.setWorldTransform( _transform ); }
		inline void	ApplyCentralForce( const btVector3& _force ) { rigidbody.applyCentralForce( _force ); }

		void SetCollisionShape( btCollisionShape* _collisionShape );

		btRigidBody rigidbody;
	};
	static constexpr size_t sizeof_rigidbody = sizeof( Rigidbody2 );
}