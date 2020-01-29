#include "game/components/fanPlanet.hpp"
#include "scene/components/fanTransform.hpp"
#include "core/time/fanTime.hpp"


namespace fan {
	REGISTER_TYPE_INFO(Planet, TypeInfo::Flags::EDITOR_COMPONENT, "game/")

	//================================================================================================================================
	//================================================================================================================================
	void Planet::Start() 
	{
		m_planet->time = 0.f;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Planet::OnAttach() {
		Actor::OnAttach();

		ecsPlanet ** tmpPlanet = &const_cast<ecsPlanet*>( m_planet );
		*tmpPlanet = m_gameobject->AddEcsComponent<ecsPlanet>();
		m_planet->Init();
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

	}

	//================================================================================================================================
	//================================================================================================================================
	void Planet::SetSpeed( const float _speed )		{ m_planet->speed = _speed; };
	void Planet::SetRadius( const float _radius )	{ m_planet->radius = _radius; };
	void Planet::SetPhase( const float _phase )		{ m_planet->phase = _phase; };

	//================================================================================================================================
	//================================================================================================================================
	void Planet::OnGui()
	{
		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			ImGui::DragFloat( "radius", &m_planet->radius, 0.1f, 0.f, 100.f );
			ImGui::DragFloat( "speed", &m_planet->speed, 0.1f, 0.f, 10.f );
			ImGui::DragFloat( "phase", &m_planet->phase, PI / 3, 0.f, 2 * PI );
			ImGui::DragFloat( "time", &m_planet->time );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Planet::Load( const Json & _json) {
		Actor::Load(_json);

		Serializable::LoadFloat( _json, "radius", m_planet->radius );
		Serializable::LoadFloat( _json, "speed",  m_planet->speed );
		Serializable::LoadFloat( _json, "phase",  m_planet->phase );

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Planet::Save( Json & _json ) const {

		Serializable::SaveFloat( _json, "radius", m_planet->radius );
		Serializable::SaveFloat( _json, "speed",  m_planet->speed );
		Serializable::SaveFloat( _json, "phase",  m_planet->phase );
		Actor::Save( _json );
		
		return true;
	}
}