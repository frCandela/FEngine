#include "fanGlobalIncludes.h"
#include "scene/components/fanDirectionalLight.h"

#include "scene/components/fanTransform.h"	
#include "scene/fanEntity.h"
#include "renderer/fanRenderer.h"
#include "editor/fanModals.h"

namespace fan {
	REGISTER_EDITOR_COMPONENT( DirectionalLight );
	REGISTER_TYPE_INFO( DirectionalLight )

	Signal< DirectionalLight * > DirectionalLight::onDirectionalLightAttach;
	Signal< DirectionalLight * > DirectionalLight::onDirectionalLightDetach;

	//================================================================================================================================
	//================================================================================================================================
	void DirectionalLight::SetAmbiant( const Color _ambiant ) {
		m_ambiant = _ambiant;
		MarkModified();
	}

	//================================================================================================================================
	//================================================================================================================================
	void DirectionalLight::SetDiffuse( const Color _diffuse ) {
		m_diffuse = _diffuse;
		MarkModified();
	}

	//================================================================================================================================
	//================================================================================================================================
	void DirectionalLight::SetSpecular( const Color _specular ) {
		m_specular = _specular;
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
		m_ambiant  = Color::Black;
		m_diffuse  = Color::White;
		m_specular = Color::White;
		onDirectionalLightAttach.Emmit( this );
	}

	//================================================================================================================================
	//================================================================================================================================
	void DirectionalLight::OnGui() {
		Component::OnGui();
		// Filter color
		if ( ImGui::Button( "##ambiant" ) ) { SetAmbiant( Color::Black ); } ImGui::SameLine();
		if ( ImGui::ColorEdit3( "ambiant", m_ambiant.Data(), gui::colorEditFlags ) ) { MarkModified(); }
		if ( ImGui::Button( "##diffuse" ) ) { SetDiffuse( Color::Black ); } ImGui::SameLine();
		if ( ImGui::ColorEdit3( "diffuse", m_diffuse.Data(), gui::colorEditFlags ) ) { MarkModified(); }
		if ( ImGui::Button( "##specular" ) ) { SetSpecular( Color::Black ); } ImGui::SameLine();
		if ( ImGui::ColorEdit3( "specular", m_specular.Data(), gui::colorEditFlags ) ) { MarkModified(); }


		const Transform * transform = GetEntity()->GetComponent<Transform>();
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
		if ( !ReadSegmentHeader( _in, "ambiant:" ) ) { return false; }
		if ( !ReadFloat3( _in, &m_ambiant[0] ) ) { return false; }
		if ( !ReadSegmentHeader( _in, "diffuse:" ) ) { return false; }
		if ( !ReadFloat3( _in, &m_diffuse[0] ) ) { return false; }
		if ( !ReadSegmentHeader( _in, "specular:" ) ) { return false; }
		if ( !ReadFloat3( _in, &m_specular[0] ) ) { return false; }
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool DirectionalLight::Save( std::ostream& _out, const int _indentLevel ) const {
		const std::string indentation = GetIndentation( _indentLevel );
		_out << indentation << "ambiant: " << m_ambiant[0] << " " << m_ambiant[1] << " " << m_ambiant[2] << std::endl;
		_out << indentation << "diffuse: " << m_diffuse[0] << " " << m_diffuse[1] << " " << m_diffuse[2] << std::endl;
		_out << indentation << "specular: " << m_specular[0] << " " << m_specular[1] << " " << m_specular[2] << std::endl;
		return true;
	}
}