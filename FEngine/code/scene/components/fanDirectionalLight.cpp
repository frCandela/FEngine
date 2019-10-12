#include "fanGlobalIncludes.h"
#include "scene/components/fanDirectionalLight.h"

#include "scene/components/fanTransform.h"	
#include "scene/fanGameobject.h"
#include "renderer/fanRenderer.h"
#include "editor/fanModals.h"

namespace fan {
	REGISTER_EDITOR_COMPONENT( DirectionalLight );
	REGISTER_TYPE_INFO( DirectionalLight )

	Signal< DirectionalLight * > DirectionalLight::onDirectionalLightAttach;
	Signal< DirectionalLight * > DirectionalLight::onDirectionalLightDetach;

	//================================================================================================================================
	//================================================================================================================================
	Color DirectionalLight::GetAmbiant() const { return GetEcsDirLight()->ambiant; }
	Color DirectionalLight::GetDiffuse() const { return GetEcsDirLight()->diffuse; }
	Color DirectionalLight::GetSpecular() const { return GetEcsDirLight()->specular; }

	//================================================================================================================================
	//================================================================================================================================
	void DirectionalLight::SetAmbiant( const Color _ambiant ) {
		GetEcsDirLight()->ambiant = _ambiant;
		MarkModified();
	}

	//================================================================================================================================
	//================================================================================================================================
	void DirectionalLight::SetDiffuse( const Color _diffuse ) {
		GetEcsDirLight()->diffuse = _diffuse;
		MarkModified();
	}

	//================================================================================================================================
	//================================================================================================================================
	void DirectionalLight::SetSpecular( const Color _specular ) {
		GetEcsDirLight()->specular = _specular;
		MarkModified();
	}

	//================================================================================================================================
	//================================================================================================================================
	void DirectionalLight::OnDetach() {
		Component::OnDetach();
		onDirectionalLightDetach.Emmit( this );
	}

	//================================================================================================================================
	//================================================================================================================================
	void DirectionalLight::OnAttach() {
		Component::OnAttach();
		GetGameobject()->AddEcsComponent<ecsDirLight>();
		onDirectionalLightAttach.Emmit( this );
	}

	//================================================================================================================================
	//================================================================================================================================
	void DirectionalLight::OnGui() {
		Component::OnGui();
		ecsDirLight * dirLight = GetEcsDirLight();

		// Filter color
		if ( ImGui::Button( "##ambiant" ) ) { SetAmbiant( Color::Black ); } ImGui::SameLine();
		if ( ImGui::ColorEdit3( "ambiant", dirLight->ambiant.Data(), gui::colorEditFlags ) ) { MarkModified(); }
		if ( ImGui::Button( "##diffuse" ) ) { SetDiffuse( Color::Black ); } ImGui::SameLine();
		if ( ImGui::ColorEdit3( "diffuse", dirLight->diffuse.Data(), gui::colorEditFlags ) ) { MarkModified(); }
		if ( ImGui::Button( "##specular" ) ) { SetSpecular( Color::Black ); } ImGui::SameLine();
		if ( ImGui::ColorEdit3( "specular", dirLight->specular.Data(), gui::colorEditFlags ) ) { MarkModified(); }


		const Transform * transform = GetGameobject()->GetComponent<Transform>();
		const btVector3 pos = transform->GetPosition();
		const btVector3 dir = transform->Forward();
		const btVector3 up = transform->Up();
		const btVector3 left = transform->Left();
		const float length = 2.f;
		const float radius = 0.5f;
		const Color color = Color::Yellow;
		btVector3 offsets[5] = { btVector3::Zero(), radius * up ,-radius * up, radius * left ,-radius * left };
		for (int offsetIndex = 0; offsetIndex < 5 ; offsetIndex++) {
			const btVector3 offset = offsets[offsetIndex];
			Debug::Render().DebugLine( pos + offset, pos + offset + length * dir, color );
		}
		Debug::Render().DebugSphere( transform->GetBtTransform(), radius,0, color );
	}

	//================================================================================================================================
	//================================================================================================================================
	bool DirectionalLight::Load( std::istream& _in ) {
		ecsDirLight * dirLight = GetEcsDirLight();

		if ( !ReadSegmentHeader( _in, "ambiant:" ) ) { return false; }
		if ( !ReadFloat3( _in, &dirLight->ambiant[0] ) ) { return false; }
		if ( !ReadSegmentHeader( _in, "diffuse:" ) ) { return false; }
		if ( !ReadFloat3( _in, &dirLight->diffuse[0] ) ) { return false; }
		if ( !ReadSegmentHeader( _in, "specular:" ) ) { return false; }
		if ( !ReadFloat3( _in, &dirLight->specular[0] ) ) { return false; }
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool DirectionalLight::Save( std::ostream& _out, const int _indentLevel ) const {
		ecsDirLight * dirLight = GetEcsDirLight();
		const std::string indentation = GetIndentation( _indentLevel );
		_out << indentation << "ambiant: " << dirLight->ambiant[0] << " " << dirLight->ambiant[1] << " " << dirLight->ambiant[2] << std::endl;
		_out << indentation << "diffuse: " << dirLight->diffuse[0] << " " << dirLight->diffuse[1] << " " << dirLight->diffuse[2] << std::endl;
		_out << indentation << "specular: " << dirLight->specular[0] << " " << dirLight->specular[1] << " " << dirLight->specular[2] << std::endl;
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	ecsDirLight* DirectionalLight::GetEcsDirLight() const { return GetGameobject()->GetEcsComponent<ecsDirLight>(); }
}