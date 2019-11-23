#pragma once

#include "scene/components/fanComponent.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================	
	class Rigidbody : public Component {
	public:
		friend class PhysicsManager;

		Signal<Rigidbody*, btPersistentManifold* const&> onContactStarted;
		Signal<Rigidbody*, btPersistentManifold* const&> onContactEnded;

		btRigidBody *			GetBtBody()		{ return m_rigidbody; }
		btDefaultMotionState *  GetBtMotion() { return m_motionState; }

		float			GetMass() const;
		void			SetMass( const float _mass );
		void			SetStatic();
		void			SetKinematic();
		bool			IsStatic() const;
		bool			IsKinematic() const;
		void			Activate();		
		bool			IsActive() const;

		bool			IsDesactivationEnabled() const;
		void			EnableDesactivation( const bool _enable );
		btVector3		GetVelocity() const;
		void			SetVelocity( const btVector3& _velocity );
		void			SetTransform( const btTransform& _transform ) { m_rigidbody->setWorldTransform( _transform ); }
		inline void		ApplyCentralForce( const btVector3& _force ) { m_rigidbody->applyCentralForce( _force ); }

		void SetCollisionShape( btCollisionShape * _collisionShape );

		void OnGui() override;
		ImGui::IconType GetIcon() const override { return ImGui::IconType::RIGIDBODY; }

		DECLARE_TYPE_INFO( Rigidbody, Component );
	protected:
		bool Load( const Json & _json ) override;
		bool Save( Json & _json ) const override;

		void OnAttach() override;
		void OnDetach() override;

	private:
		btRigidBody * const				m_rigidbody =  nullptr;
		btDefaultMotionState  * const	m_motionState = nullptr;
		btCollisionShape*				m_colShape = nullptr;

		btCollisionShape *  FindCollisionShape();

		// Editor
		enum ComboState{ DYNAMIC = 0, KINEMATIC = 1, STATIC= 2 };		
	};

}