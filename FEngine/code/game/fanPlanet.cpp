#include "fanGlobalIncludes.h"
#include "game/fanPlanet.h"

#include "core/fanTime.h"
#include "editor/windows/fanInspectorWindow.h"
#include "scene/components/fanTransform.h"
#include "scene/fanEntity.h"

namespace fan {
	REGISTER_EDITOR_COMPONENT(Planet);
	REGISTER_TYPE_INFO(Planet)

		//================================================================================================================================
		//================================================================================================================================
		void Planet::Start() {
		Debug::Log("Start planet");
	}

	//================================================================================================================================
	//================================================================================================================================
	void Planet::Update(const float /*_delta*/) {
		Transform * transform = GetEntity()->GetComponent<Transform>();

		float time = m_speed * Time::ElapsedSinceStartup();
		btVector3 position(std::cosf(time), 0, std::sinf(time));

		transform->SetPosition(m_radius * position);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Planet::OnGui() {
		Component::OnGui();
		ImGui::Text(GetEntity()->GetName().c_str());
		ImGui::SliderFloat("radius", &m_radius, 0.f, 100.f);
		ImGui::SliderFloat("speed", &m_speed, 0.f, 10.f);
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Planet::Load(std::istream& _in) {
		Actor::Load(_in);

		if (!ReadSegmentHeader(_in, "radius:")) { return false; }
		if (!ReadFloat(_in, m_radius)) { return false; }

		if (!ReadSegmentHeader(_in, "speed:")) { return false; }
		if (!ReadFloat(_in, m_speed)) { return false; }
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Planet::Save(std::ostream& _out, const int _indentLevel) const {
		Actor::Save(_out, _indentLevel);
		const std::string indentation = GetIndentation(_indentLevel);
		_out << indentation << "radius: " << m_radius << std::endl;
		_out << indentation << "speed:  " << m_speed << std::endl;
		return true;
	}
}