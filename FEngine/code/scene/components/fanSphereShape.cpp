#include "fanGlobalIncludes.h"
#include "scene/components/fanSphereShape.h"

#include "scene/fanGameobject.h"
#include "scene/components/fanTransform.h"
#include "renderer/fanRenderer.h"

namespace fan
{
	REGISTER_EDITOR_COMPONENT( SphereShape );
	REGISTER_TYPE_INFO( SphereShape )

	//================================================================================================================================
	//================================================================================================================================
	void  SphereShape::SetRadius( const float _radius ) {
		GetEcsSphereShape()->Get().setUnscaledRadius( _radius >= 0 ? _radius : 0.f );
	}

	//================================================================================================================================
	//================================================================================================================================
	float SphereShape::GetRadius() const {
		return GetEcsSphereShape()->Get().getRadius();
	}

	//================================================================================================================================
	//================================================================================================================================
	void SphereShape::OnAttach() {
		ecsSphereShape * sphereShape = GetGameobject()->AddEcsComponent<ecsSphereShape>();
		sphereShape->Init(1.f);

		ColliderShape::OnAttach();
	}

	//================================================================================================================================
	//================================================================================================================================
	void SphereShape::OnDetach() {
		GetGameobject()->RemoveEcsComponent<ecsSphereShape>();

		ColliderShape::OnDetach();
	}


	//================================================================================================================================
	//================================================================================================================================
	void SphereShape::OnGui() {
		Component::OnGui();

		float radius = GetRadius();
		if ( ImGui::DragFloat( "radius", &radius, 0.1f, 0.f ) ) {
			SetRadius( radius );
		}

		Debug::Render().DebugSphere( GetGameobject()->GetTransform()->GetBtTransform(), radius, 2, Color::Green );
	}

	//================================================================================================================================
	//================================================================================================================================
	bool SphereShape::Load( Json & _json ) {

		float radius;
		LoadFloat(_json, "radius", radius );

		SetRadius( radius );
		return true;
	}


	//================================================================================================================================
	//================================================================================================================================
	bool SphereShape::Save( Json & _json ) const {
		SaveFloat( _json, "radius", GetRadius() );
		Component::Save( _json );
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	ecsSphereShape*		SphereShape::GetEcsSphereShape() const	{ return GetGameobject()->GetEcsComponent<ecsSphereShape>(); }
	btSphereShape *		SphereShape::GetSphereShape()			{	return &GetEcsSphereShape()->Get();	}
	btCollisionShape *	SphereShape::GetCollisionShape()		{ return GetSphereShape(); }
}