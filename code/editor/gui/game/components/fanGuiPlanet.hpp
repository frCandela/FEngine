#prama once

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void Planet::SetInfo( EcsComponentInfo& _info )
	{
		_info.mIcon       = ImGui::IconType::Planet16;
		_info.mGroup      = EngineGroups::Game;
		_info.onGui       = &Planet::OnGui;
		_info.mEditorPath = "game/";
		_info.mName       = "planet";
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
        renderDebug.DebugCircle( btVector3::Zero(), planet.mRadius, btVector3::Up(), 32, Color::sCyan );
    }
}