#include "project_spaceships/components/fanPlanet.hpp"

#include "core/ecs/fanEcsWorld.hpp"
#include "engine/singletons/fanRenderDebug.hpp"
#include "engine/fanEngineSerializable.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void Planet::SetInfo( EcsComponentInfo& _info )
	{
		_info.load        = &Planet::Load;
		_info.save        = &Planet::Save;
	}

	//========================================================================================================
	//========================================================================================================
	void Planet::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		Planet& planet = static_cast<Planet&>( _component );
		planet.mSpeed  = 1.f;
		planet.mRadius = 2.f;
		planet.mPhase  = 0.f;
	}

	//========================================================================================================
	//========================================================================================================
	void Planet::Save( const EcsComponent& _component, Json& _json )
	{
		const Planet& planet = static_cast<const Planet&>( _component );
		Serializable::SaveFloat( _json, "radius", planet.mRadius );
		Serializable::SaveFloat( _json, "speed",  planet.mSpeed );
		Serializable::SaveFloat( _json, "phase",  planet.mPhase );
	}

	//========================================================================================================
	//========================================================================================================
	void Planet::Load( EcsComponent& _component, const Json& _json )
	{
		Planet& planet = static_cast<Planet&>( _component );
		Serializable::LoadFloat( _json, "radius", planet.mRadius );
		Serializable::LoadFloat( _json, "speed",  planet.mSpeed );
		Serializable::LoadFloat( _json, "phase",  planet.mPhase );
	}
}