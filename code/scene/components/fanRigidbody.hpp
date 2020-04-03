#pragma  once

#include "scene/fanScenePrecompiled.hpp"

#include "ecs/fanComponent.hpp"

namespace fan
{
	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct Rigidbody : public Component
	{
		DECLARE_COMPONENT( Rigidbody )
	public:
		Rigidbody();

		static void SetInfo( ComponentInfo& _info );
		static void Init( EcsWorld& _world, Component& _component );
		static void Destroy( EcsWorld& _world, Component& _component );
		static void OnGui( EcsWorld& _world, EntityID _entityID, Component& _component );
		static void Save( const Component& _component, Json& _json );
		static void Load( Component& _component, const Json& _json );

		float		GetMass() const;
		void		SetMass( const float _mass );
		void		SetStatic();
		void		SetKinematic();
		bool		IsStatic() const;
		bool		IsKinematic() const;
		void		Activate();
		bool		IsActive() const;
		void		SetIgnoreCollisionCheck( const Rigidbody& _rb, const bool state );
		bool		IsDeactivationEnabled() const;
		void		EnableDeactivation( const bool _enable );
		btVector3	GetVelocity() const;
		void		SetVelocity( const btVector3& _velocity );
		void		SetTransform( const btTransform& _transform ) { rigidbody.setWorldTransform( _transform ); }

		void SetCollisionShape( btCollisionShape* _collisionShape );
		void SetMotionState( btDefaultMotionState* _motionState );

		btRigidBody rigidbody;
		Signal<Rigidbody*, btPersistentManifold* const&> onContactStarted;
		Signal<Rigidbody*, btPersistentManifold* const&> onContactEnded;
	};
	static constexpr size_t sizeof_rigidbody = sizeof( Rigidbody );
}