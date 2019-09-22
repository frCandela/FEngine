#include "fanGlobalIncludes.h"
#include "scene/components/fanPointLight.h"
#include "scene/components/fanTransform.h"	
#include "scene/fanEntity.h"
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
	PointLight::PointLight() {
		m_attenuation[Attenuation::CONSTANT] = 0;
		m_attenuation[Attenuation::LINEAR] = 2;
		m_attenuation[Attenuation::QUADRATIC] = 1;
	}

	//================================================================================================================================
	//================================================================================================================================
	void PointLight::SetColor(const Color _color) {
		m_color = _color;
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
		onPointLightAttach.Emmit(this);
	}

	//================================================================================================================================
	//================================================================================================================================
	void PointLight::OnGui() {
		Component::OnGui();
		// Filter color
		if (ImGui::Button("##ColorLight")) { SetColor(Color::White); } 
		ImGui::SameLine();
		if (ImGui::ColorEdit3("Color##1", m_color.Data(), gui::colorEditFlags)) { MarkModified(); }

		// Attenuation
		
		ImGui::Text("attenuation :");  
		if (ImGui::Button("##constant attenuation")) { SetAttenuation(Attenuation::CONSTANT, 0.f ); }	ImGui::SameLine();
		if (ImGui::DragFloat("constant",  &m_attenuation[Attenuation::CONSTANT],0.25f, 0.f, 100.f)) { MarkModified(); }		
		if (ImGui::Button("##linear attenuation")) { SetAttenuation(Attenuation::LINEAR, 2.f); }	ImGui::SameLine();
		if (ImGui::DragFloat("linear",	  &m_attenuation[Attenuation::LINEAR], 0.25f, 0.f, 100.f)) { MarkModified(); }
		if (ImGui::Button("##quadratic attenuation")) { SetAttenuation(Attenuation::QUADRATIC, 1.f); }	ImGui::SameLine();
		if (ImGui::DragFloat("quadratic", &m_attenuation[Attenuation::QUADRATIC], 0.25f, 0.f, 100.f)) { MarkModified(); }
		ImGui::Unindent();




		
 		// Sphere gizmo
		float lightRange = GetLightRange();
		if (lightRange > 0 ) {
			const btTransform transform = GetEntity()->GetComponent<Transform>()->GetBtTransform();
			Renderer::Get().DebugSphere(transform, lightRange, 3, m_color);
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	float PointLight::GetLightRange() const {
		const float epsilon = 0.015f;	// Value at which we consider the light value null
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
		if (!ReadSegmentHeader(_in, "color:")) { return false; }
		if (!ReadFloat3(_in, &m_color[0])) { return false; }
		if (!ReadSegmentHeader(_in, "attenuation:")) { return false; }
		if (!ReadFloat3(_in, &m_attenuation[0])) { return false; }
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool PointLight::Save(std::ostream& _out, const int _indentLevel) const {
		const std::string indentation = GetIndentation(_indentLevel);
		_out << indentation << "color: " << m_color[0] << " " << m_color[1] << " " << m_color[2] << std::endl;
		_out << indentation << "attenuation: " << m_attenuation[0] << " " << m_attenuation[1] << " " << m_attenuation[2] << std::endl;
		return true;
	}
}