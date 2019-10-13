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
		virtual bool Load( Json & _json ) override;
		virtual bool Save( Json & _json ) const override;
		
	private:
		btRigidBody * _rigidbody;
	};

}