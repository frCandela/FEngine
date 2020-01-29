#include "scene/components/fanSphereShape.hpp"
#include "scene/components/fanTransform.hpp"
#include "render/fanRendererDebug.hpp"
// #include "editor/fanEditorDebug.hpp" @migration

namespace fan
{
	REGISTER_TYPE_INFO( SphereShape, TypeInfo::Flags::EDITOR_COMPONENT, "physics/" )

	//================================================================================================================================
	//================================================================================================================================
	void  SphereShape::SetRadius( const float _radius ) {
		m_sphereShape->setUnscaledRadius( _radius >= 0 ? _radius : 0.f );
	}

	//================================================================================================================================
	//================================================================================================================================
	float SphereShape::GetRadius() const {
		return m_sphereShape->getRadius();
	}

	//================================================================================================================================
	//================================================================================================================================
	void SphereShape::OnAttach() {

		ecsSphereShape * ecsShape = m_gameobject->AddEcsComponent<ecsSphereShape>();
		ecsShape->Init( 1.f );
		btSphereShape ** shape = &const_cast<btSphereShape*>( m_sphereShape );
		*shape = &ecsShape->Get();

		ColliderShape::OnAttach();
	}

	//================================================================================================================================
	//================================================================================================================================
	void SphereShape::OnDetach() {
		m_gameobject->RemoveEcsComponent<ecsSphereShape>();

		ColliderShape::OnDetach();
	}


	//================================================================================================================================
	//================================================================================================================================
	void SphereShape::OnGui()
	{
		Component::OnGui();

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			float radius = GetRadius();
			if ( ImGui::DragFloat( "radius", &radius, 0.1f, 0.f ) )
			{
				SetRadius( radius );
			}
			//@migration EditorDebug::Get().Renderer().DebugSphere( m_gameobject->GetTransform().GetBtTransform(), radius, 2, Color::Green );
		} ImGui::PopItemWidth();

	
	}

	//================================================================================================================================
	//================================================================================================================================
	bool SphereShape::Load( const Json & _json ) {

		float radius;
		Serializable::LoadFloat(_json, "radius", radius );

		SetRadius( radius );
		return true;
	}


	//================================================================================================================================
	//================================================================================================================================
	bool SphereShape::Save( Json & _json ) const {
		Serializable::SaveFloat( _json, "radius", GetRadius() );
		Component::Save( _json );
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	btSphereShape *		SphereShape::GetSphereShape()			{ return m_sphereShape;	}
	btCollisionShape *	SphereShape::GetCollisionShape()		{ return GetSphereShape(); }
}