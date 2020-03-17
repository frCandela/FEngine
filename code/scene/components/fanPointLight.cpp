#include "scene/components/fanPointLight.hpp"

#include "core/fanISerializable.hpp"

namespace fan
{
	REGISTER_COMPONENT( PointLight, "point_light" );

	//================================================================================================================================
	//================================================================================================================================
	void PointLight::SetInfo( ComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::POINT_LIGHT16;
		_info.onGui = &PointLight::OnGui;
		_info.init = &PointLight::Init;
		_info.load = &PointLight::Load;
		_info.save = &PointLight::Save;
		_info.editorPath = "lights/";
	}

	//================================================================================================================================
	//================================================================================================================================
	void PointLight::Init( Component& _component )
	{
		PointLight& pointLight = static_cast<PointLight&>( _component );
		pointLight.ambiant = Color::White;
		pointLight.diffuse = Color::White;
		pointLight.specular = Color::White;
		pointLight.attenuation[0] = 0.f; 
		pointLight.attenuation[1] = 0.f;
		pointLight.attenuation[2] = 0.1f;
	}

	//================================================================================================================================
	//================================================================================================================================
	void PointLight::OnGui( Component& _pointLight )
	{
		PointLight& pointLight = static_cast<PointLight&>( _pointLight );
		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 );
		{

			// Filter color
			if( ImGui::Button( "##ambiantcpl2" ) ) { pointLight.ambiant = Color( 0.0f ); } ImGui::SameLine();
			ImGui::ColorEdit3( "ambiant##ambiantpl2", pointLight.ambiant.Data(), ImGui::fanColorEditFlags );//) { m_gameobject->SetFlags( m_gameobject->GetFlags()& Gameobject::Flag::OUTDATED_LIGHT ); }
			if( ImGui::Button( "##diffusecpl2" ) ) { pointLight.diffuse = Color::White; } ImGui::SameLine();
			ImGui::ColorEdit3( "diffuse##diffusepl2", pointLight.diffuse.Data(), ImGui::fanColorEditFlags );// ) { m_gameobject->SetFlags( m_gameobject->GetFlags() & Gameobject::Flag::OUTDATED_LIGHT ); }
			if( ImGui::Button( "##specularcpl2" ) ) { pointLight.specular = Color::White ; } ImGui::SameLine();
			ImGui::ColorEdit3( "specular##specularpl2", pointLight.specular.Data(), ImGui::fanColorEditFlags );// ) { m_gameobject->SetFlags( m_gameobject->GetFlags() & Gameobject::Flag::OUTDATED_LIGHT ); }
			// Attenuation

			ImGui::Text( "attenuation :" );
			ImGui::SameLine();
			ImGui::FanShowHelpMarker(
				"Light intensity fades out with distance \n"
				"Fading follows the following formula : \n"
				"constant + linear * d + quadratic * d*d  \n"
				"(d=distance)" );
			if( ImGui::Button( "##constant attenuation" ) ) { pointLight.attenuation[ Attenuation::CONSTANT] = 0.f; }	ImGui::SameLine();
			ImGui::DragFloat( "constant", &pointLight.attenuation[Attenuation::CONSTANT], 0.01f, 0.f, 100.f );// ) { m_gameobject->SetFlags( m_gameobject->GetFlags() & Gameobject::Flag::OUTDATED_LIGHT ); }
			if( ImGui::Button( "##linear attenuation" ) ) { pointLight.attenuation[Attenuation::LINEAR] = 0.f; }	ImGui::SameLine();
			ImGui::DragFloat( "linear", &pointLight.attenuation[Attenuation::LINEAR], 0.001f, 0.f, 100.f );// ) { m_gameobject->SetFlags( m_gameobject->GetFlags() & Gameobject::Flag::OUTDATED_LIGHT ); }
			if( ImGui::Button( "##quadratic attenuation" ) ) { pointLight.attenuation[Attenuation::QUADRATIC] = 0.f; }	ImGui::SameLine();
			ImGui::DragFloat( "quadratic", &pointLight.attenuation[Attenuation::QUADRATIC], 0.0001f, 0.f, 100.f );// ) { m_gameobject->SetFlags( m_gameobject->GetFlags() & Gameobject::Flag::OUTDATED_LIGHT ); }

		} ImGui::PopItemWidth();

		// Sphere gizmo's
// 		float lightRange = pointLight.GetLightRange();
// 		if( lightRange > 0 )
// 		{
// 			const btTransform transform = m_gameobject->GetTransform().GetBtTransform();
// 			RendererDebug::Get().DebugSphere( transform, lightRange, 2, m_pointLight->diffuse );
// 		}

	}

	//================================================================================================================================
	//================================================================================================================================
	void PointLight::Save( const Component& _pointLight, Json& _json )
	{
		const PointLight& pointLight = static_cast<const PointLight&>( _pointLight );
		Serializable::SaveColor( _json, "ambiant", pointLight.ambiant );
		Serializable::SaveColor( _json, "diffuse", pointLight.diffuse );
		Serializable::SaveColor( _json, "specular", pointLight.specular );
		Serializable::SaveVec3( _json, "attenuation", btVector3( pointLight.attenuation[0], pointLight.attenuation[1], pointLight.attenuation[2] ) );

	}

	//================================================================================================================================
	//================================================================================================================================
	void PointLight::Load( Component& _pointLight, const Json& _json )
	{
		PointLight& pointLight = static_cast<PointLight&>( _pointLight );
		Serializable::LoadColor( _json, "ambiant", pointLight.ambiant );
		Serializable::LoadColor( _json, "diffuse", pointLight.diffuse );
		Serializable::LoadColor( _json, "specular", pointLight.specular );

		btVector3 tmp;
		Serializable::LoadVec3( _json, "attenuation", tmp );
		pointLight.attenuation[0] = tmp[0];
		pointLight.attenuation[1] = tmp[1];
		pointLight.attenuation[2] = tmp[2];
	}

	//================================================================================================================================
	//================================================================================================================================
	float PointLight::GetLightRange( const PointLight& _light )
	{
		const float epsilon = 0.1f;	// Value at which we consider the light value null
		const float q = _light.attenuation[2];
		const float s = _light.attenuation[0];
		const float L = _light.attenuation[1];
		if ( q < epsilon )
		{	// first order linear differential equation
			return ( ( 1.f / epsilon ) - s ) / L;
		}
		else
		{ // Second order linear differential equation
			float delta = L * L - 4 * q * ( s - 1 / epsilon );
			float sqrtDelta = std::sqrtf( delta );
			return ( -L + sqrtDelta ) / ( 2 * q );
		}
	}
}