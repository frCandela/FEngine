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
	Color DirectionalLight::GetAmbiant() const { return  m_dirLight->ambiant; }
	Color DirectionalLight::GetDiffuse() const { return  m_dirLight->diffuse; }
	Color DirectionalLight::GetSpecular() const { return m_dirLight->specular; }

	//================================================================================================================================
	//================================================================================================================================
	void DirectionalLight::SetAmbiant( const Color _ambiant ) {
		m_dirLight->ambiant = _ambiant;
		m_gameobject->AddFlag( Gameobject::Flag::OUTDATED_LIGHT );
	}

	//================================================================================================================================
	//================================================================================================================================
	void DirectionalLight::SetDiffuse( const Color _diffuse ) {
		m_dirLight->diffuse = _diffuse;
		m_gameobject->AddFlag( Gameobject::Flag::OUTDATED_LIGHT );
	}

	//================================================================================================================================
	//================================================================================================================================
	void DirectionalLight::SetSpecular( const Color _specular ) {
		m_dirLight->specular = _specular;
		m_gameobject->AddFlag( Gameobject::Flag::OUTDATED_LIGHT );
	}

	//================================================================================================================================
	//================================================================================================================================
	void DirectionalLight::OnAttach() {
		Component::OnAttach();
		
		ecsDirLight ** tmpLight = &const_cast<ecsDirLight*>( m_dirLight );
		*tmpLight = m_gameobject->AddEcsComponent<ecsDirLight>();
		m_dirLight->Init();

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

		// Filter color
		if ( ImGui::Button( "##ambiant" ) ) { SetAmbiant( Color::Black ); } ImGui::SameLine();
		if ( ImGui::ColorEdit3( "ambiant", m_dirLight->ambiant.Data(), gui::colorEditFlags ) ) { m_gameobject->AddFlag( Gameobject::Flag::OUTDATED_LIGHT ); }
		if ( ImGui::Button( "##diffuse" ) ) { SetDiffuse( Color::Black ); } ImGui::SameLine();
		if ( ImGui::ColorEdit3( "diffuse", m_dirLight->diffuse.Data(), gui::colorEditFlags ) ) { m_gameobject->AddFlag( Gameobject::Flag::OUTDATED_LIGHT ); }
		if ( ImGui::Button( "##specular" ) ) { SetSpecular( Color::Black ); } ImGui::SameLine();
		if ( ImGui::ColorEdit3( "specular", m_dirLight->specular.Data(), gui::colorEditFlags ) ) { m_gameobject->AddFlag( Gameobject::Flag::OUTDATED_LIGHT ); }


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
		LoadColor( _json, "ambiant", m_dirLight->ambiant );
		LoadColor( _json, "diffuse", m_dirLight->diffuse );
		LoadColor( _json, "specular", m_dirLight->specular );

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool DirectionalLight::Save( Json & _json  ) const {

		SaveColor( _json, "ambiant", m_dirLight->ambiant );
		SaveColor( _json, "diffuse", m_dirLight->diffuse );
		SaveColor( _json, "specular", m_dirLight->specular );
		Component::Save( _json );

		return true;
	}
}