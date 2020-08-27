#include "game/components/fanPlanet.hpp"

#include "ecs/fanEcsWorld.hpp"
#include "scene/singletons/fanRenderDebug.hpp"
#include "scene/fanSceneSerializable.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void Planet::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::PLANET16;
		_info.group = EngineGroups::Game;
		_info.onGui = &Planet::OnGui;
		_info.load = &Planet::Load;
		_info.save = &Planet::Save;
		_info.editorPath = "game/";
		_info.name = "planet";
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

    //========================================================================================================
    //========================================================================================================
    void Planet::OnGui( EcsWorld& _world, EcsEntity /*_entityID*/, EcsComponent& _component )
    {
        Planet& planet = static_cast<Planet&>( _component );
        ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
        {
            ImGui::DragFloat( "radius", &planet.mRadius, 0.1f, 0.f, 100.f );
            ImGui::DragFloat( "speed", &planet.mSpeed, 0.1f, 0.f, 10.f );
            ImGui::DragFloat( "phase", &planet.mPhase, SIMD_PI / 3, 0.f, 2 * SIMD_PI );
        }
        RenderDebug& renderDebug = _world.GetSingleton<RenderDebug>();
        renderDebug.DebugCircle( btVector3::Zero(), planet.mRadius, btVector3::Up(), 32, Color::Cyan );
    }
}