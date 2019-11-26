#include "fanGlobalIncludes.h"
#include "scene/components/fanBoxShape.h"

#include "scene/fanGameobject.h"
#include "scene/components/fanTransform.h"
#include "renderer/fanRenderer.h"

namespace fan {
	REGISTER_TYPE_INFO( BoxShape, TypeInfo::Flags::EDITOR_VISIBLE )

	//================================================================================================================================
	//================================================================================================================================
	void BoxShape::SetExtent( const btVector3 _extent ) {
		m_boxShape->setLocalScaling( _extent );
	}

	//================================================================================================================================
	//================================================================================================================================
	btVector3 BoxShape::GetExtent() const {
		return m_boxShape->getLocalScaling();
	}

	//================================================================================================================================
	//================================================================================================================================
	void BoxShape::OnAttach() {

		ecsBoxShape * ecsShape = m_gameobject->AddEcsComponent<ecsBoxShape>();
		ecsShape->Init( btVector3( 0.5f, 0.5f, 0.5f ) );
		btBoxShape ** tmpShape = &const_cast<btBoxShape*>( m_boxShape );
		*tmpShape = &ecsShape->Get();

		ColliderShape::OnAttach();
	}

	//================================================================================================================================
	//================================================================================================================================
	void BoxShape::OnDetach() {
		m_gameobject->RemoveEcsComponent<ecsBoxShape>();

		ColliderShape::OnDetach();
	}

	//================================================================================================================================
	//================================================================================================================================
	void BoxShape::OnGui()
	{
		Component::OnGui();

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			btVector3 extent = GetExtent();
			if ( ImGui::DragFloat3( "half extent", &extent[0], 0.05f, 0.f ) )
			{
				SetExtent( extent );
			}

			Debug::Render().DebugCube( m_gameobject->GetTransform()->GetBtTransform(), 0.5f * extent, Color::Green );
		} ImGui::PopItemWidth();
	}

	//================================================================================================================================
	//================================================================================================================================
	bool BoxShape::Load( const Json & _json ) {
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
	btBoxShape *		BoxShape::GetBoxShape()				{ return m_boxShape;}
	btCollisionShape *	BoxShape::GetCollisionShape()		{ return GetBoxShape(); }
}