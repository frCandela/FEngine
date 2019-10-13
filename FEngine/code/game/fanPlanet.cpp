#include "fanGlobalIncludes.h"
#include "game/fanPlanet.h"

#include "core/fanTime.h"
#include "editor/windows/fanInspectorWindow.h"
#include "scene/components/fanTransform.h"
#include "scene/fanGameobject.h"

namespace fan {
	REGISTER_EDITOR_COMPONENT(Planet)
	REGISTER_TYPE_INFO(Planet)

	//================================================================================================================================
	//================================================================================================================================
	void Planet::Start() {
	}

	//================================================================================================================================
	//================================================================================================================================
	void Planet::OnAttach() {
		Actor::OnAttach();
		GetGameobject()->AddEcsComponent<ecsPlanet>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Planet::OnDetach() {
		Actor::OnDetach();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Planet::Update(const float /*_delta*/) {
		Actor::OnGui();

		// Updates the parent entity
		ecsPlanet * planet = GetEcsPlanet();
		ecsHandle handle = GetGameobject()->GetParent()->GetEcsHandle();
		GetGameobject()->GetScene()->GetEcsManager()->FindEntity( handle , planet->parentEntity );

// 		ecsPlanet * planet = GetEcsPlanet();
// 
// 		Transform * parentTransform = GetGameobject()->GetParent()->GetComponent<Transform>();
// 
// 		float time = -planet->speed * Time::ElapsedSinceStartup();
// 		btVector3 position(std::cosf(time + planet->phase), 0, std::sinf(time + planet->phase));
// 		btVector3 parentPosition = parentTransform->GetPosition();
// 
// 		Transform * transform = GetGameobject()->GetComponent<Transform>();
// 		transform->SetPosition(parentPosition + planet->radius * position);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Planet::SetSpeed( const float _speed )		{ GetEcsPlanet()->speed = _speed; };
	void Planet::SetRadius( const float _radius )	{ GetEcsPlanet()->radius = _radius; };
	void Planet::SetPhase( const float _phase )		{ GetEcsPlanet()->phase = _phase; };

	//================================================================================================================================
	//================================================================================================================================
	void Planet::OnGui() {
		ecsPlanet * planet = GetEcsPlanet();
		ImGui::DragFloat("radius", &planet->radius, 0.1f, 0.f, 100.f);
		ImGui::DragFloat("speed", &planet->speed,   0.1f, 0.f, 10.f);
		ImGui::DragFloat("phase", &planet->phase, PI/3, 0.f, 2 * PI);
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Planet::Save(std::ostream& _out, const int _indentLevel) const {
		Actor::Save(_out, _indentLevel);
		const std::string indentation = GetIndentation(_indentLevel);
		ecsPlanet * planet = GetEcsPlanet();

		_out << indentation << "radius: " << planet->radius << std::endl;
		_out << indentation << "speed:  " << planet->speed << std::endl;
		_out << indentation << "phase:  " << planet->phase << std::endl;
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Planet::Load(std::istream& _in) {
		Actor::Load(_in);
		ecsPlanet * planet = GetEcsPlanet();

		if (!ReadSegmentHeader(_in, "radius:")) { return false; }
		if (!ReadFloat(_in, planet->radius)) { return false; }

		if (!ReadSegmentHeader(_in, "speed:")) { return false; }
		if (!ReadFloat(_in, planet->speed)) { return false; }

		if (!ReadSegmentHeader(_in, "phase:")) { return false; }
		if (!ReadFloat(_in, planet->phase)) { return false; }
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	ecsPlanet* Planet::GetEcsPlanet() const { return GetGameobject()->GetEcsComponent<ecsPlanet>(); }
}