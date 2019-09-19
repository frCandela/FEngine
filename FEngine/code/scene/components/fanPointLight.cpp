#include "fanGlobalIncludes.h"
#include "scene/components/fanPointLight.h"

namespace fan
{
	namespace scene {
		REGISTER_EDITOR_COMPONENT(PointLight);
		REGISTER_TYPE_INFO(PointLight)

		fan::Signal< PointLight * > PointLight::onPointLightAttach;
		fan::Signal< PointLight * > PointLight::onPointLightDetach;

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
		}

		//================================================================================================================================
		//================================================================================================================================
		bool PointLight::Load(std::istream& /*_in*/) {
// 			if (!ReadSegmentHeader(_in, "material:")) { return false; }
// 			if (!ReadString(_in, path)) { return false; }
			return true;
		}

		//================================================================================================================================
		//================================================================================================================================
		bool PointLight::Save(std::ostream& /*_out*/, const int /*_indentLevel*/) const {
// 			const std::string indentation = GetIndentation(_indentLevel);
// 			_out << indentation << "material: " << (m_texture != nullptr ? m_texture->GetPath() : "void") << std::endl;
			return true;
		}
	}
}