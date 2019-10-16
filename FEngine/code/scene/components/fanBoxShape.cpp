#include "fanGlobalIncludes.h"
#include "scene/components/fanBoxShape.h"

#include "scene/fanGameobject.h"
#include "scene/components/fanTransform.h"
#include "renderer/fanRenderer.h"

namespace fan {

	REGISTER_EDITOR_COMPONENT( BoxShape );
	REGISTER_TYPE_INFO( BoxShape )

	//================================================================================================================================
	//================================================================================================================================
	void BoxShape::SetExtent( const btVector3 _extent ) {
		GetEcsBoxShape()->Get().setLocalScaling( _extent );
	}

	//================================================================================================================================
	//================================================================================================================================
	btVector3 BoxShape::GetExtent() const {
		return GetEcsBoxShape()->Get().getLocalScaling();
	}

	//================================================================================================================================
	//================================================================================================================================
	void BoxShape::OnAttach() {
		ecsBoxShape * boxShape = GetGameobject()->AddEcsComponent<ecsBoxShape>();
		boxShape->Init( btVector3(0.5f, 0.5f, 0.5f) );

		ColliderShape::OnAttach();
	}

	//================================================================================================================================
	//================================================================================================================================
	void BoxShape::OnDetach() {
		GetGameobject()->RemoveEcsComponent<ecsBoxShape>();

		ColliderShape::OnDetach();
	}

	//================================================================================================================================
	//================================================================================================================================
	void BoxShape::OnGui() {
		Component::OnGui();

		btVector3 extent = GetExtent();;
		if ( ImGui::DragFloat3("half extent", &extent[0], 0.25f, 0.f ) ) {
			SetExtent( extent );
		}

		Debug::Render().DebugCube( GetGameobject()->GetTransform()->GetBtTransform(), 0.5f * extent, Color::Green );

	}

	//================================================================================================================================
	//================================================================================================================================
	bool BoxShape::Load( Json & _json ) {
		btVector3 extent;

		LoadVec3(_json, "extent", extent );

		SetExtent(extent);
		return true;
	}


	//================================================================================================================================
	//================================================================================================================================
	bool BoxShape::Save( Json & _json ) const {
		SaveVec3( _json, "extent", GetExtent() );
		Component::Save( _json );
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	ecsBoxShape*		BoxShape::GetEcsBoxShape() const	{ return GetGameobject()->GetEcsComponent<ecsBoxShape>();}
	btBoxShape *		BoxShape::GetBoxShape()				{ return &GetEcsBoxShape()->Get();}
	btCollisionShape *	BoxShape::GetCollisionShape()		{ return GetBoxShape(); }
}