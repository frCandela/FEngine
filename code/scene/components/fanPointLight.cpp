#include "scene/components/fanPointLight.hpp"

#include "bullet/LinearMath/btVector3.h"
#include "core/fanSerializable.hpp"
#include "editor/fanModals.hpp"

namespace fan
{

	//================================================================================================================================
	//================================================================================================================================
	void PointLight::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::POINT_LIGHT16;
		_info.onGui = &PointLight::OnGui;
		_info.load = &PointLight::Load;
		_info.save = &PointLight::Save;
		_info.editorPath = "lights/";
		_info.name = "point light";
	}

	//================================================================================================================================
	//================================================================================================================================
	void PointLight::Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component )
	{
		PointLight& pointLight = static_cast<PointLight&>( _component );
		pointLight.ambiant = Color::White;
		pointLight.diffuse = Color::White;
		pointLight.specular = Color::White;
		pointLight.attenuation[0] = 1.0f; 
		pointLight.attenuation[1] = 0.5f;
		pointLight.attenuation[2] = 0.2f;
	}

	//================================================================================================================================
	//================================================================================================================================
	void PointLight::OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component )
	{
		PointLight& pointLight = static_cast<PointLight&>( _component );
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
	}

	//================================================================================================================================
	//================================================================================================================================
	void PointLight::Save( const EcsComponent& _component, Json& _json )
	{
		const PointLight& pointLight = static_cast<const PointLight&>( _component );
		Serializable::SaveColor( _json, "ambiant", pointLight.ambiant );
		Serializable::SaveColor( _json, "diffuse", pointLight.diffuse );
		Serializable::SaveColor( _json, "specular", pointLight.specular );
		Serializable::SaveVec3( _json, "attenuation", btVector3( pointLight.attenuation[0], pointLight.attenuation[1], pointLight.attenuation[2] ) );

	}

	//================================================================================================================================
	//================================================================================================================================
	void PointLight::Load( EcsComponent& _component, const Json& _json )
	{
		PointLight& pointLight = static_cast<PointLight&>( _component );
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