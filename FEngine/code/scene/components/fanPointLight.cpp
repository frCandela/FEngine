#include "fanGlobalIncludes.h"
#include "scene/components/fanPointLight.h"
#include "editor/fanModals.h"

namespace fan
{
	namespace scene {
		REGISTER_EDITOR_COMPONENT(PointLight);
		REGISTER_TYPE_INFO(PointLight)

		Signal< PointLight * > PointLight::onPointLightAttach;
		Signal< PointLight * > PointLight::onPointLightDetach;
		
		//================================================================================================================================
		//================================================================================================================================
		void PointLight::SetColor( const Color _color ) {
			m_color = _color;
			MarkModified();
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
			if (ImGui::ColorEdit3("Color##1", m_color.Data(), gui::colorEditFlags)) {
				MarkModified();
			}
		}

		//================================================================================================================================
		//================================================================================================================================
		bool PointLight::Load(std::istream& _in) {
 			if (!ReadSegmentHeader(_in, "color:")) { return false; }
			if (!ReadFloat(_in, m_color[0])) { return false; }
			if (!ReadFloat(_in, m_color[1])) { return false; }
			if (!ReadFloat(_in, m_color[2])) { return false; }
			return true;
		}

		//================================================================================================================================
		//================================================================================================================================
		bool PointLight::Save(std::ostream& _out, const int _indentLevel) const {
 			const std::string indentation = GetIndentation(_indentLevel);
			_out << indentation << "color: " << m_color[0] << " " << m_color[1] << " " << m_color[2] << std::endl;
			return true;
		}
	}
}