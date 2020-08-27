#pragma  once

#include "fanDisableWarnings.hpp"
WARNINGS_BULLET_PUSH()
#include "bullet/btBulletDynamicsCommon.h"
WARNINGS_POP()
#include "ecs/fanEcsComponent.hpp"
#include "ecs/fanSignal.hpp"

class btCollisionShape;
class btPersistentManifold;
struct btDefaultMotionState;

namespace fan
{
	//========================================================================================================
	// bullet physics rigidbody
	// must be registered manually
	// unregisters automagically
	//========================================================================================================
	struct Rigidbody : public EcsComponent
	{
		ECS_COMPONENT( Rigidbody )
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void Destroy( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );
		static void NetSave( const EcsComponent& _component, sf::Packet& _packet );
		static void NetLoad( EcsComponent& _component, sf::Packet& _packet );
		static void RollbackSave( const EcsComponent& _component, sf::Packet& _packet );
		static void RollbackLoad( EcsComponent& _component, sf::Packet& _packet );

		EcsHandle	GetHandle(){ return static_cast<EcsHandle>( mRigidbody->getUserIndex() ); }
		EcsWorld&	GetWorld() { return *static_cast<EcsWorld*>( mRigidbody->getUserPointer() ); }
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
		btVector3			GetPosition() const;
		btQuaternion		GetRotation() const;
		const btVector3&	GetVelocity() const;
		const btVector3&	GetAngularVelocity() const;
		const btTransform&	GetTransform() const;
		void		SetAngularVelocity( const btVector3& _velocity );
		void		SetVelocity( const btVector3& _velocity );
		void		SetTransform( const btTransform& _transform );
		void		ClearForces() { mRigidbody->clearForces();  }

		void SetCollisionShape( btCollisionShape* _collisionShape );
		void SetMotionState( btDefaultMotionState* _motionState );

		btRigidBody * mRigidbody;
		Signal<Rigidbody&, Rigidbody&, btPersistentManifold* const&> mOnContactStarted;
		Signal<Rigidbody&, Rigidbody&, btPersistentManifold* const&> mOnContactEnded;
	};
}