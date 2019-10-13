#pragma once

#include "scene/components/fanComponent.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================	
	class Rigidbody : public Component {
	public:
		Rigidbody();
		~Rigidbody();

		DECLARE_EDITOR_COMPONENT( Rigidbody )
		DECLARE_TYPE_INFO( Rigidbody );		
	protected:
		bool Load( Json & _json ) override;
		bool Save( Json & _json ) const override;
		void OnAttach() override;

	private:
		btRigidBody * m_rigidbody;
		btDefaultMotionState  * m_motionState;
	};

}