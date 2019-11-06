#include "fanGlobalIncludes.h"
#include "scene/components/fanPointLight.h"
#include "scene/components/fanTransform.h"	
#include "scene/fanGameobject.h"
#include "renderer/fanRenderer.h"
#include "editor/fanModals.h"

namespace fan
{
	REGISTER_EDITOR_COMPONENT(PointLight);
	REGISTER_TYPE_INFO(PointLight)

	Signal< PointLight * > PointLight::onPointLightAttach;
	Signal< PointLight * > PointLight::onPointLightDetach;

	//================================================================================================================================
	//================================================================================================================================
	void PointLight::SetAmbiant(const Color _ambiant) {
		m_pointLight->ambiant = _ambiant;
		m_gameobject->AddFlag( Gameobject::Flag::OUTDATED_LIGHT );
	}

	//================================================================================================================================
	//================================================================================================================================
	void PointLight::SetDiffuse(const Color _diffuse) {
		m_pointLight->diffuse = _diffuse;
		m_gameobject->AddFlag( Gameobject::Flag::OUTDATED_LIGHT );
	}	
	
	//================================================================================================================================
	//================================================================================================================================
	void PointLight::SetSpecular(const Color _specular) {
		m_pointLight->specular = _specular;
		m_gameobject->AddFlag( Gameobject::Flag::OUTDATED_LIGHT );
	}

	//================================================================================================================================
	//================================================================================================================================
	void PointLight::SetAttenuation(const Attenuation _attenuation, const float _value) {
		if (_value >= 0) {
			m_pointLight->attenuation[_attenuation] = _value;
			m_gameobject->AddFlag( Gameobject::Flag::OUTDATED_LIGHT );
		}
		else {
			Debug::Warning("Light attenuation cannot be negative");
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void PointLight::OnAttach() {
		Component::OnAttach();

		ecsPointLight ** tmpLight = &const_cast<ecsPointLight*>( m_pointLight );
		*tmpLight = m_gameobject->AddEcsComponent<ecsPointLight>();
		m_pointLight->Init();

		onPointLightAttach.Emmit( this );
	}

	//================================================================================================================================
	//================================================================================================================================
	void PointLight::OnDetach() {
		Component::OnDetach();
		m_gameobject->RemoveEcsComponent<ecsPointLight>();
		onPointLightDetach.Emmit(this);
	}
	
	//================================================================================================================================
	//================================================================================================================================
	Color		PointLight::GetAmbiant() const {  return m_pointLight->ambiant; }
	Color		PointLight::GetDiffuse() const {  return m_pointLight->diffuse; }
	Color		PointLight::GetSpecular() const { return m_pointLight->specular; }
	float		PointLight::GetAttenuation( const Attenuation _attenuation ) const { return m_pointLight->attenuation[_attenuation]; }
	glm::vec3	PointLight::GetAttenuation() const { return glm::vec3( m_pointLight->attenuation[0], m_pointLight->attenuation[1], m_pointLight->attenuation[2] );	}

	//================================================================================================================================
	//================================================================================================================================
	void PointLight::OnGui() {
		Component::OnGui();

		// Filter color
		if (ImGui::Button("##ambiant")) { SetAmbiant(Color(0.0f)); } ImGui::SameLine();
		if (ImGui::ColorEdit3("ambiant", m_pointLight->ambiant.Data(), gui::colorEditFlags)) { m_gameobject->AddFlag( Gameobject::Flag::OUTDATED_LIGHT ); }
		if (ImGui::Button("##diffuse")) { SetDiffuse(Color::White); } ImGui::SameLine();
		if (ImGui::ColorEdit3("diffuse", m_pointLight->diffuse.Data(), gui::colorEditFlags)) { m_gameobject->AddFlag( Gameobject::Flag::OUTDATED_LIGHT ); }
		if (ImGui::Button("##specular")) { SetSpecular(Color::White); } ImGui::SameLine();
		if (ImGui::ColorEdit3("specular", m_pointLight->specular.Data(), gui::colorEditFlags)) { m_gameobject->AddFlag( Gameobject::Flag::OUTDATED_LIGHT ); }
		// Attenuation
		
		ImGui::Text("attenuation :");  
		ImGui::SameLine();  
		gui::ShowHelpMarker(
			"Light intensity fades out with distance \n"
			"Fadings follows the following formula : \n"
			"constant + linear * d + quadratic * d*d  \n"
			"(d=distance)");
		if (ImGui::Button("##constant attenuation")) { SetAttenuation(Attenuation::CONSTANT, 0.f ); }	ImGui::SameLine();
		if (ImGui::DragFloat("constant",  &m_pointLight->attenuation[Attenuation::CONSTANT],0.01f, 0.f, 100.f)) { m_gameobject->AddFlag( Gameobject::Flag::OUTDATED_LIGHT ); }
		if (ImGui::Button("##linear attenuation")) { SetAttenuation(Attenuation::LINEAR, 0.f); }	ImGui::SameLine();
		if (ImGui::DragFloat("linear",	  &m_pointLight->attenuation[Attenuation::LINEAR], 0.001f, 0.f, 100.f)) { m_gameobject->AddFlag( Gameobject::Flag::OUTDATED_LIGHT ); }
		if (ImGui::Button("##quadratic attenuation")) { SetAttenuation(Attenuation::QUADRATIC, 0.f); }	ImGui::SameLine();
		if (ImGui::DragFloat("quadratic", &m_pointLight->attenuation[Attenuation::QUADRATIC], 0.0001f, 0.f, 100.f)) { m_gameobject->AddFlag( Gameobject::Flag::OUTDATED_LIGHT ); }
		
 		// Sphere gizmo
		float lightRange = GetLightRange();
		if (lightRange > 0 ) {
			const btTransform transform = m_gameobject->GetComponent<Transform>()->GetBtTransform();
			Debug::Render().DebugSphere(transform, lightRange, 2, m_pointLight->diffuse);
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	float PointLight::GetLightRange() const {
		const float epsilon = 0.1f;	// Value at which we consider the light value null
		const float q = m_pointLight->attenuation[2];
		const float s = m_pointLight->attenuation[0];
		const float L = m_pointLight->attenuation[1];
		if (q < epsilon) {	// first order linear differential equation
			return ((1.f / epsilon) - s) / L;
		}
		else { // Second order linear differential equation
			float delta = L * L - 4 * q*(s - 1 / epsilon);
			float sqrtDelta = std::sqrtf(delta);
			return (-L + sqrtDelta) / (2 * q);
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	bool PointLight::Load( Json & _json ) {

		LoadColor( _json, "ambiant", m_pointLight->ambiant );
		LoadColor( _json, "diffuse", m_pointLight->diffuse );
		LoadColor( _json, "specular", m_pointLight->specular );

		btVector3 tmp;
		LoadVec3 ( _json, "attenuation", tmp );
		m_pointLight->attenuation[0] = tmp[0];
		m_pointLight->attenuation[1] = tmp[1];
		m_pointLight->attenuation[2] = tmp[2];

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool PointLight::Save( Json & _json ) const {
		SaveColor( _json, "ambiant", m_pointLight->ambiant );
		SaveColor( _json, "diffuse", m_pointLight->diffuse );
		SaveColor( _json, "specular", m_pointLight->specular );
		SaveVec3 ( _json, "attenuation", btVector3( m_pointLight->attenuation[0], m_pointLight->attenuation[1], m_pointLight->attenuation[2] ) );
		Component::Save( _json );
		
		return true;
	}
}