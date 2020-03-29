#include "game/components/fanPlanet.hpp"

#include "scene/fanSceneSerializable.hpp"
#include "render/fanRendererDebug.hpp"

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
	void Planet::Init( EcsWorld& _world, Component& _component )
	{
		Planet& planet = static_cast<Planet&>( _component );
		planet.timeAccumulator = 0.f;
		planet.speed = 1.f;
		planet.radius = 2.f;
		planet.phase = 0.f;		
	}

	//================================================================================================================================
	//================================================================================================================================
	void Planet::OnGui( EcsWorld& _world, EntityID _entityID, Component& _component )
	{
		Planet& planet = static_cast<Planet&>( _component );
		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			ImGui::DragFloat( "radius", &planet.radius, 0.1f, 0.f, 100.f );
			ImGui::DragFloat( "speed", &planet.speed, 0.1f, 0.f, 10.f );
			ImGui::DragFloat( "phase", &planet.phase, PI / 3, 0.f, 2 * PI );
		}

		RendererDebug::Get().DebugCircle( btVector3::Zero(), planet.radius, btVector3::Up(), 32, Color::Cyan );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Planet::Save( const Component& _component, Json& _json )
	{
		const Planet& planet = static_cast<const Planet&>( _component );
		Serializable::SaveFloat( _json, "radius", planet.radius );
		Serializable::SaveFloat( _json, "speed",  planet.speed );
		Serializable::SaveFloat( _json, "phase",  planet.phase );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Planet::Load( Component& _component, const Json& _json )
	{
		Planet& planet = static_cast<Planet&>( _component );
		Serializable::LoadFloat( _json, "radius", planet.radius );
		Serializable::LoadFloat( _json, "speed",  planet.speed );
		Serializable::LoadFloat( _json, "phase",  planet.phase );
	}
}