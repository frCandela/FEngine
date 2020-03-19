#include "game/components/fanPlanet.hpp"

#include "game/fanGameSerializable.hpp"

namespace fan
{
	REGISTER_COMPONENT( Planet, "planet" );

	//================================================================================================================================
	//================================================================================================================================
	void Planet::SetInfo( ComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::PLANET16;
		_info.onGui = &Planet::OnGui;
		_info.init = &Planet::Init;
		_info.load = &Planet::Load;
		_info.save = &Planet::Save;
		_info.editorPath = "game/";
	}

	//================================================================================================================================
	//================================================================================================================================
	void Planet::Init( Component& _component )
	{
		Planet& pointLight = static_cast<Planet&>( _component );
		pointLight.timeAccumulator = 0.f;
		pointLight.speed = 1.f;
		pointLight.radius = 2.f;
		pointLight.phase = 0.f;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Planet::OnGui( Component& _planet )
	{
		Planet& planet = static_cast<Planet&>( _planet );
		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			ImGui::DragFloat( "radius", &planet.radius, 0.1f, 0.f, 100.f );
			ImGui::DragFloat( "speed", &planet.speed, 0.1f, 0.f, 10.f );
			ImGui::DragFloat( "phase", &planet.phase, PI / 3, 0.f, 2 * PI );
			//ImGui::DragFloat( "time", &m_planet->time );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Planet::Save( const Component& _planet, Json& _json )
	{
		const Planet& planet = static_cast<const Planet&>( _planet );
		Serializable::SaveFloat( _json, "radius", planet.radius );
		Serializable::SaveFloat( _json, "speed",  planet.speed );
		Serializable::SaveFloat( _json, "phase",  planet.phase );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Planet::Load( Component& _planet, const Json& _json )
	{
		Planet& planet = static_cast<Planet&>( _planet );
		Serializable::LoadFloat( _json, "radius", planet.radius );
		Serializable::LoadFloat( _json, "speed",  planet.speed );
		Serializable::LoadFloat( _json, "phase",  planet.phase );
	}
}