#pragma  once

#include "bullet/btBulletDynamicsCommon.h"
#include "ecs/fanEcsComponent.hpp"
#include "core/fanSignal.hpp"

class btCollisionShape;
class btPersistentManifold;
struct btDefaultMotionState;

namespace fan
{
	//==============================================================================================================================================================
	// bullet physics rigidbody
	// must be registered manually
	// unregisters automagically
	//==============================================================================================================================================================
	struct Rigidbody : public EcsComponent
	{
		ECS_COMPONENT( Rigidbody )
	public:
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void Destroy( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );
		static void NetSave( const EcsComponent& _component, sf::Packet& _packet );
		static void NetLoad( EcsComponent& _component, sf::Packet& _packet );

		EcsHandle	GetHandle() { return static_cast<EcsHandle>( rigidbody->getUserIndex() ); }
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
		btVector3   GetAngularVelocity() const;
		void		SetAngularVelocity( const btVector3& _velocity );
		void		SetVelocity( const btVector3& _velocity );
		void		SetTransform( const btTransform& _transform ) { rigidbody->setWorldTransform( _transform ); }
		void		ClearForces() { rigidbody->clearForces();  }

		void SetCollisionShape( btCollisionShape* _collisionShape );
		void SetMotionState( btDefaultMotionState* _motionState );

		btRigidBody * rigidbody;
		Signal<Rigidbody&, Rigidbody&, btPersistentManifold* const&> onContactStarted;
		Signal<Rigidbody&, Rigidbody&, btPersistentManifold* const&> onContactEnded;
	};
	static constexpr size_t sizeof_rigidbody = sizeof( Rigidbody );
}