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
		GetEcsPointLight()->ambiant = _ambiant;
		MarkModified();
	}

	//================================================================================================================================
	//================================================================================================================================
	void PointLight::SetDiffuse(const Color _diffuse) {
		GetEcsPointLight()->diffuse = _diffuse;
		MarkModified();
	}	
	
	//================================================================================================================================
	//================================================================================================================================
	void PointLight::SetSpecular(const Color _specular) {
		GetEcsPointLight()->specular = _specular;
		MarkModified();
	}

	//================================================================================================================================
	//================================================================================================================================
	void PointLight::SetAttenuation(const Attenuation _attenuation, const float _value) {
		if (_value >= 0) {
			GetEcsPointLight()->attenuation[_attenuation] = _value;
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
		GetGameobject()->AddEcsComponent<ecsPointLight>();
		onPointLightAttach.Emmit(this);
	}
	
	//================================================================================================================================
	//================================================================================================================================
	Color		PointLight::GetAmbiant() const {  return GetEcsPointLight()->ambiant; }
	Color		PointLight::GetDiffuse() const {  return GetEcsPointLight()->diffuse; }
	Color		PointLight::GetSpecular() const { return GetEcsPointLight()->specular; }
	float		PointLight::GetAttenuation( const Attenuation _attenuation ) const { return GetEcsPointLight()->attenuation[_attenuation]; }
	glm::vec3	PointLight::GetAttenuation() const { 
		ecsPointLight * pointLight = GetEcsPointLight();
		return glm::vec3( pointLight->attenuation[0], pointLight->attenuation[1], pointLight->attenuation[2] );
	}

	//================================================================================================================================
	//================================================================================================================================
	void PointLight::OnGui() {
		Component::OnGui();
		ecsPointLight * ecsLight = GetEcsPointLight();

		// Filter color
		if (ImGui::Button("##ambiant")) { SetAmbiant(Color(0.0f)); } ImGui::SameLine();
		if (ImGui::ColorEdit3("ambiant", ecsLight->ambiant.Data(), gui::colorEditFlags)) { MarkModified(); }
		if (ImGui::Button("##diffuse")) { SetDiffuse(Color::White); } ImGui::SameLine();
		if (ImGui::ColorEdit3("diffuse", ecsLight->diffuse.Data(), gui::colorEditFlags)) { MarkModified(); }
		if (ImGui::Button("##specular")) { SetSpecular(Color::White); } ImGui::SameLine();
		if (ImGui::ColorEdit3("specular", ecsLight->specular.Data(), gui::colorEditFlags)) { MarkModified(); }
		// Attenuation
		
		ImGui::Text("attenuation :");  
		ImGui::SameLine();  
		gui::ShowHelpMarker(
			"Light intensity fades out with distance \n"
			"Fadings follows the following formula : \n"
			"constant + linear * d + quadratic * d*d  \n"
			"(d=distance)");
		if (ImGui::Button("##constant attenuation")) { SetAttenuation(Attenuation::CONSTANT, 0.f ); }	ImGui::SameLine();
		if (ImGui::DragFloat("constant",  &ecsLight->attenuation[Attenuation::CONSTANT],0.05f, 0.f, 100.f)) { MarkModified(); }
		if (ImGui::Button("##linear attenuation")) { SetAttenuation(Attenuation::LINEAR, 0.f); }	ImGui::SameLine();
		if (ImGui::DragFloat("linear",	  &ecsLight->attenuation[Attenuation::LINEAR], 0.05f, 0.f, 100.f)) { MarkModified(); }
		if (ImGui::Button("##quadratic attenuation")) { SetAttenuation(Attenuation::QUADRATIC, 0.f); }	ImGui::SameLine();
		if (ImGui::DragFloat("quadratic", &ecsLight->attenuation[Attenuation::QUADRATIC], 0.05f, 0.f, 100.f)) { MarkModified(); }
		
 		// Sphere gizmo
		float lightRange = GetLightRange();
		if (lightRange > 0 ) {
			const btTransform transform = GetGameobject()->GetComponent<Transform>()->GetBtTransform();
			Debug::Render().DebugSphere(transform, lightRange, 2, ecsLight->diffuse);
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	float PointLight::GetLightRange() const {
		ecsPointLight * ecsLight = GetEcsPointLight();

		const float epsilon = 0.1f;	// Value at which we consider the light value null
		const float q = ecsLight->attenuation[2];
		const float s = ecsLight->attenuation[0];
		const float L = ecsLight->attenuation[1];
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
		ecsPointLight * ecsLight = GetEcsPointLight();

		if (!ReadSegmentHeader(_in, "ambiant:")) { return false; }
		if (!ReadFloat3(_in, &ecsLight->ambiant[0])) { return false; }
		if (!ReadSegmentHeader(_in, "diffuse:")) { return false; }
		if (!ReadFloat3(_in, &ecsLight->diffuse[0])) { return false; }
		if (!ReadSegmentHeader(_in, "specular:")) { return false; }
		if (!ReadFloat3(_in, &ecsLight->specular[0])) { return false; }
		if (!ReadSegmentHeader(_in, "attenuation:")) { return false; }
		if (!ReadFloat3(_in, &ecsLight->attenuation[0])) { return false; }
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool PointLight::Save(std::ostream& _out, const int _indentLevel) const {
		ecsPointLight * ecsLight = GetEcsPointLight();

		const std::string indentation = GetIndentation(_indentLevel);
		_out << indentation << "ambiant: " << ecsLight->ambiant[0] << " " << ecsLight->ambiant[1] << " " << ecsLight->ambiant[2] << std::endl;
		_out << indentation << "diffuse: " << ecsLight->diffuse[0] << " " << ecsLight->diffuse[1] << " " << ecsLight->diffuse[2] << std::endl;
		_out << indentation << "specular: " << ecsLight->specular[0] << " " << ecsLight->specular[1] << " " << ecsLight->specular[2] << std::endl;
		_out << indentation << "attenuation: " << ecsLight->attenuation[0] << " " << ecsLight->attenuation[1] << " " << ecsLight->attenuation[2] << std::endl;
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	ecsPointLight* PointLight::GetEcsPointLight() const { return GetGameobject()->GetEcsComponent<ecsPointLight>(); }
}