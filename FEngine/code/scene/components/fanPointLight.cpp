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
		m_ambiant = _ambiant;
		MarkModified();
	}

	//================================================================================================================================
	//================================================================================================================================
	void PointLight::SetDiffuse(const Color _diffuse) {
		m_diffuse = _diffuse;
		MarkModified();
	}	
	
	//================================================================================================================================
	//================================================================================================================================
	void PointLight::SetSpecular(const Color _specular) {
		m_specular = _specular;
		MarkModified();
	}

	//================================================================================================================================
	//================================================================================================================================
	void PointLight::SetAttenuation(const Attenuation _attenuation, const float _value) {
		if (_value >= 0) {
			m_attenuation[_attenuation] = _value;
			MarkModified();
		}
		else {
			Debug::Warning("Light attenuation cannot be negative");
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void PointLight::OnDetach() {
		Component::OnDetach();
		onPointLightDetach.Emmit(this);
	}

	//================================================================================================================================
	//================================================================================================================================
	void PointLight::OnAttach() {
		Component::OnAttach();
		m_ambiant  = Color::White ;
		m_diffuse  = Color::White ;
		m_specular = Color::White ;
		m_attenuation[Attenuation::CONSTANT] = 0;
		m_attenuation[Attenuation::LINEAR] = 0;
		m_attenuation[Attenuation::QUADRATIC] = 0.1f;
		onPointLightAttach.Emmit(this);
	}

	//================================================================================================================================
	//================================================================================================================================
	void PointLight::OnGui() {
		Component::OnGui();
		// Filter color
		if (ImGui::Button("##ambiant")) { SetAmbiant(Color(0.0f)); } ImGui::SameLine();
		if (ImGui::ColorEdit3("ambiant", m_ambiant.Data(), gui::colorEditFlags)) { MarkModified(); }
		if (ImGui::Button("##diffuse")) { SetDiffuse(Color::White); } ImGui::SameLine();
		if (ImGui::ColorEdit3("diffuse", m_diffuse.Data(), gui::colorEditFlags)) { MarkModified(); }
		if (ImGui::Button("##specular")) { SetSpecular(Color::White); } ImGui::SameLine();
		if (ImGui::ColorEdit3("specular", m_specular.Data(), gui::colorEditFlags)) { MarkModified(); }
		// Attenuation
		
		ImGui::Text("attenuation :");  
		ImGui::SameLine();  
		gui::ShowHelpMarker(
			"Light intensity fades out with distance \n"
			"Fadings follows the following formula : \n"
			"constant + linear * d + quadratic * d*d  \n"
			"(d=distance)");
		if (ImGui::Button("##constant attenuation")) { SetAttenuation(Attenuation::CONSTANT, 0.f ); }	ImGui::SameLine();
		if (ImGui::DragFloat("constant",  &m_attenuation[Attenuation::CONSTANT],0.05f, 0.f, 100.f)) { MarkModified(); }		
		if (ImGui::Button("##linear attenuation")) { SetAttenuation(Attenuation::LINEAR, 0.f); }	ImGui::SameLine();
		if (ImGui::DragFloat("linear",	  &m_attenuation[Attenuation::LINEAR], 0.05f, 0.f, 100.f)) { MarkModified(); }
		if (ImGui::Button("##quadratic attenuation")) { SetAttenuation(Attenuation::QUADRATIC, 0.f); }	ImGui::SameLine();
		if (ImGui::DragFloat("quadratic", &m_attenuation[Attenuation::QUADRATIC], 0.05f, 0.f, 100.f)) { MarkModified(); }
		
 		// Sphere gizmo
		float lightRange = GetLightRange();
		if (lightRange > 0 ) {
			const btTransform transform = GetGameobject()->GetComponent<Transform>()->GetBtTransform();
			Debug::Render().DebugSphere(transform, lightRange, 2, m_diffuse);
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	float PointLight::GetLightRange() const {
		const float epsilon = 0.1f;	// Value at which we consider the light value null
		const float q = m_attenuation[2];
		const float s = m_attenuation[0];
		const float L = m_attenuation[1];
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
	bool PointLight::Load(std::istream& _in) {
		if (!ReadSegmentHeader(_in, "ambiant:")) { return false; }
		if (!ReadFloat3(_in, &m_ambiant[0])) { return false; }
		if (!ReadSegmentHeader(_in, "diffuse:")) { return false; }
		if (!ReadFloat3(_in, &m_diffuse[0])) { return false; }
		if (!ReadSegmentHeader(_in, "specular:")) { return false; }
		if (!ReadFloat3(_in, &m_specular[0])) { return false; }
		if (!ReadSegmentHeader(_in, "attenuation:")) { return false; }
		if (!ReadFloat3(_in, &m_attenuation[0])) { return false; }
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool PointLight::Save(std::ostream& _out, const int _indentLevel) const {
		const std::string indentation = GetIndentation(_indentLevel);
		_out << indentation << "ambiant: " << m_ambiant[0] << " " << m_ambiant[1] << " " << m_ambiant[2] << std::endl;
		_out << indentation << "diffuse: " << m_diffuse[0] << " " << m_diffuse[1] << " " << m_diffuse[2] << std::endl;
		_out << indentation << "specular: " << m_specular[0] << " " << m_specular[1] << " " << m_specular[2] << std::endl;
		_out << indentation << "attenuation: " << m_attenuation[0] << " " << m_attenuation[1] << " " << m_attenuation[2] << std::endl;
		return true;
	}
}