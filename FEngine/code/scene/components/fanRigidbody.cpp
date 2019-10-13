#include "fanGlobalIncludes.h"
#include "scene/components/fanRigidbody.h"
#include "scene/fanGameobject.h"

namespace fan {
	REGISTER_EDITOR_COMPONENT( Rigidbody );
	REGISTER_TYPE_INFO( Rigidbody )

	//================================================================================================================================
	//================================================================================================================================	
	Rigidbody::Rigidbody() {

	}

	//================================================================================================================================
	//================================================================================================================================	
	Rigidbody::~Rigidbody() {

	}

	//================================================================================================================================
	//================================================================================================================================	
	void Rigidbody::OnAttach() {

		ecsMotionState * motionState = GetGameobject()->AddEcsComponent<ecsMotionState>();
		ecsRigidbody * rigidbody = GetGameobject()->AddEcsComponent<ecsRigidbody>();

		(void)motionState;
		(void)rigidbody;
		//rigidbody is dynamic if and only if mass is non zero, otherwise static
// 		bool isDynamic = ( mass != 0.f );
// 
// 		btVector3 localInertia( 0, 0, 0 );
// 		if ( isDynamic )
// 			groundShape->calculateLocalInertia( mass, localInertia );
// 
// 		m_motionState = motionState->Init();
// 
// 		btRigidBody::btRigidBodyConstructionInfo rbInfo( mass, m_motionState, groundShape );
// 		m_rigidbody = rigidbody->Init( rbInfo );
	}

	//================================================================================================================================
	//================================================================================================================================	
	bool Rigidbody::Load( Json & /*_json*/ ) {
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================	
	bool Rigidbody::Save( Json & _json ) const {
		Component::Save( _json );
		return true;
	}
}
