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
		m_gameobject->AddEcsComponent<ecsPlanet>()->Init();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Planet::OnDetach() {
		Actor::OnDetach();
		m_gameobject->RemoveEcsComponent<ecsPlanet>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Planet::Update(const float /*_delta*/) {
		Actor::OnGui();

		// Updates the parent entity
		ecsPlanet * planet = GetEcsPlanet();
		ecsHandle handle = m_gameobject->GetParent()->GetEcsHandle();
		m_gameobject->GetScene()->GetEcsManager()->FindEntity( handle , planet->parentEntity );
		m_gameobject->AddFlag(Gameobject::Flag::OUTDATED_TRANSFORM);

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
	bool Planet::Load( Json & _json) {
		Actor::Load(_json);
		ecsPlanet * planet = GetEcsPlanet();

		LoadFloat( _json, "radius", planet->radius );
		LoadFloat( _json, "speed", planet->speed );
		LoadFloat( _json, "phase", planet->phase );

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Planet::Save( Json & _json ) const {
		ecsPlanet * planet = GetEcsPlanet();

		SaveFloat( _json, "radius", planet->radius );
		SaveFloat( _json, "speed", planet->speed );
		SaveFloat( _json, "phase", planet->phase );
		Actor::Save( _json );
		
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	ecsPlanet* Planet::GetEcsPlanet() const { return m_gameobject->GetEcsComponent<ecsPlanet>(); }
}