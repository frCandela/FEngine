#include "game/fanSunLight.hpp"

namespace fan {
	REGISTER_TYPE_INFO( SunLight, TypeInfo::Flags::EDITOR_COMPONENT, "game/" )

	//================================================================================================================================
	//================================================================================================================================
	void SunLight::Start() {
		
	}

	//================================================================================================================================
	//================================================================================================================================
	void SunLight::OnAttach() {
		Actor::OnAttach();
	}

	//================================================================================================================================
	//================================================================================================================================
	void SunLight::OnDetach() {
		Actor::OnDetach();
	}

	//================================================================================================================================
	//================================================================================================================================
	void SunLight::Update(const float /*_delta*/) {
	}

	//================================================================================================================================
	//================================================================================================================================
	void SunLight::LateUpdate( const float /*_delta*/ )
	{}

	
	//================================================================================================================================
	//================================================================================================================================
	void SunLight::OnGui()
	{
		// Get singleton components 
		ecsSunLightMesh_s& sunLight = GetScene().GetEcsManager().GetSingletonComponents().GetComponent<ecsSunLightMesh_s>();

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			// Mesh generation
			ImGui::Checkbox( "debug draw",	&sunLight.debugDraw );
			ImGui::DragFloat( "radius",		&sunLight.radius, 0.1f );
			ImGui::DragFloat( "sub-angle",	&sunLight.subAngle, 1.f, 1.f, 180.f );
		} ImGui::PopItemWidth();
	}

	//================================================================================================================================
	//================================================================================================================================
	bool SunLight::Load( const Json & _json) 
	{
		ecsSunLightMesh_s& sunLight = GetScene().GetEcsManager().GetSingletonComponents().GetComponent<ecsSunLightMesh_s>();

		Actor::Load(_json);
		Serializable::LoadFloat( _json, "radius",		sunLight.radius );
		Serializable::LoadFloat( _json, "sub_angle",	sunLight.subAngle );

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool SunLight::Save( Json & _json ) const 
	{
		ecsSunLightMesh_s& sunLight = GetScene().GetEcsManager().GetSingletonComponents().GetComponent<ecsSunLightMesh_s>();

		Actor::Save( _json );		
		Serializable::SaveFloat( _json, "radius",		sunLight.radius );
		Serializable::SaveFloat( _json, "sub_angle",  sunLight.subAngle );
		return true;
	}
}