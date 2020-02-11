#include "scene/components/fanColliderShape.hpp"
#include "scene/components/fanRigidbody.hpp"
#include "scene/actors/fanActor.hpp"

namespace fan
{

	REGISTER_ABSTRACT_TYPE_INFO( ColliderShape );

	//================================================================================================================================
	//================================================================================================================================
	void ColliderShape::OnAttach()
	{
		Rigidbody* rb = m_gameobject->GetComponent<Rigidbody>();
		if ( rb != nullptr )
		{
			rb->SetCollisionShape( GetCollisionShape() );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ColliderShape::OnDetach()
	{
		// Remove collider from rigidbody
		Rigidbody* rb = m_gameobject->GetComponent<Rigidbody>();
		if ( rb != nullptr )
		{
			rb->SetCollisionShape( nullptr );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ColliderShape::OnGui()
	{
		Component::OnGui();
	}

	//================================================================================================================================
	//================================================================================================================================
	bool ColliderShape::Load( const Json& /*_json*/ )
	{
		// 		if (!ReadSegmentHeader(_in, "isEnabled:")) { return false; }
		// 		if (!ReadBool(_in, m_isEnabled)) { return false; }
		return true;
	}


	//================================================================================================================================
	//================================================================================================================================
	bool ColliderShape::Save( Json& _json ) const
	{
		//Serializable::SaveBool( jActor, "isEnabled", m_isEnabled );
		Component::Save( _json );
		return true;
	}
}