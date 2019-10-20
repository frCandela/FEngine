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
		m_gameobject->AddFlag( Gameobject::Flag::OUTDATED_LIGHT );
	}

	//================================================================================================================================
	//================================================================================================================================
	void DirectionalLight::SetDiffuse( const Color _diffuse ) {
		GetEcsDirLight()->diffuse = _diffuse;
		m_gameobject->AddFlag( Gameobject::Flag::OUTDATED_LIGHT );
	}

	//================================================================================================================================
	//================================================================================================================================
	void DirectionalLight::SetSpecular( const Color _specular ) {
		GetEcsDirLight()->specular = _specular;
		m_gameobject->AddFlag( Gameobject::Flag::OUTDATED_LIGHT );
	}

	//================================================================================================================================
	//================================================================================================================================
	void DirectionalLight::OnAttach() {
		Component::OnAttach();
		m_gameobject->AddEcsComponent<ecsDirLight>()->Init();
		onDirectionalLightAttach.Emmit( this );
	}

	//================================================================================================================================
	//================================================================================================================================
	void DirectionalLight::OnDetach() {
		Component::OnDetach();
		m_gameobject->RemoveEcsComponent<ecsDirLight>();
		onDirectionalLightDetach.Emmit( this );
	}

	//================================================================================================================================
	//================================================================================================================================
	void DirectionalLight::OnGui() {
		Component::OnGui();
		ecsDirLight * dirLight = GetEcsDirLight();

		// Filter color
		if ( ImGui::Button( "##ambiant" ) ) { SetAmbiant( Color::Black ); } ImGui::SameLine();
		if ( ImGui::ColorEdit3( "ambiant", dirLight->ambiant.Data(), gui::colorEditFlags ) ) { m_gameobject->AddFlag( Gameobject::Flag::OUTDATED_LIGHT ); }
		if ( ImGui::Button( "##diffuse" ) ) { SetDiffuse( Color::Black ); } ImGui::SameLine();
		if ( ImGui::ColorEdit3( "diffuse", dirLight->diffuse.Data(), gui::colorEditFlags ) ) { m_gameobject->AddFlag( Gameobject::Flag::OUTDATED_LIGHT ); }
		if ( ImGui::Button( "##specular" ) ) { SetSpecular( Color::Black ); } ImGui::SameLine();
		if ( ImGui::ColorEdit3( "specular", dirLight->specular.Data(), gui::colorEditFlags ) ) { m_gameobject->AddFlag( Gameobject::Flag::OUTDATED_LIGHT ); }


		const Transform * transform = m_gameobject->GetComponent<Transform>();
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
	bool DirectionalLight::Load( Json & _json ) {
 		ecsDirLight * dirLight = GetEcsDirLight();

		LoadColor( _json, "ambiant", dirLight->ambiant );
		LoadColor( _json, "diffuse", dirLight->diffuse );
		LoadColor( _json, "specular", dirLight->specular );

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool DirectionalLight::Save( Json & _json  ) const {
		ecsDirLight * dirLight = GetEcsDirLight();

		SaveColor( _json, "ambiant", dirLight->ambiant );
		SaveColor( _json, "diffuse", dirLight->diffuse );
		SaveColor( _json, "specular", dirLight->specular );
		Component::Save( _json );

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	ecsDirLight* DirectionalLight::GetEcsDirLight() const { return m_gameobject->GetEcsComponent<ecsDirLight>(); }
}