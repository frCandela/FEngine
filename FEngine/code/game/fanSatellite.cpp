#include "fanGlobalIncludes.h"
#include "game/fanSatellite.h"

#include "core/fanTime.h"
#include "editor/windows/fanInspectorWindow.h"
#include "scene/components/fanTransform.h"
#include "scene/fanEntity.h"

namespace fan {
	REGISTER_EDITOR_COMPONENT(Satellite)
		REGISTER_TYPE_INFO(Satellite)

		//================================================================================================================================
		//================================================================================================================================
		void Satellite::Start() {
		Debug::Log("Start satellite");
	}

	//================================================================================================================================
	//================================================================================================================================
	void Satellite::Update(const float /*_delta*/) {
		Actor::OnGui();

		Transform * parentTransform = GetEntity()->GetParent()->GetComponent<Transform>();

		float time = -m_speed * Time::ElapsedSinceStartup();
		btVector3 position(std::cosf(time), 0, std::sinf(time));
		btVector3 parentPosition = parentTransform->GetPosition();

		Transform * transform = GetEntity()->GetComponent<Transform>();
		transform->SetPosition(parentPosition + m_radius * position);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Satellite::OnGui() {
		ImGui::SliderFloat("radius", &m_radius, 0.f, 100.f);
		ImGui::SliderFloat("speed", &m_speed, 0.f, 10.f);
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Satellite::Load(std::istream& _in) {
		Actor::Load(_in);

		if (!ReadSegmentHeader(_in, "radius:")) { return false; }
		if (!ReadFloat(_in, m_radius)) { return false; }

		if (!ReadSegmentHeader(_in, "speed:")) { return false; }
		if (!ReadFloat(_in, m_speed)) { return false; }
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Satellite::Save(std::ostream& _out, const int _indentLevel) const {
		Actor::Save(_out, _indentLevel);
		const std::string indentation = GetIndentation(_indentLevel);
		_out << indentation << "radius: " << m_radius << std::endl;
		_out << indentation << "speed:  " << m_speed << std::endl;
		return true;
	}
}